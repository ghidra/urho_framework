#include <Urho3D/Urho3D.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Serializable.h>

#include <Urho3D/UI/UI.h>
#include <Urho3D/Input/Input.h>

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>

#include "CameraLogic.h"

#include <Urho3D/DebugNew.h>
#include <Urho3D/IO/Log.h>

CameraLogic::CameraLogic(Context* context) :
    LogicComponent(context),
    cameraDistanceMin_(1.0f),
    cameraDistanceMax_(50.0f),
    cameraDistanceIni_(20.0f),
    targetOffset_(Vector3(0.0f,0.0f,0.0f)),
    outDirection_(Vector3(0.0f,0.0f,-1.0f)),
    outDirectionOrientation_(Quaternion(30.0f,0.0f,0.0f)),
    cameraDistance_(20.0f),
    cameraType_(String("default"))
{
    // Only the scene update event is needed: unsubscribe from the rest for optimization
    //context->RegisterFactory<CameraLogic>();
    //post update appears to give the smoothest results
    SetUpdateEventMask(USE_POSTUPDATE);
}

void CameraLogic::RegisterObject(Context* context)
{
    context->RegisterFactory<CameraLogic>("Framework");

    //URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Target", GetCameraTarget, SetCameraTarget, ResourceRef, ResourceRef(Node::GetTypeStatic()), AM_DEFAULT);
    //URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Target Offset", GetTargetOffset, SetTargetOffset, Vector3, Vector3::ZERO, AM_FILE);
    URHO3D_ACCESSOR_ATTRIBUTE("Camera Distance", GetCameraDistance, SetCameraDistance, float, 20.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Camera Distance Min", GetCameraDistanceMin, SetCameraDistanceMin, float, 1.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Camera Distance Max", GetCameraDistanceMax, SetCameraDistanceMax, float, 50.0f, AM_DEFAULT);
}
void CameraLogic::OnSetAttribute(const AttributeInfo& attr, const Variant& src)
{
    Serializable::OnSetAttribute(attr, src);
}
void CameraLogic::SetCameraParameters(const float distance, const float distance_min, const float distance_max, const Quaternion orientation)
{
    cameraDistance_ = distance;
    cameraDistanceMin_ = distance_min;
    cameraDistanceMax_ = distance_max;
    cameraDistanceIni_ = distance;
    outDirectionOrientation_ = orientation;
}

void CameraLogic::SetCameraParameters( VariantMap& parms)
{
    if( parms.Contains("targetOffset") ) targetOffset_ = parms["targetOffset"].GetVector3();
    if( parms.Contains("distance") ) cameraDistance_ = parms["distance"].GetFloat();
    if( parms.Contains("distance_min") ) cameraDistanceMin_ = parms["distance_min"].GetFloat();
    if( parms.Contains("distance_max") ) cameraDistanceMax_ = parms["distance_max"].GetFloat();
    if( parms.Contains("orientation") ) outDirectionOrientation_ = parms["orientation"].GetQuaternion();
    //cameraDistance_ = distance;
    //cameraDistanceMin_ = distance_min;
    //cameraDistanceMax_ = distance_max;
    //cameraDistanceIni_ = distance;
    //outDirectionOrientation_ = orientation;
}

void CameraLogic::PostUpdate(float timeStep)
{
    // Do not move if the UI has a focused element (the console)
    if (GetSubsystem<UI>()->GetFocusElement())
        return;

    Input* input = GetSubsystem<Input>();

    // Movement speed as world units per second
    const float MOVE_SPEED = 20.0f;
    // Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY = 0.1f;

    if(cameraType_ == String("default"))
    {
        //this is the default "debug" behavior of the camera
        // Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
        IntVector2 mouseMove = input->GetMouseMove();
        yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
        pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
        pitch_ = Clamp(pitch_, -90.0f, 90.0f);

        // Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
        node_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

        // Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
        if (input->GetKeyDown('W'))
            node_->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
        if (input->GetKeyDown('S'))
            node_->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
        if (input->GetKeyDown('A'))
            node_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
        if (input->GetKeyDown('D'))
            node_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);
    }
    else if(cameraType_ == String("sidescrolling"))
    {
        if(target_)//this will only work if I have a target
        {
            //i need the targets position, orientation, to get my cameras position and orientation
            Vector3 target_position = target_->GetWorldPosition()+(target_->GetWorldRotation() * targetOffset_);
            Quaternion target_orientation = target_->GetWorldRotation();//i may or may not need this at the moment

            Vector3 rotated_origin = outDirectionOrientation_ * outDirection_* cameraDistance_;
            Vector3 target_offset_position = rotated_origin + target_position;

            newPosition_ = target_offset_position;
            Vector3 smoothPos = SmoothPosition(timeStep); 
            
            Vector3 lookDirection = target_position - smoothPos;
            newRotation_.FromLookRotation(lookDirection.Normalized());

            node_->SetPosition(smoothPos);
            node_->SetRotation(newRotation_);
        }
    }
    else if(cameraType_ == String("shmup"))
    {
        if(target_)//this will only work if I have a target
        {
                //i need the targets position, orientation, to get my cameras position and orientation
            Vector3 target_position = target_->GetWorldPosition()+(target_->GetWorldRotation() * targetOffset_);
            unsigned count = 1;
            unsigned s = targets_.Size();
            //lets get the average position of targets, check the targets array
            /*for(unsigned t = 0; t<s; ++t)
            {
                SharedPtr<Node> n = targets_.At(s-t-1);
                if(n != NULL)
                {
                    target_position += n->GetWorldPosition();
                    ++count;
                }
                else
                {
                    targets_.Erase(s-t);//get it off the roster if it doesnt exist
                }
            }*/
            //if (count>1) target_position-=target_->GetWorldRotation() * targetOffset_;
            //target_position/=float(count);
            
            //Quaternion target_orientation = target_->GetWorldRotation();//i may or may not need this at the moment

            Vector3 rotated_origin = outDirectionOrientation_ * outDirection_* cameraDistance_;
            Vector3 target_offset_position = rotated_origin + target_position;

            newPosition_ = target_offset_position;
            Vector3 smoothPos = SmoothPosition(timeStep); 
            
            //Vector3 lookDirection = target_position - smoothPos;
            //newRotation_.FromLookRotation(lookDirection.Normalized());

            node_->SetPosition(smoothPos);
            node_->SetRotation(outDirectionOrientation_ );
        }
    }

    // Toggle debug geometry with space
    //if (input->GetKeyPress(KEY_SPACE))
    //    drawDebug_ = !drawDebug_;
    
    //LOGINFO("updating i guess");
}
void CameraLogic::AddTarget(SharedPtr<Node> target)
{
    targets_.Push(target);
}

Vector3 CameraLogic::SmoothPosition(float timeStep) {
    Vector3 pos = node_->GetWorldPosition();
    Vector3 diff = newPosition_ - pos;
    Vector3 a = diff * timeStep * 10;
    return pos + a;
}

void CameraLogic::SmoothZoom(float timeStep) {
  //curFollow_ = Lerp(curFollow_, follow_, timeStep * 20.0f);
}

void CameraLogic::ShakeCamera(const Vector3 shake)
{
    node_->SetPosition(node_->GetWorldPosition()+shake);
}