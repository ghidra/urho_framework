#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>

#include "StateMachine.h"

#include <Urho3D/DebugNew.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Engine/DebugHud.h>

StateMachine::StateMachine(Context* context):
    Object(context)
{
    debug_ = new Debug(context_);
}
StateMachine::~StateMachine(){}

//-------------------
//-------------------

void StateMachine::Setup(Controller* controller, Pawn* pawn)
{
    if(controller_!=NULL) controller_ = controller;
    if(pawn!=NULL) pawn_ = pawn;

   //debug_->Hud("STATEMACHINE","STATE MACHINE READY");
}
void StateMachine::SetPawn(Pawn* pawn)
{
    pawn_=pawn;
}

void StateMachine::SetState(State* state)
{
    state_ = state;
    state_->Enter(pawn_);
    //debug_->Hud("STATEMACHINE:",state_->Name());
}
//--------
void StateMachine::Update(float timeStep)
{
    //debug_->Hud("STATEMACHINE:","NOT looping here");
    if(controller_!=NULL)
    {
        //debug_->Hud("STATEMACHINE:","looping here");
        Controls& ctrl = controller_->controls_;
        //--------------------------------------------------
        //--------------------------------------------------
        ///state stuff
        if(state_ != NULL)
        {
            State* state = state_->HandleInput(ctrl);
            if (state != NULL)
            {
                //do exit state before removing state
                state_->Exit();
                delete state_;
                state_ = state;
                //we are entering the new state
                state_->Enter(pawn_);//static_cast<Pawn*>(this)
            }
            state_->Update(timeStep);
            //state_->Debug("State",state_->Name());
        }
    }
}