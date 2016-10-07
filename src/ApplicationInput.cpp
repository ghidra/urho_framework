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
//#include <Urho3D/UI/Sprite.h>
//#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Core/Timer.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Resource/XMLFile.h>

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/DebugRenderer.h>

#include "Controller.h"
#include "ApplicationInput.h"
#include "CameraLogic.h"

#include <Urho3D/DebugNew.h>
#include <Urho3D/IO/Log.h>

ApplicationInput::ApplicationInput(Context* context):
    Controller(context),
    touchSensitivity_(2.0f),
    paused_(false),
    quit_(false),
    debugCamera_(false),
    debugDrawPhysics_(false),
    fullscreen_(true)
{
    if (GetPlatform() == "Android" || GetPlatform() == "iOS")
        // On mobile platform, enable touch by adding a screen joystick
        InitTouchInput();
    else if (GetSubsystem<Input>()->GetNumJoysticks() == 0)
        // On desktop platform, do not detect touch when we already got a joystick
        SubscribeToEvent(E_TOUCHBEGIN, URHO3D_HANDLER(ApplicationInput, HandleTouchBegin));

    // Subscribe key down event
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(ApplicationInput, HandleKeyDown));
    // Subscribe scene update event
    //SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(ApplicationInput, HandleSceneUpdate));
    //update is called from the class using this class to ahndle input, main.cpp
    //SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(ApplicationInput, HandleMouseButtonDown));
    //SubscribeToEvent(E_MOUSEBUTTONUP, URHO3D_HANDLER(ApplicationInput, HandleMouseButtonUp));
}

//void ApplicationInput::SetParameters(ApplicationHandler * applicationHandler)
//{
//    applicationHandler_ = applicationHandler;
//}
void ApplicationInput::SetCameraNode(SharedPtr<Node> cameraNode)
{
    cameraNode_ = cameraNode;
    cameraLogic_ = cameraNode_->CreateComponent<CameraLogic>();
    //cameraLogic_->SetCameraType(cameraType);
}
void ApplicationInput::SetCameraTarget(SharedPtr<Node> target)
{
    cameraLogic_->SetCameraTarget(target);
}
void ApplicationInput::SetCameraType(const String& cameraType)
{
    cameraLogic_->SetCameraType(cameraType);
    cameraType_ = cameraType;
}
void ApplicationInput::SetCameraParameters(const float distance, const float distance_min, const float distance_max, const Quaternion orientation)
{
    cameraLogic_->SetCameraParameters(distance,distance_max,distance_min,orientation);
}
void ApplicationInput::SetCameraParameters( VariantMap& parms )
{
    cameraLogic_->SetCameraParameters( parms );
}

/*void ApplicationInput::Possess(Actor* actor)
{
    actor_ = actor;
}*/
void ApplicationInput::SetMouseVisible(const bool b)
{
    mouseVisible_=b;
    Input* input = GetSubsystem<Input>();
    input->SetMouseVisible(true);
}

void ApplicationInput::InitTouchInput()
{
    touchEnabled_ = true;

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Input* input = GetSubsystem<Input>();
    XMLFile* layout = cache->GetResource<XMLFile>("UI/ScreenJoystick_Samples.xml");
    const String& patchString = GetScreenJoystickPatchString();
    if (!patchString.Empty())
    {
        // Patch the screen joystick layout further on demand
        SharedPtr<XMLFile> patchFile(new XMLFile(context_));
        if (patchFile->FromString(patchString))
            layout->Patch(patchFile);
    }
    screenJoystickIndex_ = input->AddScreenJoystick(layout, cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));
    input->SetScreenJoystickVisible(screenJoystickSettingsIndex_, true);
}

