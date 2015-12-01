#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>

#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Physics/PhysicsEvents.h>

#include "Actor.h"

#include <Urho3D/DebugNew.h>
#include <Urho3D/IO/Log.h>


Actor::Actor(Context* context) :
    LogicComponent(context),
    duration_(-0.1f),
    onGround_(false),
    collision_layer_(1),
    collision_mask_(60)
{
    // Only the scene update event is needed: unsubscribe from the rest for optimization
    //SetUpdateEventMask(USE_FIXEDUPDATE);
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
