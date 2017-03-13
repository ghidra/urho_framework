#ifndef PROJECTILE_H
#define PROJECTILE_H

#pragma once

#include "Actor.h"
#include "Weapon.h"

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

    virtual void FixedUpdate(float timeStep);
    virtual void SetupBase();//base setup method
    virtual void Setup(const Vector3 direction);
    virtual void Setup(VariantMap& parms);
    //virtual float GetDamage(){return damage_;};
    virtual void Impact(Node* node, const Vector3 pos, const Vector3 dir);
    //virtual void Impact(RigidBody* body, const Vector3 pos, const Vector3 dir);
    //virtual void Attach(Node* bone);
    void SetOwner(SharedPtr<Weapon> owner);
    SharedPtr<Weapon> GetOwner(){return owner_;};

    float GetSpeed() const {return speed_;};
    void SetSpeed(float value){speed_=value;};
    float GetRange() const {return range_;};
    void SetRange(float value){range_=value;};
    float GetDamage() const {return damage_;};
    void SetDamage(float value){damage_=value;};
    

protected:
    
    virtual void HandleNodeCollision(StringHash eventType, VariantMap& eventData);

    Vector3 pos_born_;//the position this projectile was born
    Vector3 pos_last_;//the last position
    Vector3 hit_;//this is if we want this projectile to aim at a specific position
    Vector3 dir_;//the direction that we are travelling, length is speed
    float speed_;//the speed of the bullet
    float range_;//how far this projectile can travel
    float damage_;//how much damage this projectile can do
    float collision_size_;//this is the collision size
    float collision_size_half_;//this is the collision size
    bool ray_test_;//weather we want to do ray testing for better collision results on fast projectiles

    SharedPtr<Weapon> owner_;
   
};
#endif