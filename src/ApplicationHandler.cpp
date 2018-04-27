#include <Urho3D/Urho3D.h>
#include <Urho3D/AngelScript/Script.h>
#include <Urho3D/AngelScript/ScriptFile.h>
#include <Urho3D/Audio/Audio.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource3D.h>
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
#include <Urho3D/Graphics/RenderSurface.h>
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

//needed for the script loading
#include <Urho3D/Core/Main.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/ResourceEvents.h>

#include "ApplicationInput.h"
#include "ApplicationHandler.h"

#include "ConfigFile.h"
#include "Controller.h"

#include "Emitter.h"
#include "Remover.h"

#include "Actor.h"
#include "Pawn.h"
#include "PawnAnimated.h"
#include "CameraLogic.h"
#include "PickUp.h"
#include "Popcorn.h"
#include "Projectile.h"
#include "ProjectileTargeting.h"
#include "RagDoll.h"
#include "Spawn.h"
#include "Weapon.h"

#include "IK.h"
#include "IKTarget.h"

#include <Urho3D/IO/Log.h>
#include <Urho3D/DebugNew.h>//maybe i need this for scrip loading

ApplicationHandler::ApplicationHandler(Context* context) :
    Application(context),
    touchEnabled_(false),
    screenJoystickIndex_(M_MAX_UNSIGNED),
    screenJoystickSettingsIndex_(M_MAX_UNSIGNED),
    paused_(false),
    reflectionViewportEnabled_(false),
    audio_(GetSubsystem<Audio>()),
    cache_(GetSubsystem<ResourceCache>()),
    input_(GetSubsystem<Input>())
{
	//windows only
	OpenConsoleWindow();

    context_->RegisterSubsystem(new Script(context_));
    script_ = GetSubsystem<Script>();
    cfg_ = new ConfigManager(context, String::EMPTY, false, false);
    context->RegisterSubsystem(cfg_);
    ConfigFile::RegisterObject(context);
    Emitter::RegisterObject(context);
    Remover::RegisterObject(context);

    Actor::RegisterObject(context);
    Pawn::RegisterObject(context);
    PawnAnimated::RegisterObject(context);
    CameraLogic::RegisterObject(context);
    IK::RegisterObject(context);
    IKTarget::RegisterObject(context);
    PickUp::RegisterObject(context);
    Popcorn::RegisterObject(context);
    Projectile::RegisterObject(context);
    ProjectileTargeting::RegisterObject(context);
    RagDoll::RegisterObject(context);
    Spawn::RegisterObject(context);
    Weapon::RegisterObject(context); 
}

