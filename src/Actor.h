#ifndef ACTOR_H
#define ACTOR_H

#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>

#include "Debug.h"

using namespace Urho3D;

const int SIDE_NEUTRAL = 0;
const int SIDE_PLAYER = 1;
const int SIDE_ENEMY = 2;

enum DamageType
{
    DT_BALISTIC,
    DT_ELEMENTAL
};

class Actor : public LogicComponent
{
    URHO3D_OBJECT(Actor,LogicComponent);
    
public:
    Actor(Context* context);
    ~Actor();

    virtual void FixedUpdate(float timeStep);
    Node* GetNode(){return node_;};
    RigidBody* GetBody(){return body_;};
    CollisionShape* GetShape(){return shape_;};
    void SetCollisionLayers(const unsigned layer, const unsigned mask);

    bool canCollect_;//can collect pickups

    virtual void TakeDamage(const float amount, const Vector3 pos = Vector3(), const Vector3 dir= Vector3(), const unsigned level = 0, const enum DamageType type = DT_BALISTIC);//called from something that hits this... like a projectile
    
protected:

    virtual void Setup();
    //virtual bool Damage(Actor * origin, float amount);
    //virtual bool Heal(float amount);
    //virtual void PlaySound(const String& soundName);
    virtual void SetRigidBody(const float mass = 1.0f, const float friction = 1.0f);
    virtual void HandleNodeCollision(StringHash eventType, VariantMap& eventData);
    float Fit(float v, float l1, float h1, float l2, float h2);
    //virtual void WorldCollision(VariantMap& eventData);

    String mesh_;
    String mesh_convex_;
    String material_;

    float health_;
    float maxHealth_;
    unsigned level_;
    int lastDamageSide_;
    unsigned lastDamageCreatorID_;

    //bool readyForUpdate_;//this is to make sure that we are totally initialized before calling fixedupdate. for physcs objects
  
    //bool onGround_;
    //bool isSliding_;
    float duration_;
    //float health_;
    //float maxHealth_;
    int side_;
    //int lastDamageSide_;
    //unsigned lastDamageCreatorID_;
    unsigned creatorID_;

    float speed_;
    float sensitivity_;

    float timeIncrement_;

    WeakPtr<RigidBody> body_;
    WeakPtr<CollisionShape> shape_;

    //character collision masks as default
    unsigned collision_layer_;//=1;
    unsigned collision_mask_;//=60;

    //collision data
    Vector3 contactPosition_;
    Vector3 contactNormal_;
    float contactDistance_;
    float contactImpulse_;

    Debug* debug_=NULL;
/*
COLLISION LAYERS AND MASKS
--------------------------------
layers:                        masks:
000001 = (1)character          111100 (60)
000010 = (2)character bullets  111000 (56)
000100 = (4)pickups            100001 (33)
001000 = (8)enemy bullets      100001 (33)
010000 = (16)enemies           100011 (35) 110011 (51)(collide with other enemies)
100000 = (32)env               111111 (63)

000010 = (2)character casings  111001 (57)

32,16,8,4,2,1
---------------------------------
*/

};
#endif