#ifndef CONTROLLERAI_H
#define CONTROLLERAI_H

#pragma once

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Core/Object.h>

#include "Controller.h"
#include "CameraLogic.h"

class ControllerAI : public Controller
{
    URHO3D_OBJECT(ControllerAI,Controller);

public:
    /// Construct.
    ControllerAI(Context* context);
    ~ControllerAI();

    void SetCameraNode(SharedPtr<Node> cameraNode);
    Node* GetCameraNode(){return cameraNode_;};
    //these all just get passes along
    void SetCameraTarget(SharedPtr<Node> target);//whom the camera should target

    void HandleUpdate(StringHash eventType, VariantMap& eventData);//this is called from outside this class, in the appication handler
protected:
    SharedPtr<Node> cameraNode_;
    CameraLogic* cameraLogic_;
};
#endif