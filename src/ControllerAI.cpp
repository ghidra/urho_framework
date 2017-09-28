#include <Urho3D/Urho3D.h>

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Graphics/Renderer.h>

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/UI/Cursor.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/DebugRenderer.h>

#include "ControllerAI.h"
#include "ApplicationInput.h"

ControllerAI::ControllerAI(Context* context):
    Controller(context)
{
}
ControllerAI::~ControllerAI(){}

void ControllerAI::SetCameraNode(SharedPtr<Node> cameraNode)
{
    cameraNode_ = cameraNode;
    cameraLogic_ = cameraNode_->CreateComponent<CameraLogic>();
    //cameraLogic_->SetCameraType(cameraType);
}
void ControllerAI::SetCameraTarget(SharedPtr<Node> target)
{
    cameraLogic_->SetCameraTarget(target);
}


void ControllerAI::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    Controller::HandleUpdate(eventType, eventData);

    /*
        controls_.Set(CTRL_UP, input->GetKeyDown('W'));
        controls_.Set(CTRL_DOWN, input->GetKeyDown('S'));
        controls_.Set(CTRL_LEFT, input->GetKeyDown('A'));
        controls_.Set(CTRL_RIGHT, input->GetKeyDown('D'));
        if(input->GetKeyDown(KEY_LCTRL) || input->GetKeyPress(KEY_LCTRL) || input->GetMouseButtonDown(MOUSEB_LEFT) || input->GetMouseButtonPress(MOUSEB_LEFT) )
        {
            controls_.Set(CTRL_FIRE, true);
        }
        controls_.Set(CTRL_JUMP, input->GetKeyDown(KEY_SPACE));
        controls_.Set(CTRL_ALL, input->GetKeyDown(KEY_RETURN));
        */
    
}


