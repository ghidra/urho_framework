#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Scene.h>

//#include "Pawn.h"
//#include "Controller.h"

#include "SceneMachine.h"

#include <Urho3D/DebugNew.h>
#include <Urho3D/IO/Log.h>


SceneMachine::SceneMachine(Context* context) :
    LogicComponent(context),
    state_machine_(new StateMachine(context))
{
    // Only the scene update event is needed: unsubscribe from the rest for optimization
    //SetUpdateEventMask(USE_FIXEDUPDATE);
}
SceneMachine::~SceneMachine(){}

void SceneMachine::RegisterObject(Context* context)
{
    context->RegisterFactory<SceneMachine>();
}

void SceneMachine::Setup(Controller* controller, Pawn* pawn)
{
    controller_ = controller;
    pawn_ = pawn;
    state_machine_->Setup(controller,pawn);
}

void SceneMachine::FixedUpdate(float timeStep)
{
    
}