void ApplicationInput::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    Controller::HandleUpdate(eventType, eventData);

    using namespace Update;

    Input* input = GetSubsystem<Input>();

    //controls_.Set(CTRL_UP | CTRL_DOWN | CTRL_LEFT | CTRL_RIGHT | CTRL_FIRE |CTRL_JUMP, false);

    //LOGINFO("hi");

    //if(actor_)
    //{
        /*this is from character demo for touch controls, which might be worth using here
        // Update controls using touch utility class
        if (touch_)
            touch_->UpdateTouches(character_->controls_);
        */
    UI* ui = GetSubsystem<UI>();
    if (!ui->GetFocusElement())
    {
        //if (!touch_ || !touch_->useGyroscope_)
        //{
        controls_.Set(CTRL_UP, input->GetKeyDown('W'));
        controls_.Set(CTRL_DOWN, input->GetKeyDown('S'));
        controls_.Set(CTRL_LEFT, input->GetKeyDown('A'));
        controls_.Set(CTRL_RIGHT, input->GetKeyDown('D'));
        //}
        if(input->GetKeyDown(KEY_LCTRL) || input->GetKeyPress(KEY_LCTRL) || input->GetMouseButtonDown(MOUSEB_LEFT) || input->GetMouseButtonPress(MOUSEB_LEFT) )
        //if(input->GetKeyDown(KEY_LCTRL) || input->GetKeyPress(KEY_LCTRL) )
        {
            controls_.Set(CTRL_FIRE, true);
        }
        controls_.Set(CTRL_JUMP, input->GetKeyDown(KEY_SPACE));
        controls_.Set(CTRL_ALL, input->GetKeyDown(KEY_RETURN));
    
    }
    //now if we have possessed something, we can send it commands
    /*if(actor_)
    {
        actor_->Control(&controls_);
    }*/
    //}

    /*if (actor_)
    {
        // Clear previous controls
        character_->controls_.Set(CTRL_FORWARD | CTRL_BACK | CTRL_LEFT | CTRL_RIGHT | CTRL_JUMP, false);

        // Update controls using touch utility class
        if (touch_)
            touch_->UpdateTouches(character_->controls_);

        // Update controls using keys
        UI* ui = GetSubsystem<UI>();
        if (!ui->GetFocusElement())
        {
            if (!touch_ || !touch_->useGyroscope_)
            {
                character_->controls_.Set(CTRL_FORWARD, input->GetKeyDown('W'));
                character_->controls_.Set(CTRL_BACK, input->GetKeyDown('S'));
                character_->controls_.Set(CTRL_LEFT, input->GetKeyDown('A'));
                character_->controls_.Set(CTRL_RIGHT, input->GetKeyDown('D'));
            }
            character_->controls_.Set(CTRL_JUMP, input->GetKeyDown(KEY_SPACE));

            // Add character yaw & pitch from the mouse motion or touch input
            if (touchEnabled_)
            {
                for (unsigned i = 0; i < input->GetNumTouches(); ++i)
                {
                    TouchState* state = input->GetTouch(i);
                    if (!state->touchedElement_)    // Touch on empty space
                    {
                        Camera* camera = cameraNode_->GetComponent<Camera>();
                        if (!camera)
                            return;

                        Graphics* graphics = GetSubsystem<Graphics>();
                        character_->controls_.yaw_ += TOUCH_SENSITIVITY * camera->GetFov() / graphics->GetHeight() * state->delta_.x_;
                        character_->controls_.pitch_ += TOUCH_SENSITIVITY * camera->GetFov() / graphics->GetHeight() * state->delta_.y_;
                    }
                }
            }
            else
            {
                character_->controls_.yaw_ += (float)input->GetMouseMoveX() * YAW_SENSITIVITY;
                character_->controls_.pitch_ += (float)input->GetMouseMoveY() * YAW_SENSITIVITY;
            }
            // Limit pitch
            character_->controls_.pitch_ = Clamp(character_->controls_.pitch_, -80.0f, 80.0f);

            // Switch between 1st and 3rd person
            if (input->GetKeyPress('F'))
                firstPerson_ = !firstPerson_;

            // Turn on/off gyroscope on mobile platform
            if (touch_ && input->GetKeyPress('G'))
                touch_->useGyroscope_ = !touch_->useGyroscope_;

            // Check for loading / saving the scene
            if (input->GetKeyPress(KEY_F5))
            {
                File saveFile(context_, GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Scenes/CharacterDemo.xml", FILE_WRITE);
                scene_->SaveXML(saveFile);
            }
            if (input->GetKeyPress(KEY_F7))
            {
                File loadFile(context_, GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Scenes/CharacterDemo.xml", FILE_READ);
                scene_->LoadXML(loadFile);
                // After loading we have to reacquire the weak pointer to the Character component, as it has been recreated
                // Simply find the character's scene node by name as there's only one of them
                Node* characterNode = scene_->GetChild("Jack", true);
                if (characterNode)
                    character_ = characterNode->GetComponent<Character>();
            }
        }

        // Set rotation already here so that it's updated every rendering frame instead of every physics frame
        character_->GetNode()->SetRotation(Quaternion(character_->controls_.yaw_, Vector3::UP));
    }*/
}

