#ifndef APPLICATIONINPUT_H
#define APPLICATIONINPUT_H

#pragma once

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Core/Object.h>

#include "Controller.h"
#include "CameraLogic.h"

class ApplicationInput : public Controller
{
    URHO3D_OBJECT(ApplicationInput,Controller);

public:
    /// Construct.
    ApplicationInput(Context* context);

    bool GetQuit(){return quit_;};

    void SetCameraNode(SharedPtr<Node> cameraNode);
    Node* GetCameraNode(){return cameraNode_;};
    //these all just get passes along
    void SetCameraTarget(SharedPtr<Node> target);//whom the camera should target
    void SetCameraType(const String& cameraType );
    void SetCameraParameters(const float distance, const float distance_min = 1.0f, const float distance_max = 20.0f, const Quaternion orientation = Quaternion(30.0f,0.0f,0.0f) );
    void SetCameraParameters( VariantMap& parms );

    //void Possess(Actor* actor);

    void HandleUpdate(StringHash eventType, VariantMap& eventData);//this is called from outside this class, in the appication handler
    //void HandleFixedUpdate(StringHash eventType, VariantMap& eventData);//this is called from outside this class

    void SetFullscreen(const bool value);
    void ToggleFullscreen();
    //Controls controls_;
    void SetMouseVisible(const bool b = true);

    bool IsDebugCamera(){return debugCamera_;}
    bool IsDebugDrawGeometry(){return debugDrawGeometry_;}
    bool IsDebugDrawPhysics(){return debugDrawPhysics_;}
private:
    /// Construct a new Text instance, containing the 'Hello World' String, and add it to the UI root element.
    //void CreateText();
    //void SetParameters(SharedPtr<Engine>& engine, ApplicationHandler* applicationHandler);
    /// Subscribe to application-wide logic update events.
    void InitTouchInput();
    /// Handle the logic update event.
    void HandleKeyDown(StringHash eventType, VariantMap& eventData);

    //void HandleMouseButtonDown(StringHash eventType, VariantMap& eventData);
    //void HandleMouseButtonUp(StringHash eventType, VariantMap& eventData);

    //
    void HandleTouchBegin(StringHash eventType, VariantMap& eventData);
    

    virtual String GetScreenJoystickPatchString() const { return String::EMPTY; }

    /// Movement controls. Assigned by the main program each frame.
    

    //SharedPtr<Actor> actor_;

    bool touchEnabled_;
    /// Screen joystick index for navigational controls (mobile platforms only).
    unsigned screenJoystickIndex_;
    /// Screen joystick index for settings (mobile platforms only).
    unsigned screenJoystickSettingsIndex_;

    bool paused_;
    bool quit_;


    //mouse stuff
    bool mouseVisible_;
    //debug bools
    bool debugCamera_;
    bool debugDrawGeometry_;
    bool debugDrawPhysics_;
    bool fullscreen_;

    SharedPtr<Node> cameraNode_;
    CameraLogic* cameraLogic_;//i might not need to store this in the end
    String cameraType_;//store this hear because when i use the debug camera to go into debug view, i need to be able to switch it back, so this stores it

    /// Touch sensitivity.
    float touchSensitivity_;
    /// Current camera zoom distance.
    
    /// Zoom flag.
    //bool zoom_;
    /// Gyroscope on/off flag.
    bool useGyroscope_;


};
#endif