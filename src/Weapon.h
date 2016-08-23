#ifndef WEAPON_H
#define WEAPON_H

#pragma once

#include <Urho3D/Input/Controls.h>
#include "Actor.h"

using namespace Urho3D;

namespace Urho3D
{

class Node;
class Scene;

}

class Weapon : public Actor
{

    URHO3D_OBJECT(Weapon,Actor);

public:

    Weapon(Context* context);
    ~Weapon();

    //virtual void FixedUpdate(float timeStep);
    virtual void Update(Controls& ctrl, float timeStep);
    virtual void Setup();
    //virtual void Attach(Node* bone);
    virtual void Fire(float timestep);
    virtual void ReleaseFire();

    virtual void SetProjectileRate(const unsigned rate);//number of projectiles at once
    virtual void SetFireRate(const float fireRate);
    virtual void SetProjectileSpeed(const float speed);
    virtual void SetProjectileRange(const float range);
    virtual void SetProjectileRecoil(const float recoil);
    virtual void SetProjectileContinuous(const bool continuous);
    virtual void SetProjectileCollision(const unsigned layer, const unsigned mask=56);
    bool IsFiring(){return firing_;};
    bool IsReloading(){return reloading_;};
    bool IsContinuous(){return continuous_;};

    virtual void SetOwner(SharedPtr<Actor> owner);
    SharedPtr<Actor> GetOwner(){return owner_;};
    virtual void OnProjectileHitActor(Actor* victim);

    virtual void StartReload();
    virtual void Reload(const float timeStep);
    //virtual unsigned GetFiring(){return firing_;};
    //virtual Vector3 GetFireOffset(return fire_offset_);
    //virtual Vector3 GetLeftHandTarget();
    virtual Vector3 GetLeftHandTarget(){return lefthand_target_;};
    virtual void SetMagSize(const unsigned size,  const float rate);
    //virtual void UpdateMagRemains(const unsigned amount);//this is called incase we do something else for reloading
    virtual unsigned GetMagSize(){return mag_size_;};
    virtual unsigned GetMagRemain(){return mag_remains_;};
    virtual float GetFireRate(){return firing_interval_;};

protected:

    
    //virtual void HandleNodeCollision(StringHash eventType, VariantMap& eventData);
    //virtual void HandleSceneDrawableUpdateFinished(StringHash eventType, VariantMap& eventData);

    
    //virtual void FireLogic(float timeStep);
    virtual void Recoil();
    virtual void SpawnProjectile();
    Node* ProjectileFactory(const Vector3 offset = Vector3::ZERO);

    Vector3 projectile_spawn_position_;
    Vector3 projectile_spawn_direction_;

    //virtual void SetLeftHandOffset();//set the offest. sets itself on fire. needs to always be set

    //bool collected_;
    bool firing_;
    float fire_velocity_;
    float firing_timer_;
    float firing_interval_;//the rate of fire
    unsigned num_projectiles_;
    float projectile_speed_;
    float projectile_range_;
    float projectile_recoil_;
    unsigned projectile_collision_layer_;
    unsigned projectile_collision_mask_;
    bool continuous_;//for continus type projectiles, ie laser
    //bool continuous_spawned_;//weather a continuous projectile has been spawned

    unsigned mag_size_;
    unsigned mag_remains_;
    float reload_timer_;
    bool reloading_;
    float reload_interval_;//speed or reload

    Vector3 fire_off_;//the vector offset of the position releative to where projectiles should spawn
    Quaternion kick_rot_;
    Vector3 kick_off_;

    Vector3 lefthand_off_;//the constant offset of the IK position

    SharedPtr<Actor> owner_;

private:
    
    SharedPtr<Node> lefthand_grip_;//node that is the grip of the weapons for the left had
    Vector3 lefthand_target_;
};
#endif