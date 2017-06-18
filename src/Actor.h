#ifndef ACTOR_H
#define ACTOR_H

#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Graphics/Material.h>

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
    virtual ~Actor();

    static void RegisterObject(Context* context);

    virtual void FixedUpdate(float timeStep);
    //virtual void FixedPostUpdate(float timeStep);

    Node* GetNode(){return node_;};
    RigidBody* GetBody(){return body_;};
    //CollisionShape* GetShape(){return shape_;};
    void SetCollisionLayers(const unsigned layer, const unsigned mask);

    bool canCollect_;//can collect pickups

    virtual void TakeDamage(const float amount, const Vector3 pos = Vector3(), const Vector3 dir= Vector3(), const unsigned level = 0, const enum DamageType type = DT_BALISTIC);//called from something that hits this... like a projectile
    virtual void OnProjectileHitActor(Actor* victim);

    void MarkForRemoval();
    void CheckForRemoval();
    bool MarkedForRemoval(){return markedForRemoval_;};

    //editor functions
    virtual void OnSetAttribute(const AttributeInfo& attr, const Variant& src);
    float GetMaxHealth() const {return maxHealth_;};
    void SetMaxHealth(float value){maxHealth_=value;};
    float GetDuration() const {return duration_;};
    void SetDuration(float value){duration_=value;};
    float GetSpeed() const {return speed_;};
    void SetSpeed(float value){speed_=value;};
    float GetSensitivity()const {return sensitivity_;};
    void SetSensitivity(float value){sensitivity_=value;};
    
protected:

    virtual void Setup();
    virtual void SetSide(const unsigned side);
    //virtual bool Damage(Actor * origin, float amount);
    //virtual bool Heal(float amount);
    //virtual void PlaySound(const String& soundName);
    virtual void SetRigidBody(const float mass = 1.0f, const float friction = 1.0f);
    virtual void CreateCollisionShape();
    virtual void HandleNodeCollision(StringHash eventType, VariantMap& eventData);
    float Fit(float v, float l1, float h1, float l2=0.0f, float h2=1.0f);
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

    RigidBody* body_=NULL;
    CollisionShape* shape_=NULL;

    SharedPtr<Material> assignedMaterial_;
    bool materialIsUnique_;

    //character collision masks as default
    unsigned collision_layer_;//=1;
    unsigned collision_mask_;//=60;

    //collision data
    Vector3 contactPosition_;
    Vector3 contactNormal_;
    float contactDistance_;
    float contactImpulse_;

    SharedPtr<Debug> debug_;

    bool markedForRemoval_;
};
#endif
