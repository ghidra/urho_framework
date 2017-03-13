#ifndef PICKUP_H
#define PICKUP_H

#pragma once

#include "Actor.h"

using namespace Urho3D;

namespace Urho3D
{

class Node;
class Scene;

}

using namespace Urho3D;

class PickUp : public Actor
{

    URHO3D_OBJECT(PickUp,Actor);

public:

    PickUp(Context* context);
    ~PickUp();

    static void RegisterObject(Context* context);

    virtual void FixedUpdate(float timeStep);

protected:

    virtual void Setup();
    virtual void HandleNodeCollision(StringHash eventType, VariantMap& eventData);

    bool collected_;
    float life_span_;
    float life_;//this will be the normalized life value
   
};
#endif