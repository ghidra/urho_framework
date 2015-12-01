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
    virtual void FixedUpdate(float timeStep);

    //setters
    virtual void SetCameraType(const String& cameraType){ cameraType_ = cameraType; }
    virtual void SetCameraTarget(SharedPtr<Node> target){  target_ = target; }
    virtual void SetCameraParameters(const float distance, const float distance_min = 1.0f, const float distance_max = 20.0f, const Quaternion orientation = Quaternion(0.0f,0.0f,0.0f,1.0f) );
    virtual void SetCameraParameters( VariantMap& parms );


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
    SharedPtr<Node> target_;
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