void ApplicationHandler::Setup()
{
    assert(audio_);
    assert(cache_);
    assert(input_);
    assert(script_);
    

    //load the editor
    //FileSystem* filesystem = GetSubsystem<FileSystem>();
    GetScriptFileName();

    // Show usage if not found
    //if (scriptFileName_.Empty())//here we didnt find a script to run nmow play the game
    //{
    //}



    // Modify engine startup parameters
    engineParameters_["WindowTitle"] = GetTypeName();
    engineParameters_["LogName"]     = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";

    if (!engineParameters_.Contains("LogLevel"))
        engineParameters_["LogLevel"] = LOG_ERROR;
    //engineParameters_["FullScreen"]  = false;
    engineParameters_["Headless"]    = false;
    engineParameters_["WindowResizable"] = true;
    //engineParameters_["WindowWidth"] = 800;
    //engineParameters_["WindowHeight"] = 600;
    //engineParameters_["VSync"] = true; // need this on my slow laptop
    engineParameters_["ResourcePaths"] = "Data;CoreData;Resources";//or
    //cache->AddResourceDir("Resources");


    // Override engine parameters with those from configuration file and program arguments.

    const Vector<String>& args = GetArguments();

    // Determine the configuration filepath (default or given in program arguments) and load config.
    String configFilePath = cfgFileName_;

    for (unsigned i=0; i< args.Size(); i++)
    {
        if (args[i].ToLower() == "-conf")
            configFilePath = args[++i];
    }
    if (cfg_->Load(configFilePath, true))
    {
        URHO3D_LOGRAW(String("Configuration file loaded: ") + configFilePath + "\n"); // raw, Log not yet active
    }
    else
    {
        URHO3D_LOGRAW(String("Cannot load configuration file: ") + configFilePath + "\n"); // raw, Log not yet active
    }

    // If LogLevel is in program arguments, do nothing; otherwise, set it from config.
    bool foundArgLog(false);
    for (unsigned i=0; i< args.Size(); i++)
    {
        if (args[i].ToLower() == "-log")
            foundArgLog = true;
    }
    if (!foundArgLog && cfg_->Has("engine", "LogLevel"))
    {
        const char* logLevelPrefixes[] = { "DEBUG", "INFO", "WARNING", "ERROR", 0 }; // IO/Log.cpp
        String logLevel = cfg_->GetString("engine", "LogLevel");
        logLevel.ToUpper();
        unsigned cfgLogLevelIndex = GetStringListIndex(logLevel.CString(), logLevelPrefixes, M_MAX_UNSIGNED);
        if (cfgLogLevelIndex != M_MAX_UNSIGNED)
        {
            GetSubsystem<Log>()->SetLevel(cfgLogLevelIndex);
            engineParameters_["LogLevel"] = cfgLogLevelIndex;
        }
    }

    if (cfg_->Has("engine", "ResourcePaths"))
    {
        engineParameters_["ResourcePaths"] = cfg_->Get("engine", "ResourcePaths");
    }
    /*if (!engineParameters_.Contains("WindowWidth"))
    {
        engineParameters_["WindowWidth"] = cfg_->GetInt("engine", "WindowWidth", 800);
    }
    if (!engineParameters_.Contains("WindowHeight"))
    {
        engineParameters_["WindowHeight"] = cfg_->GetInt("engine", "WindowHeight", 600);
    }*/
    if (!engineParameters_.Contains("VSync"))
    {
        engineParameters_["VSync"] = cfg_->GetBool("engine", "VSync", false);
    }
    if (!engineParameters_.Contains("FullScreen"))
    {
        engineParameters_["FullScreen"] = cfg_->GetBool("engine", "FullScreen", false);
    }

    if (!engineParameters_.Contains("MultiSample"))
    {
        unsigned multiSample(cfg_->GetUInt("engine", "MultiSample", 1));
        engineParameters_["MultiSample"] = multiSample;
    }

    if (cfg_->Has("engine", "SoundMaster"))
    {
        float soundMasterGain(cfg_->GetFloat("engine", "SoundMaster") / 1.0f);
        VariantMap& vm(GetEventDataMap());
        vm[SetMasterGain::P_NAME] = SOUND_MASTER;
        vm[SetMasterGain::P_GAIN] = soundMasterGain;
        SendEvent(E_SETMASTERGAIN, vm);
    }
    if (cfg_->Has("engine", "SoundMusic"))
    {
        float soundMusicGain(cfg_->GetFloat("engine", "SoundMusic") / 1.0f);
        VariantMap& vm(GetEventDataMap());
        vm[SetMasterGain::P_NAME] = SOUND_MUSIC;
        vm[SetMasterGain::P_GAIN] = soundMusicGain;
        SendEvent(E_SETMASTERGAIN, vm);
    }
    if (cfg_->Has("engine", "SoundBuffer") && cfg_->Has("engine", "SoundMixRate"))
    {
        int soundBuffer(cfg_->GetInt("engine", "SoundBuffer", 100));
        int mixRate(cfg_->GetUInt("engine", "SoundMixRate", 48000));
        if (!audio_->SetMode(soundBuffer, mixRate, true)) {
            ErrorExit("Audio::SetMode() failed (disable Config SoundBuffer or SoundMixRate?); soundBuffer "+String(soundBuffer)+" mixRate "+String(mixRate)); }
    }
}

void ApplicationHandler::Start()
{
    URHO3D_LOGDEBUG("Start(); program dir: " + GetSubsystem<FileSystem>()->GetProgramDir());

    int maxFPS(cfg_->GetInt("engine", "maxFps"));
    if (maxFPS >= 0) {
      engine_->SetMaxFps(maxFPS);
    }

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

    //ResourceCache* cache = GetSubsystem<ResourceCache>();
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

    if (scriptFile_)
    {
        // Execute the optional stop function
        if (scriptFile_->GetFunction("void Stop()"))
            scriptFile_->Execute("void Stop()");
    }

    engine_->DumpResources(true);
}

