#include <Urho3D/Urho3D.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/UI/Cursor.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Core/Timer.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Graphics/Animation.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationState.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Graphics/Zone.h>

#include "ApplicationInput.h"
#include "ApplicationHandler.h"

#include <Urho3D/IO/Log.h>

ApplicationHandler::ApplicationHandler(Context* context) :
    Application(context),
    touchEnabled_(false),
    screenJoystickIndex_(M_MAX_UNSIGNED),
    screenJoystickSettingsIndex_(M_MAX_UNSIGNED),
    paused_(false)
{
    //CameraLogic::RegisterObject(context);
    //context->RegisterFactory<CameraLogic>();
}

void ApplicationHandler::Setup()
{
    // Modify engine startup parameters
    engineParameters_["WindowTitle"] = GetTypeName();
    engineParameters_["LogName"]     = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
    //engineParameters_["FullScreen"]  = false;
    engineParameters_["Headless"]    = false;
    //engineParameters_["WindowResizable"] = true;
    //engineParameters_["WindowWidth"] = 800;
    //engineParameters_["WindowHeight"] = 600;
    engineParameters_["VSync"] = true; // need this on my slow laptop
    engineParameters_["ResourcePaths"] = "Data;CoreData;Resources";//or
    //cache->AddResourceDir("Resources");
}

void ApplicationHandler::Start()
{
    /*if (GetPlatform() == "Android" || GetPlatform() == "iOS")
        // On mobile platform, enable touch by adding a screen joystick
        InitTouchInput();
    else if (GetSubsystem<Input>()->GetNumJoysticks() == 0)
        // On desktop platform, do not detect touch when we already got a joystick
        SubscribeToEvent(E_TOUCHBEGIN, URHO3D_HANDLER(ApplicationHandler, HandleTouchBegin));
    */
    // Create logo
    //CreateLogo();
    //GetSubsystem<Input>()->SetMouseVisible(true);

    // Set custom window Title & Icon
    SetWindowTitleAndIcon();

    // Create console and debug HUD
    CreateConsoleAndDebugHud();

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    //cache->AddResourceDir("/home/jimmy/Documents/projects/urho/urho_vania/bin/Resources");

    //----
    CreateScene();

    // Setup the viewport for displaying the scene
    SetupViewport();

    // Hook up to the frame update and render post-update events
    SubscribeToEvents();
    //-------

    // Subscribe key down event
    //SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(ApplicationHandler, HandleKeyDown));
    // Subscribe scene update event
    //SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(ApplicationHandler, HandleSceneUpdate));
}

void ApplicationHandler::Stop()
{
    engine_->DumpResources(true);
}

void ApplicationHandler::SetApplicationInput(ApplicationInput* applicationInput, const bool fullscreen)
{
    applicationInput_ = applicationInput;
    applicationInput_->SetCameraNode(cameraNode_);
    applicationInput_->SetFullscreen(fullscreen);
    //CameraLogic* cameraLogic_ = cameraNode_->CreateComponent<CameraLogic>();
    //cameraLogic_->SetCameraType(cameraType);
}

void ApplicationHandler::SetWindowTitleAndIcon()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Graphics* graphics = GetSubsystem<Graphics>();
    Image* icon = cache->GetResource<Image>("Textures/UrhoIcon.png");
    graphics->SetWindowIcon(icon);
    graphics->SetWindowTitle("Urho3D Sample");
}

void ApplicationHandler::CreateConsoleAndDebugHud()
{
    // Get default style
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    // Create console
    Console* console = engine_->CreateConsole();
    console->SetDefaultStyle(xmlFile);
    console->GetBackground()->SetOpacity(0.8f);

    // Create debug HUD.
    DebugHud* debugHud = engine_->CreateDebugHud();
    debugHud->SetDefaultStyle(xmlFile);
}

