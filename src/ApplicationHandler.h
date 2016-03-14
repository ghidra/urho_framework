//
#ifndef APPLICATIONURHO3D_HANDLER_H
#define APPLICATIONURHO3D_HANDLER_H

#pragma once

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Graphics/Texture2D.h>
#include "ApplicationInput.h"

namespace Urho3D
{

class Node;
class Scene;
class Sprite;

}

// All Urho3D classes reside in namespace Urho3D
using namespace Urho3D;

/// Sample class, as framework for all samples.
///    - Initialization of the Urho3D engine (in Application class)
///    - Modify engine parameters for windowed mode and to show the class name as title
///    - Create Urho3D logo at screen
///    - Set custom window title and icon
///    - Create Console and Debug HUD, and use F1 and F2 key to toggle them
///    - Toggle rendering options from the keys 1-8
///    - Take screenshot with key 9
///    - Handle Esc key down to hide Console or exit application
///    - Init touch input on mobile platform using screen joysticks (patched for each individual sample)
class ApplicationHandler : public Application
{
    // Enable type information.
    URHO3D_OBJECT(ApplicationHandler,Application);

public:
    /// Construct.
    ApplicationHandler(Context* context);

    /// Setup before engine initialization. Modifies the engine parameters.
    virtual void Setup();
    /// Setup after engine initialization. Creates the logo, console & debug HUD.
    virtual void Start();
    /// Cleanup after the main loop. Called by Application.
    virtual void Stop();

    SharedPtr<Scene> scene_;
    /// Camera scene node.
    SharedPtr<Node> cameraNode_;

    unsigned GetReflectionViewMask(){return 0x80000000;};// Set a different viewmask on the water plane to be able to hide it from the reflection camera
    SharedPtr<Texture2D> GetReflectionTexture(){return reflectionTexture_;};
protected:
    /// Return XML patch instructions for screen joystick layout for a specific sample app, if any.
    //virtual String GetScreenJoystickPatchString() const { return String::EMPTY; }
    /// Initialize touch input on mobile platform.
    //void InitTouchInput();
    /// Control logo visibility.
    //void SetLogoVisible(bool enable);
    /// Logo sprite.
    //SharedPtr<Sprite> logoSprite_;
    void SetApplicationInput(ApplicationInput* applicationInput, const bool fullscreen=true);
    /// Scene.
    void CreateScene();
    /// Construct an instruction text to the UI.
    void SetupViewport();
    void SetupReflectionViewport(Node* waterPlaneNode,const String mat = "Water");
    /// Subscribe to application-wide logic update and post-render update events.
    void SubscribeToEvents();

    virtual void HandleUpdate(StringHash eventType, VariantMap& eventData);
    //void HandleFixedUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle the post-render update event.
    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);
    
    
    /// Flag to indicate whether touch input has been enabled.
    bool touchEnabled_;

//private:
    /// Create logo.
    //void CreateLogo();
    /// Set custom window Title & Icon
    void SetWindowTitleAndIcon();
    /// Create console and debug HUD.
    void CreateConsoleAndDebugHud();
    /// Handle key down event to process key controls common to all samples.
    //void HandleKeyDown(StringHash eventType, VariantMap& eventData);
    /// Handle scene update event to control camera's pitch and yaw for all samples.
    void HandleSceneUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle touch begin event to initialize touch input on desktop platform.
    void HandleTouchBegin(StringHash eventType, VariantMap& eventData);

    void ToggleFullscreen();//we start out in fullscreen mode

    /// Screen joystick index for navigational controls (mobile platforms only).
    unsigned screenJoystickIndex_;
    /// Screen joystick index for settings (mobile platforms only).
    unsigned screenJoystickSettingsIndex_;
    /// Pause flag.
    bool paused_;
    /// Flag for drawing debug geometry.
    bool drawDebug_;

    Plane waterPlane_;
    Plane waterClipPlane_;
    bool reflectionViewportEnabled_;
    SharedPtr<Node> reflectionCameraNode_;//might not need t be public
    SharedPtr<Texture2D> reflectionTexture_;

    ApplicationInput * applicationInput_ = NULL;
};

//#include "Sample.inl"

#endif