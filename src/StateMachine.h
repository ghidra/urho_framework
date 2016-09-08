#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#pragma once

#include <Urho3D/Core/Object.h>
#include "Controller.h"
#include "Pawn.h"
#include "Debug.h"

#include "State.h"

class Pawn;
class State;

class StateMachine : public Object
{

    URHO3D_OBJECT(StateMachine,Object);

public:

    StateMachine(Context* context);
    ~StateMachine();

    virtual void Setup(Controller* controller, Pawn* pawn = NULL);
    virtual void Update(float timeStep);
    virtual void SetState(State* state);
    virtual String StateName();

    void DebugStateName();

protected:
 
    WeakPtr<State> state_;  
    WeakPtr<Controller> controller_; 
    WeakPtr<Pawn> pawn_;

    Debug* debug_=NULL;
};
#endif