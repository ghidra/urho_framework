#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>

#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Resource/ResourceCache.h>

#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Physics/PhysicsEvents.h>

#include "Actor.h"

#include <Urho3D/DebugNew.h>
#include <Urho3D/IO/Log.h>


Actor::Actor(Context* context) :
    LogicComponent(context),
    duration_(-0.1f),
    health_(100.0f),
    maxHealth_(100.0f),
    onGround_(false),
    collision_layer_(1),
    collision_mask_(60)
{
    // Only the scene update event is needed: unsubscribe from the rest for optimization
    //SetUpdateEventMask(USE_FIXEDUPDATE);
    debug_ = new Debug(context_);
}
Actor::~Actor(){}

void Actor::FixedUpdate(float timeStep)
{
    //LOGINFO("inside actor fixedupdate");
	timeIncrement_+=timeStep;
    // Disappear when duration expired
    if (duration_ >= 0.0f){
      duration_ -= timeStep;
      if (duration_ <= 0.0f)
          node_->Remove();
    }
}
void Actor::Setup()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    //node_->SetPosition(Vector3(0.0f, 1.0f, 0.0f));//objectNode

    // Create the rendering component + animation controller
    //AnimatedModel* object = node_->CreateComponent<AnimatedModel>();
    StaticModel* object = node_->CreateComponent<StaticModel>();
    object->SetModel(cache->GetResource<Model>("Models/"+mesh_));
    //object->SetMaterial(cache->GetResource<Material>("Materials/Jack.xml"));
    object->SetCastShadows(true);

    //animationController_ = node_->CreateComponent<AnimationController>();

    SetRigidBody();
    body_->SetAngularFactor(Vector3::ZERO);
    body_->SetCollisionEventMode(COLLISION_ALWAYS);// Set the rigidbody to signal collision also when in rest, so that we get ground collisions properly

    //we still need to setup the collisionshape in the child class
    
    //here we are setting the ragdoll object waiting to accept some commands to build out
    //ragdoll_ = node_->CreateComponent<RagDoll>();//

}
void Actor::SetCollisionLayers(const unsigned layer, const unsigned mask)
{
    collision_layer_ = layer;
    collision_mask_ = mask;
    if(body_)
    {
        body_->SetCollisionLayer(collision_layer_);
        body_->SetCollisionMask(collision_mask_);
    }
}
void Actor::SetRigidBody(const float mass, const float friction)
{
    body_ = node_->CreateComponent<RigidBody>();
    body_->SetCollisionLayer(collision_layer_);
    body_->SetCollisionMask(collision_mask_);
    body_->SetMass(mass);
    body_->SetFriction(friction);
}
void Actor::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{
    // Check collision contacts and see if character is standing on ground (look for a contact that has near vertical normal)
    using namespace NodeCollision;
    
    /*Node* otherNode = static_cast<Node*>(eventData[P_OTHERNODE].GetPtr());
    RigidBody* otherBody = static_cast<RigidBody*>(eventData[P_OTHERBODY].GetPtr());*/
    MemoryBuffer contacts(eventData[P_CONTACTS].GetBuffer());

    //LOGINFO(String(otherNode->HasComponent("Character")));

    /*Actor* otherObject = otherNode->GetComponent<Actor>();
    //Actor* actor = static_cast<Actor*>(otherNode->GetComponent<Character>());
    if(otherObject != NULL)
    {
        LOGINFO("COLLISION");
    }*/
    
    while (!contacts.IsEof())
    {
        contactPosition_ = contacts.ReadVector3();
        contactNormal_ = contacts.ReadVector3();
        contactDistance_ = contacts.ReadFloat();
        contactImpulse_ = contacts.ReadFloat();
        
        // If contact is below node center and mostly vertical, assume it's a ground contact
        if (contactPosition_.y_ < (node_->GetPosition().y_ + 1.0f))
        {
            float level = Abs(contactNormal_.y_);
            if (level > 0.75)
                onGround_ = true;
        }
    }
}

float Actor::Fit(float v, float l1, float h1, float l2=0.0f, float h2=1.0f)
{
    return Clamp( l2 + (v - l1) * (h2 - l2) / (h1 - l1), l2,h2);
}