void ApplicationInput::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    using namespace KeyDown;

    int key = eventData[P_KEY].GetInt();

    // Close console (if open) or exit when ESC is pressed
    if (key == KEY_ESCAPE)
    {
        Console* console = GetSubsystem<Console>();
        if (console->IsVisible())
            console->SetVisible(false);
        else
            //engine_->Exit();
            quit_=true;
    }

    // Toggle console with F1
    else if (key == KEY_F1)
        GetSubsystem<Console>()->Toggle();
    
    // Toggle debug HUD with F2
    else if (key == KEY_F2)
        GetSubsystem<DebugHud>()->ToggleAll();

    else if (key == KEY_F3)
        debugDrawPhysics_ = !debugDrawPhysics_;
    else if (key == KEY_F4)
    {
        //turn on the debug camera
        debugCamera_ = !debugCamera_;
        if(debugCamera_)
            cameraLogic_->SetCameraType(String("default"));
        else
            cameraLogic_->SetCameraType(cameraType_);
    }
    else if (key == KEY_F10)
    {
        GetSubsystem<DebugHud>()->Toggle(1);//just the debug renderer
    }
    else if(key == KEY_F12)
    {
        ToggleFullscreen();
    }
    else if (key == '[') { // +FOV
        Camera* cam(cameraNode_->GetComponent<Camera>());
        if (cam)
        {
            cam->SetFov(cam->GetFov() + 2.0f);
            URHO3D_LOGINFO(String("Camera FOV = ") + String(cam->GetFov()));
            URHO3D_LOGINFO(String("Camera nearclip = ") + String(cam->GetNearClip()));
            URHO3D_LOGINFO(String("Camera farclip = ") + String(cam->GetFarClip()));
        }
    }
    else if (key == ']') { // +FOV
        Camera* cam(cameraNode_->GetComponent<Camera>());
        if (cam)
        {
            cam->SetFov(cam->GetFov() - 2.0f);
            URHO3D_LOGINFO(String("Camera FOV = ") + String(cam->GetFov()));
            URHO3D_LOGINFO(String("Camera nearclip = ") + String(cam->GetNearClip()));
            URHO3D_LOGINFO(String("Camera farclip = ") + String(cam->GetFarClip()));
        }
    }
    // Common rendering quality controls, only when UI has no focused element
    else if (!GetSubsystem<UI>()->GetFocusElement())
    {
        Renderer* renderer = GetSubsystem<Renderer>();
        
        // Preferences / Pause
        if (key == KEY_SELECT && touchEnabled_)
        {
            paused_ = !paused_;

            Input* input = GetSubsystem<Input>();
            if (screenJoystickSettingsIndex_ == M_MAX_UNSIGNED)
            {
                // Lazy initialization
                ResourceCache* cache = GetSubsystem<ResourceCache>();
                screenJoystickSettingsIndex_ = input->AddScreenJoystick(cache->GetResource<XMLFile>("UI/ScreenJoystickSettings_Samples.xml"), cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));
            }
            else
                input->SetScreenJoystickVisible(screenJoystickSettingsIndex_, paused_);
        }

        // Texture quality
        else if (key == '1')
        {
            int quality = renderer->GetTextureQuality();
            ++quality;
            if (quality > QUALITY_HIGH)
                quality = QUALITY_LOW;
            renderer->SetTextureQuality(quality);
        }
        
        // Material quality
        else if (key == '2')
        {
            int quality = renderer->GetMaterialQuality();
            ++quality;
            if (quality > QUALITY_HIGH)
                quality = QUALITY_LOW;
            renderer->SetMaterialQuality(quality);
        }
        
        // Specular lighting
        else if (key == '3')
            renderer->SetSpecularLighting(!renderer->GetSpecularLighting());
        
        // Shadow rendering
        else if (key == '4')
            renderer->SetDrawShadows(!renderer->GetDrawShadows());
        
        // Shadow map resolution
        else if (key == '5')
        {
            int shadowMapSize = renderer->GetShadowMapSize();
            shadowMapSize *= 2;
            if (shadowMapSize > 2048)
                shadowMapSize = 512;
            renderer->SetShadowMapSize(shadowMapSize);
        }
        
        // Shadow depth and filtering quality
        else if (key == '6')
        {
            ShadowQuality quality = renderer->GetShadowQuality();
            quality = (ShadowQuality)(quality + 1);
            if (quality > SHADOWQUALITY_BLUR_VSM)
                quality = SHADOWQUALITY_SIMPLE_16BIT;
            renderer->SetShadowQuality(quality);
        }
        
        // Occlusion culling
        else if (key == '7')
        {
            bool occlusion = renderer->GetMaxOccluderTriangles() > 0;
            occlusion = !occlusion;
            renderer->SetMaxOccluderTriangles(occlusion ? 5000 : 0);
        }
        
        // Instancing
        else if (key == '8')
            renderer->SetDynamicInstancing(!renderer->GetDynamicInstancing());
        
        // Take screenshot
        else if (key == '9')
        {
            Graphics* graphics = GetSubsystem<Graphics>();
            Image screenshot(context_);
            graphics->TakeScreenShot(screenshot);
            // Here we save in the Data folder with date and time appended
            // I expect to have a Resources/Screenshots folder to save into
            screenshot.SavePNG(GetSubsystem<FileSystem>()->GetProgramDir() + "Resources/Screenshots/Screenshot_" +
                Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_') + ".png");
        }
    }
}
void ApplicationInput::SetFullscreen(const bool value)
{
    if(value!=fullscreen_) ToggleFullscreen();
    fullscreen_ = value;
}
void ApplicationInput::ToggleFullscreen()
{
    Graphics* graphics = GetSubsystem<Graphics>();
    IntVector2 res = graphics->GetDesktopResolution();
    if(fullscreen_){
        
        //set it to half res i assume
        IntVector2 resh = res-IntVector2(int(res.x_/2.0),int(res.y_/2.0));
        unsigned posx = unsigned(resh.x_/2.0);
        unsigned posy = unsigned(resh.y_/2.0);
        IntVector2 pos = IntVector2(posx,posy);

        graphics->SetMode(unsigned(res.x_/2.0),unsigned(res.y_/2.0),false,true,false,false,false,false,1);
        graphics->SetWindowPosition(pos.x_,pos.y_);

        fullscreen_=false;
    }else{
        graphics->SetMode(res.x_,res.y_,false,true,false,false,false,false,1);
        graphics->SetWindowPosition(0,0);

        fullscreen_=true;
    }
}

void ApplicationInput::HandleTouchBegin(StringHash eventType, VariantMap& eventData)
{
    // On some platforms like Windows the presence of touch input can only be detected dynamically
    InitTouchInput();
    UnsubscribeFromEvent("TouchBegin");
}