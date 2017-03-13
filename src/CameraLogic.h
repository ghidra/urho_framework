#ifndef CAMERALOGIC_H
#define CAMERALOGIC_H

#pragma once

#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;

/// Custom logic component for moving the animated model and rotating at area edges.
class CameraLogic : public LogicComponent
{
    URHO3D_OBJECT(CameraLogic,LogicComponent);
    
public:
    /// Construct.
    CameraLogic(Context* context);

    static void RegisterObject(Context* context);
    virtual void PostUpdate(float timeStep);

    //setters
    virtual void SetCameraType(const String& cameraType){ cameraType_ = cameraType; }
    virtual void SetCameraTarget(SharedPtr<Node> target){  target_ = target; }
    virtual void SetCameraParameters(const float distance, const float distance_min = 1.0f, const float distance_max = 20.0f, const Quaternion orientation = Quaternion(0.0f,0.0f,0.0f,1.0f) );
    virtual void SetCameraParameters( VariantMap& parms );

    //getters
    //SharedPtr<Node> GetCameraTarget()const{return target_;};
    virtual void OnSetAttribute(const AttributeInfo& attr, const Variant& src);
    Vector3 GetTargetOffset() const {return targetOffset_;};
    void SetTargetOffset(Vector3 value){targetOffset_=value;};
    float GetCameraDistance() const {return cameraDistance_;};
    void SetCameraDistance(float value){cameraDistance_=value;};
    float GetCameraDistanceMin() const {return cameraDistanceMin_;};
    void SetCameraDistanceMin(float value){cameraDistanceMin_=value;};
    float GetCameraDistanceMax()const {return cameraDistanceMax_;};
    void SetCameraDistanceMax(float value){cameraDistanceMax_=value;};

    virtual void ShakeCamera(const Vector3 shake);

    virtual void AddTarget(SharedPtr<Node> target);


private:

    Vector3 SmoothPosition(float timeStep);
    void SmoothZoom(float timeStep);
    //Quaternion SmoothRotation(float timeStep);

    //SharedPtr<Node> cameraNode_;
    /// Camera yaw angle.
    float yaw_;
    /// Camera pitch angle.
    float pitch_;

    String cameraType_;
    SharedPtr<Node> target_;//our main focus target
    Vector< SharedPtr<Node> > targets_;//if we want to focus on more tha one
    Vector3 targetOffset_;//a positional offset to aim at from the target node

    Vector3 newPosition_;//the target position, for smoothing
    Quaternion newRotation_;//the target rotation, incase I want to transition smoothly

    float cameraDistance_;
    float cameraDistanceMin_;
    float cameraDistanceMax_;
    float cameraDistanceIni_;

    Vector3 outDirection_;
    Quaternion outDirectionOrientation_;
};

#endif
