#ifndef STATE_H
#define STATE_H

#pragma once

#include <Urho3D/Core/Object.h>
#include <Urho3D/Input/Controls.h>
#include "Controller.h"//for the control constants

#include "Pawn.h"

class Pawn;

class State : public Object
{
    URHO3D_OBJECT(State,Object);
    
public:
    State(Context* context);
    ~State();

    virtual State* HandleInput(Controls& ctrl);
    virtual void Enter(Pawn* pawn);
    virtual void Update();
    virtual void Exit();

    virtual void Debug(const String label, const String value);

protected:

    String name_;
    Pawn* pawn_=NULL;

    float Fit(float v, float l1, float h1, float l2, float h2);
  
};
#endif