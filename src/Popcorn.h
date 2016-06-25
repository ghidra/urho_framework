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

    
    virtual void FixedUpdate(float timeStep);

    Node* GetNode(){return node_;};
    
protected:

    virtual void Setup(Vector3 dir = Vector3::UP , Vector3 g = Vector3(0.0,-9.0,0.0), Vector3 f = Vector3(), Vector3 angular = Vector3(), float ang_var = 0.0f );

    float Fit(float v, float l1, float h1, float l2=0.0f, float h2=1.0f);

    String mesh_;
    String material_;

    float duration_;
    float timeIncrement_;
    
    Vector3 velocity_;
    Vector3 gravity_;
    Vector3 force_;//another other force we want to apply, like a basic wind for example

    Vector3 angular_;//the angluar velocity. The normalized should be the rotation axis, the lenth the speed
    float angular_variance_;//the amount we should vary to angular axis

    Debug* debug_=NULL;
};
#endif