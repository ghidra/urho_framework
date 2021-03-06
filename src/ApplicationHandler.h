//
#ifndef APPLICATIONURHO3D_HANDLER_H
#define APPLICATIONURHO3D_HANDLER_H

#pragma once

#include <Urho3D/AngelScript/Script.h>
#include <Urho3D/Container/Ptr.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/IO/Log.h>

#include "ApplicationInput.h"
#include "ConfigManager.h"

namespace Urho3D
{
class Input;
class Node;
class Scene;
class Script;
class Sprite;
class Audio;
class ResourceCache;
class Variant;
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

    String cfgFileName_;
    SharedPtr<ConfigManager> cfg_;
    SharedPtr<Scene> scene_;
    VariantMap renderTargets_; // Map of RenderTarget settings (bool enabled).
    /// Camera scene node.
    SharedPtr<Node> cameraNode_;
    SharedPtr<Script> script_;
    ResourceCache* cache_;
    Audio* audio_;
    Input* input_;

    unsigned GetReflectionViewMask(){return 0x80000000;};// Set a different viewmask on the water plane to be able to hide it from the reflection camera
    //SharedPtr<Texture2D> GetReflectionTexture(){return reflectionTexture_;};
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

    /// Create logo.
    //void CreateLogo();
    /// Set custom window Title & Icon
    void SetWindowTitleAndIcon();
    /// Create console and debug HUD.
    void CreateConsoleAndDebugHud();
    bool LoadScriptFile();
    void GetScriptFileName();
    void LogNodeInfo(Urho3D::Node* node, int logType = Urho3D::LOG_DEBUG, bool recursive = true, unsigned depth = 0);
    void GetNodeInfo(Urho3D::String& lhs, Urho3D::Node* node, unsigned depth = 0);

    /// Handle key down event to process key controls common to all samples.
    //void HandleKeyDown(StringHash eventType, VariantMap& eventData);
    /// Handle scene update event to control camera's pitch and yaw for all samples.
    void HandleSceneUpdate(StringHash eventType, VariantMap& eventData);
    virtual void HandleUpdate(StringHash eventType, VariantMap& eventData);
    //void HandleFixedUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle the post-render update event.
    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle touch begin event to initialize touch input on desktop platform.
    void HandleTouchBegin(StringHash eventType, VariantMap& eventData);

    void ToggleFullscreen();//we start out in fullscreen mode

    void HandleSetMasterGain(StringHash eventType, VariantMap& eventData);
    void HandleGetMasterGain(StringHash eventType, VariantMap& eventData);
    void HandleSpawnSound3D(StringHash eventType, VariantMap& eventData);
    void HandleSpawnSound(StringHash eventType, VariantMap& eventData);
    void HandleStartMusic(StringHash eventType, VariantMap& eventData);
    void HandleStopMusic(StringHash eventType, VariantMap& eventData);

    /// Screen joystick index for navigational controls (mobile platforms only).
    unsigned screenJoystickIndex_;
    /// Screen joystick index for settings (mobile platforms only).
    unsigned screenJoystickSettingsIndex_;
    /// Pause flag.
    bool paused_;
    /// Flag for drawing debug geometry.
    bool drawDebug_;
    /// Flag to indicate whether touch input has been enabled.
    bool touchEnabled_;

    Plane waterPlane_;
    Plane waterClipPlane_;
    bool reflectionViewportEnabled_;
    SharedPtr<Node> reflectionCameraNode_;//might not need t be public
    //SharedPtr<Texture2D> reflectionTexture_;

    SharedPtr<ApplicationInput> applicationInput_;

    SharedPtr<ScriptFile> scriptFile_;/// Script file.
    String scriptFileName_;/// Script file name.
};


URHO3D_EVENT(E_SPAWNSOUND3D, SpawnSound3D) {
  URHO3D_PARAM(P_NAME, name);
  URHO3D_PARAM(P_GAIN, gain); // float
  URHO3D_PARAM(P_POSITION, position); }

URHO3D_EVENT(E_SPAWNSOUND, SpawnSound) {
  URHO3D_PARAM(P_NAME, name);
  URHO3D_PARAM(P_GAIN, gain); // float
}

URHO3D_EVENT(E_STARTMUSIC, StartMusic) {
  URHO3D_PARAM(P_NAME, name);
  URHO3D_PARAM(P_LOOPED, looped); // bool
}

URHO3D_EVENT(E_STOPMUSIC, StopMusic) { }

URHO3D_EVENT(E_SETMASTERGAIN, SetMasterGain) {
  URHO3D_PARAM(P_NAME, name);
  URHO3D_PARAM(P_GAIN, gain); // float
}

URHO3D_EVENT(E_GETMASTERGAIN, GetMasterGain) {
  URHO3D_PARAM(P_NAME, name);
  URHO3D_PARAM(P_GAIN, gain); // float
}

//#include "Sample.inl"

#endif
