#ifndef SCENEMACHINE_H
#define SCENEMACHINE_H

#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "Controller.h"
#include "Pawn.h"
#include "StateMachine.h"

using namespace Urho3D;

/*namespace Urho3D
{

class Node;
class Scene;

}*/

class SceneMachine : public LogicComponent
{
    URHO3D_OBJECT(SceneMachine,LogicComponent);
    
public:
    SceneMachine(Context* context);
    ~SceneMachine();

    static void RegisterObject(Context* context);

    virtual void Setup(Controller* controller, Pawn* pawn = NULL);
    virtual void FixedUpdate(float timeStep);
    
protected:

    WeakPtr<StateMachine> state_machine_;
    WeakPtr<Controller> controller_; 
    WeakPtr<Pawn> pawn_;
    
};
#endif