void ApplicationHandler::SetApplicationInput(ApplicationInput* applicationInput, const bool fullscreen)
{
    applicationInput_ = applicationInput;
    applicationInput_->SetCameraNode(cameraNode_);
///@j    applicationInput_->SetFullscreen(fullscreen);
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
    camera->SetFov(cfg_->GetFloat("camera", "FOV", 45.0f));
    camera->SetNearClip(cfg_->GetFloat("camera", "nearClip", 0.1f));
    camera->SetFarClip(cfg_->GetFloat("camera", "farClip", 300.0f));

}
void ApplicationHandler::SetupViewport()
{
    Renderer* renderer = GetSubsystem<Renderer>();

    // Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);
}

void ApplicationHandler::SetupReflectionViewport(Node* waterPlaneNode, const String mat)
{
    reflectionViewportEnabled_ = true;

    Graphics* graphics = GetSubsystem<Graphics>();
    Renderer* renderer = GetSubsystem<Renderer>();
    ResourceCache* cache = GetSubsystem<ResourceCache>(); 

    // Create a mathematical plane to represent the water in calculations
    waterPlane_ = Plane(waterPlaneNode->GetWorldRotation() * Vector3(0.0f, 1.0f, 0.0f), waterPlaneNode->GetWorldPosition());
    // Create a downward biased plane for reflection view clipping. Biasing is necessary to avoid too aggressive clipping
    waterClipPlane_ = Plane(waterPlaneNode->GetWorldRotation() * Vector3(0.0f, 1.0f, 0.0f), waterPlaneNode->GetWorldPosition() -
        Vector3(0.0f, 0.1f, 0.0f));

    // Create camera for water reflection
    // It will have the same farclip and position as the main viewport camera, but uses a reflection plane to modify
    // its position when rendering
    reflectionCameraNode_ = cameraNode_->CreateChild();
    Camera* reflectionCamera = reflectionCameraNode_->CreateComponent<Camera>();
    reflectionCamera->SetFarClip(300.0);
    reflectionCamera->SetViewMask(0x7fffffff); // Hide objects with only bit 31 in the viewmask (the water plane)
    reflectionCamera->SetAutoAspectRatio(false);
    reflectionCamera->SetUseReflection(true);
    reflectionCamera->SetReflectionPlane(waterPlane_);
    reflectionCamera->SetUseClipping(true); // Enable clipping of geometry behind water plane
    reflectionCamera->SetClipPlane(waterClipPlane_);
    // The water reflection texture is rectangular. Set reflection camera aspect ratio to match
    reflectionCamera->SetAspectRatio((float)graphics->GetWidth() / (float)graphics->GetHeight());
    // View override flags could be used to optimize reflection rendering. For example disable shadows
    //reflectionCamera->SetViewOverrideFlags(VO_DISABLE_SHADOWS);

    // Create a texture and setup viewport for water reflection. Assign the reflection texture to the diffuse
    // texture unit of the water material
    int texSize = 1024;
    //SharedPtr<Texture2D> renderTexture_(new Texture2D(context_));
    SharedPtr<Texture2D> reflectionTexture_(new Texture2D(context_));
    //reflectionTexture_ = new Texture2D(context_);
    reflectionTexture_->SetSize(texSize, texSize, Graphics::GetRGBFormat(), TEXTURE_RENDERTARGET);
    reflectionTexture_->SetFilterMode(FILTER_BILINEAR);
    RenderSurface* surface = reflectionTexture_->GetRenderSurface();
    SharedPtr<Viewport> rttViewport(new Viewport(context_, scene_, reflectionCamera));
    surface->SetViewport(0, rttViewport);
    Material* waterMat = cache->GetResource<Material>("Materials/"+mat+".xml");
    //Material* waterMat = cache->GetResource<Material>("Materials/WaterSurface.xml");
    waterMat->SetTexture(TU_DIFFUSE, reflectionTexture_);
}

