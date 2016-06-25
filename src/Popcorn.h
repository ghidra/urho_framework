#ifndef POPCORN_H
#define POPCORN_H

#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "Debug.h"

using namespace Urho3D;

class Popcorn : public LogicComponent
{
    URHO3D_OBJECT(Popcorn,LogicComponent);
    
public:
    Popcorn(Context* context);
    ~Popcorn();

    virtual void Setup();
    virtual void FixedUpdate(float timeStep);

    Node* GetNode(){return node_;};
    
protected:

    float Fit(float v, float l1, float h1, float l2=0.0f, float h2=1.0f);

    String mesh_;
    String material_;

    float duration_;
    float timeIncrement_;
    
    Vector3 velocity_;
    Vector3 gravity_;
    Vector3 force_;//another other force we want to apply, like a basic wind for example

    Debug* debug_=NULL;
};
#endif