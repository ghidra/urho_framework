#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Scene.h>

#include <Urho3D/Physics/PhysicsEvents.h>//this is for the namespace node collision
#include <Urho3D/Physics/RigidBody.h>

#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>

#include <Urho3D/Resource/ResourceCache.h>
//#include <Urho3D/Graphics/Material.h>

#include "PickUp.h"
#include "Pawn.h"



PickUp::PickUp(Context* context) :
    Actor(context),
    collected_(false)
{
    SetUpdateEventMask(USE_FIXEDUPDATE);
    collision_layer_ = 4;
    collision_mask_ = 33;
}
PickUp::~PickUp(){}

void PickUp::FixedUpdate(float timeStep)
{
    Actor::FixedUpdate(timeStep);
}
void PickUp::Setup()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();

    StaticModel* object = node_->CreateComponent<StaticModel>();
    object->SetModel(cache->GetResource<Model>("Models/"+mesh_));
    //object->SetMaterial(cache->GetResource<Material>("Materials/Jack.xml"));
    object->SetCastShadows(true);


    RigidBody* body = node_->CreateComponent<RigidBody>();
    body->SetCollisionLayer(collision_layer_);
    body->SetCollisionMask(collision_mask_);
    body->SetMass(0.01f);
    body->SetCollisionEventMode(COLLISION_ALWAYS);
    //body->SetTrigger(true);//wont sit on floor
}

void PickUp::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{
    //pickups only really care about collision with the character
    using namespace NodeCollision;

    Node* otherNode = static_cast<Node*>(eventData[P_OTHERNODE].GetPtr());
    Pawn* actor = otherNode->GetDerivedComponent<Pawn>();//this allows me to get a derived from pawn class component!

    if(actor != NULL)
    {
        Actor::HandleNodeCollision(eventType,eventData);
        collected_=true;
    }
}