void ApplicationHandler::SubscribeToEvents()
{
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(ApplicationHandler, HandleUpdate));
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(ApplicationHandler, HandlePostRenderUpdate));
    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(ApplicationHandler, HandleSceneUpdate));
    //SubscribeToEvent(E_FIXEDUPDATE, URHO3D_HANDLER(ApplicationHandler, HandleFixedUpdate));
    SubscribeToEvent(E_SPAWNSOUND, URHO3D_HANDLER(ApplicationHandler, HandleSpawnSound));
    SubscribeToEvent(E_SPAWNSOUND3D, URHO3D_HANDLER(ApplicationHandler, HandleSpawnSound3D));
    SubscribeToEvent(E_SETMASTERGAIN, URHO3D_HANDLER(ApplicationHandler, HandleSetMasterGain));
    SubscribeToEvent(E_GETMASTERGAIN, URHO3D_HANDLER(ApplicationHandler, HandleGetMasterGain));
    SubscribeToEvent(E_STARTMUSIC, URHO3D_HANDLER(ApplicationHandler, HandleStartMusic));
    SubscribeToEvent(E_STOPMUSIC, URHO3D_HANDLER(ApplicationHandler, HandleStopMusic));

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
        //applicationInput_->HandleUpdate();//NOW I HAVE TO CALL THIS ELSEWHERE
    }

    // Move the camera, scale movement with time step
    //MoveCamera(timeStep);
}

void ApplicationHandler::ToggleFullscreen()
{
    if(applicationInput_)
    {
        applicationInput_->ToggleFullscreen();
        if(reflectionViewportEnabled_)
        {
            Graphics* graphics = GetSubsystem<Graphics>();
            Camera* reflectionCamera = reflectionCameraNode_->GetComponent<Camera>();
            reflectionCamera->SetAspectRatio((float)graphics->GetWidth() / (float)graphics->GetHeight()); 
        }
    } 
}

void ApplicationHandler::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{
    // If draw debug mode is enabled, draw viewport debug geometry, which will show eg. drawable bounding boxes and skeleton
    // bones. Note that debug geometry has to be separately requested each frame. Disable depth test so that we can see the
    // bones properly
    if(applicationInput_)
    {
        if (applicationInput_->IsDebugDrawGeometry())
            GetSubsystem<Renderer>()->DrawDebugGeometry(true);
        if (applicationInput_->IsDebugDrawPhysics())
            scene_->GetComponent<PhysicsWorld>()->DrawDebugGeometry(true);
    }
}

void ApplicationHandler::HandleSetMasterGain(StringHash eventType, VariantMap& eventData)
{
    const String& soundType(eventData[SetMasterGain::P_NAME].GetString());
    const float gain(eventData[SetMasterGain::P_GAIN].GetFloat());
    audio_->SetMasterGain(soundType, gain);
}

void ApplicationHandler::HandleGetMasterGain(StringHash eventType, VariantMap& eventData)
{
    const String& typeName(eventData[GetMasterGain::P_NAME].GetString());
    const float gain(audio_->GetMasterGain(typeName));
    eventData[GetMasterGain::P_GAIN] = gain;
}


void ApplicationHandler::HandleSpawnSound3D(StringHash eventType, VariantMap& eventData)
{
    const String& soundResourceName(eventData[SpawnSound3D::P_NAME].GetString());
    Sound* sound(cache_->GetResource<Sound>(soundResourceName));
    if (!sound)
    {
        URHO3D_LOGERROR("Sound resource not found: " + soundResourceName);
        return;
    }
    float gain(eventData[SpawnSound3D::P_GAIN].GetFloat());
    if (gain == 0.0f)
    {
        gain = 1.0f;
    }

    Node* soundNode(scene_->CreateChild(soundResourceName));
    soundNode->SetPosition(eventData[SpawnSound3D::P_POSITION].GetVector3());
    SoundSource3D* soundSource(soundNode->CreateComponent<SoundSource3D>());
    soundSource->SetSoundType(SOUND_MASTER);
    soundSource->SetAutoRemoveMode(REMOVE_NODE);
    soundSource->SetDistanceAttenuation(1.0f, 10.0f, 1.0f); ///@TODO parameterize?
    soundSource->SetGain(gain);
    soundSource->Play(sound);
}

