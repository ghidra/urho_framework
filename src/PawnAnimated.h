#ifndef PAWNANIMATED_H
#define PAWNANIMATED_H

#pragma once

#include "Pawn.h"
#include "RagDoll.h"
#include <Urho3D/Graphics/AnimationController.h>

class RagDoll;

class PawnAnimated : public Pawn
{

    URHO3D_OBJECT(PawnAnimated,Pawn);

public:

    PawnAnimated(Context* context);
    ~PawnAnimated();

    //virtual void FixedUpdate(float timeStep);
    //virtual void Possess(Controller* controller);
    //virtual void EquipWeapon(Weapon* weapon);

    AnimationController* GetAnimationController(){return animationController_;};
    //void SetState(State* state);
    RagDoll* GetRagDoll(){return ragdoll_;};

protected:

    virtual void Setup();

    virtual void HandleNodeCollision(StringHash eventType, VariantMap& eventData);

    AnimationController* animationController_;

    //ragdoll
    WeakPtr<RagDoll> ragdoll_;

    //now I can maybe have an array or something to hold the required animation paths?
   
};
#endif