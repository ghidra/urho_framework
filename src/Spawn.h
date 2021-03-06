#ifndef SPAWN_H
#define SPAWN_H

#pragma once

#include "Actor.h"

#include "Debug.h"

using namespace Urho3D;

class Spawn : public Actor
{
    URHO3D_OBJECT(Spawn,Actor);
    
public:
    Spawn(Context* context);
    ~Spawn();

    static void RegisterObject(Context* context);

    virtual void FixedUpdate(float timeStep);

    Node* GetNode(){return node_;};

    //void MarkForRemoval();
    //void CheckForRemoval();
    //bool MarkedForRemoval(){return markedForRemoval_;};

    virtual void OnSetAttribute(const AttributeInfo& attr, const Variant& src);
    //float GetDuration() const {return duration_;};
    //void SetDuration(float value){duration_=value;};
    unsigned GetAmount() const {return amount_;};
    void SetAmount(unsigned value){amount_=value;};
    
protected:

    virtual void Setup(const Vector3 pos);

    //float duration_;
    float timeIncrement_;

    float spawning_timer_;
    float spawning_interval_;//the rate of fire
    float spawn_rate_;

    //bool markedForRemoval_;
    
    unsigned amount_;//the amount to spawn

    Debug* debug_=NULL;

};
#endif