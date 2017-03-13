#ifndef SPAWN_H
#define SPAWN_H

#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "Debug.h"

using namespace Urho3D;

class Spawn : public LogicComponent
{
    URHO3D_OBJECT(Spawn,LogicComponent);
    
public:
    Spawn(Context* context);
    ~Spawn();

    static void RegisterObject(Context* context);

    virtual void FixedUpdate(float timeStep);

    Node* GetNode(){return node_;};

    void MarkForRemoval();
    void CheckForRemoval();

    virtual void OnSetAttribute(const AttributeInfo& attr, const Variant& src);
    float GetDuration() const {return duration_;};
    void SetDuration(float value){duration_=value;};
    unsigned GetAmount() const {return amount_;};
    void SetAmount(unsigned value){amount_=value;};
    
protected:

    virtual void Setup(const Vector3 pos);

    float duration_;
    bool markedForRemoval_;
    
    unsigned amount_;//the amount to spawn

    Debug* debug_=NULL;

};
#endif