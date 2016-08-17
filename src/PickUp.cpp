#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Scene.h>

#include <Urho3D/Physics/PhysicsEvents.h>//this is for the namespace node collision
#include <Urho3D/Physics/RigidBody.h>

#include "PickUp.h"
#include "Pawn.h"



PickUp::PickUp(Context* context) :
    Actor(context),
    collected_(false),
    life_span_(-1.0f)
{
    //SetUpdateEventMask(USE_FIXEDUPDATE | USE_FIXEDPOSTUPDATE);
    SetUpdateEventMask(USE_FIXEDUPDATE);
    collision_layer_ = 4;
    collision_mask_ = 33;
    //material_="";
}
PickUp::~PickUp(){}

void PickUp::FixedUpdate(float timeStep)
{
    Actor::FixedUpdate(timeStep);

    life_=duration_/life_span_;
    //node_->SetWorldPosition( node_->GetWorldPosition()*Vector3(1.0,0.0,1.0) );
    CheckForRemoval();
}
void PickUp::Setup()
{
    Actor::Setup();
	/*ResourceCache* cache = GetSubsystem<ResourceCache>();

    StaticModel* object = node_->CreateComponent<StaticModel>();
    object->SetModel(cache->GetResource<Model>("Models/"+mesh_));
    //object->SetMaterial(cache->GetResource<Material>("Materials/Jack.xml"));
    object->SetCastShadows(true);


    body_ = node_->CreateComponent<RigidBody>();
    body_->SetCollisionLayer(collision_layer_);
    body_->SetCollisionMask(collision_mask_);
    body_->SetMass(0.01f);*/
    body_->SetCollisionEventMode(COLLISION_ALWAYS);
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
        if(actor->canCollect_)
        {
            Actor::HandleNodeCollision(eventType,eventData);
            collected_=true;
        }
    }
}