void ApplicationHandler::HandleSceneUpdate(StringHash eventType, VariantMap& eventData)
{
    // Move the camera by touch, if the camera node is initialized by descendant sample class
    if (touchEnabled_ && cameraNode_)
    {
        //LOGINFO("TOUCH ENABLED");
        Input* input = GetSubsystem<Input>();
        for (unsigned i = 0; i < input->GetNumTouches(); ++i)
        {
            TouchState* state = input->GetTouch(i);
            if (!state->touchedElement_)    // Touch on empty space
            {
                if (state->delta_.x_ ||state->delta_.y_)
                {
                    Camera* camera = cameraNode_->GetComponent<Camera>();
                    if (!camera)
                        return;

                    Graphics* graphics = GetSubsystem<Graphics>();
                    //yaw_ += TOUCH_SENSITIVITY * camera->GetFov() / graphics->GetHeight() * state->delta_.x_;
                    //pitch_ += TOUCH_SENSITIVITY * camera->GetFov() / graphics->GetHeight() * state->delta_.y_;

                    // Construct new orientation for the camera scene node from yaw and pitch; roll is fixed to zero
                    //cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));
                }
                else
                {
                    // Move the cursor to the touch position
                    Cursor* cursor = GetSubsystem<UI>()->GetCursor();
                    if (cursor && cursor->IsVisible())
                        cursor->SetPosition(state->position_);
                }
            }
        }
    }/*else{
        LOGINFO("WHAT NOW");
        Cursor* cursor = GetSubsystem<UI>()->GetCursor();
        if (cursor && cursor->IsVisible())
            cursor->SetPosition(state->position_);
    }*/
}

void ApplicationHandler::CreateScene()
{
    scene_ = new Scene(context_);

    // Create octree, use default volume (-1000, -1000, -1000) to (1000, 1000, 1000)
    // Also create a DebugRenderer component so that we can draw debug geometry
    scene_->CreateComponent<Octree>();
    PhysicsWorld* pw = scene_->CreateComponent<PhysicsWorld>();
    scene_->CreateComponent<DebugRenderer>();

    //pw->SetGravity(Vector3(0.0f,-20.0f,0.0f));

    // Create the camera. Limit far clip distance to match the fog
    cameraNode_ = scene_->CreateChild("Camera");
    Camera* camera = cameraNode_->CreateComponent<Camera>();
    camera->SetFarClip(300.0f);

}
void ApplicationHandler::SetupViewport()
{
    Renderer* renderer = GetSubsystem<Renderer>();

    // Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);
}

void ApplicationHandler::SubscribeToEvents()
{
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(ApplicationHandler, HandleUpdate));
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(ApplicationHandler, HandlePostRenderUpdate));
    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(ApplicationHandler, HandleSceneUpdate));
    //SubscribeToEvent(E_FIXEDUPDATE, URHO3D_HANDLER(ApplicationHandler, HandleFixedUpdate));

}
void ApplicationHandler::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    // Take the frame time step, which is stored as a float
    //float timeStep = eventData[P_TIMESTEP].GetFloat();

    //check if we have an input object
    ////LOGDEBUG("something");
    //LOGERROR("something errors");
    //LOGINFO("something info");
    if(applicationInput_)
    {
        if (applicationInput_->GetQuit())
        {
            Console* console = GetSubsystem<Console>();
            if (console->IsVisible())
                console->SetVisible(false);
            else
                engine_->Exit();
        }

        //-----
        applicationInput_->HandleUpdate(eventType, eventData);
    }

    // Move the camera, scale movement with time step
    //MoveCamera(timeStep);
}

void ApplicationHandler::ToggleFullscreen()
{
    if(applicationInput_)
    {
        applicationInput_->ToggleFullscreen();
    } 
}

void ApplicationHandler::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{
    // If draw debug mode is enabled, draw viewport debug geometry, which will show eg. drawable bounding boxes and skeleton
    // bones. Note that debug geometry has to be separately requested each frame. Disable depth test so that we can see the
    // bones properly
    if(applicationInput_)
    {
        if (applicationInput_->IsDebugDrawPhysics())
            scene_->GetComponent<PhysicsWorld>()->DrawDebugGeometry(true);
            //GetSubsystem<Renderer>()->DrawDebugGeometry(false);//this draws lights and boundinbaxes at least
    }
}