void ApplicationHandler::HandleSpawnSound(StringHash eventType, VariantMap& eventData)
{
    const String& soundResourceName(eventData[SpawnSound::P_NAME].GetString());
    Sound* sound(cache_->GetResource<Sound>(soundResourceName));
    if (!sound)
    {
        URHO3D_LOGERROR("Sound resource not found: " + soundResourceName);
        return;
    }
    float gain(eventData[SpawnSound::P_GAIN].GetFloat());
    if (gain == 0.0f)
    {
        gain = 1.0f;
    }
    Node* soundNode(scene_->CreateChild(soundResourceName));
    SoundSource* soundSource(soundNode->CreateComponent<SoundSource>());
    soundSource->SetAutoRemoveMode(REMOVE_NODE);
    soundSource->SetSoundType(SOUND_MASTER);
    soundSource->SetGain(gain);
    soundSource->Play(sound);
}

/// Music

void ApplicationHandler::HandleStartMusic(StringHash eventType, VariantMap& eventData)
{
    HandleStopMusic(eventType, eventData);
    String soundName(eventData[StartMusic::P_NAME].GetString());
    bool looped(eventData[StartMusic::P_LOOPED].GetBool());

    Sound* sound(cache_->GetResource<Sound>(soundName));
    if (!sound)
    {
        URHO3D_LOGERROR("Music track not found: " + soundName);
        return;
    }
    sound->SetLooped(looped);

    // Create a scene node and a sound source for the music.
    Node* soundNode(scene_->CreateChild("Music"));
    SoundSource* soundSource(soundNode->CreateComponent<SoundSource>());
    // Set its automatic removal and channel, and play it.
    soundSource->SetAutoRemoveMode(REMOVE_NODE);
    soundSource->SetSoundType(SOUND_MUSIC);
    soundSource->Play(sound);
}

void ApplicationHandler::HandleStopMusic(StringHash eventType, VariantMap& eventData)
{
    // Remove the music player node from the scene.
    Node* musicNodeOld(scene_->GetChild("Music"));
    if (musicNodeOld)
    {
        musicNodeOld->Remove();
    }
}

void ApplicationHandler::LogNodeInfo(Node* node, int logType, bool recursive /* = true */, unsigned depth /* = 0 */)
{
  String info;
  GetNodeInfo(info, node);
  Log::Write(logType, info);

  if (recursive)
  {
      if (!node->GetChildren().Empty())
      {
          const Vector<SharedPtr<Node>>& children(node->GetChildren());
          for (Vector<SharedPtr<Node>>::ConstIterator i(children.Begin()); i != children.End(); ++i)
          {
              LogNodeInfo(*i, logType, true, depth + 1);
          }
      }
  }
}

void ApplicationHandler::GetNodeInfo(String& lhs, Node* node, unsigned depth /* = 0 */)
{
    lhs = String('+', depth); // Using the 'fill' ctor.

    // Name
    lhs += "<" + node->GetName() + ">";
    // Pointer in hex
    lhs += " " + ToStringHex((unsigned)(std::size_t)node) + " ";

    // List of components
    const Vector<SharedPtr<Component>>& comps(node->GetComponents());
    for (Vector<SharedPtr<Component>>::ConstIterator i(comps.Begin()); i != comps.End(); ++i)
    {
        if (i != comps.Begin())
        {
          lhs += "/";
        }
        lhs += (*i)->GetTypeName();
    }

    lhs += " pos " + String(node->GetPosition());
    lhs += " posw " + String(node->GetWorldPosition());
}

////loading scripts i need this from urho3dplayer.cpp
void ApplicationHandler::GetScriptFileName()
{
    const Vector<String>& arguments = GetArguments();
    if (arguments.Size() && arguments[0][0] != '-')
    {
        scriptFileName_ = GetInternalPath(arguments[0]);
        if (scriptFileName_ == "editor")
        {
            scriptFileName_ = "/Scripts/Editor.as";
        }
        /*else
        {
            scriptFileName_ = "";
        }*/
    }
}

bool ApplicationHandler::LoadScriptFile()
{
    if(scriptFileName_!="")
    {
        scriptFile_ = GetSubsystem<ResourceCache>()->GetResource<ScriptFile>(scriptFileName_);

        // If script loading is successful, the load the editor
        if (scriptFile_ && scriptFile_->Execute("void Start()"))
        {
            URHO3D_LOGRAW(String("EDITOR CALLED: ") + scriptFileName_ + "\n"); // raw, Log not yet active
            return true;
        }else{
            URHO3D_LOGRAW(String("EDITOR NOT CALLED: ") + scriptFileName_ + "\n"); // raw, Log not yet active
            return false;
        }
    }
    else
    {
        return false;
    }
}
