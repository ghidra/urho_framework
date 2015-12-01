#ifndef PROJECTILE_H
#define PROJECTILE_H

#pragma once

#include "Actor.h"

using namespace Urho3D;

namespace Urho3D
{

class Node;
class Scene;

}

using namespace Urho3D;

class Projectile : public Actor
{

    URHO3D_OBJECT(Projectile,Actor);

public:

    Projectile(Context* context);
    ~Projectile();
    
    static void RegisterObject(Context* context);

    virtual void Start();
    virtual void FixedUpdate(float timeStep);
    virtual void Setup(const Vector3 direction);
    //virtual void Attach(Node* bone);

protected:

    
    virtual void HandleNodeCollision(StringHash eventType, VariantMap& eventData);

    Vector3 pos_born_;//the position this projectile was born
  	Vector3 hit_;//this is if we want this projectile to aim at a specific position
  	float range_;//how far this projectile can travel
  	float damage_;//how much damage this projectile can do

    //bool collected_;
   
};
#endif