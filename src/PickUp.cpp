#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Scene.h>

#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Scene/SceneEvents.h>

#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
//#include <Urho3D/Graphics/AnimationController.h>

#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Material.h>

#include "PickUp.h"
//i need to remove this dependency from here... it should not have to know about character class
//#include "../game/Pawn.h"
#include "Pawn.h"

#include <Urho3D/DebugNew.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Engine/DebugHud.h>


PickUp::PickUp(Context* context) :
    Actor(context),
    collected_(false)
{
    // Only the scene update event is needed: unsubscribe from the rest for optimization
    SetUpdateEventMask(USE_FIXEDUPDATE);
    collision_layer_ = 4;
    collision_mask_ = 33;
}
PickUp::~PickUp(){}

void PickUp::FixedUpdate(float timeStep)
{
    Actor::FixedUpdate(timeStep);
    //something
}
void PickUp::Setup()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();

    // Create the rendering component + animation controller
    //AnimatedModel* object = node_->CreateComponent<AnimatedModel>();
    StaticModel* object = node_->CreateComponent<StaticModel>();
    object->SetModel(cache->GetResource<Model>("Models/"+mesh_));
    //object->SetMaterial(cache->GetResource<Material>("Materials/Jack.xml"));
    object->SetCastShadows(true);
    //node_->CreateComponent<AnimationController>();

    // Set the head bone for manual control
    //object->GetSkeleton().GetBone("Bip01_Head")->animated_ = false;

    // Create rigidbody, and set non-zero mass so that the body becomes dynamic
    RigidBody* body = node_->CreateComponent<RigidBody>();
    body->SetCollisionLayer(collision_layer_);
    body->SetCollisionMask(collision_mask_);
    //body->SetMass(1.0f);
    body->SetMass(0.01f);

    // Set zero angular factor so that physics doesn't turn the character on its own.
    // Instead we will control the character yaw manually
    //body->SetAngularFactor(Vector3::ZERO);

    // Set the rigidbody to signal collision also when in rest, so that we get ground collisions properly
    body->SetCollisionEventMode(COLLISION_ALWAYS);

    //body->SetTrigger(true);//wont sit on floor
    //body->SetRestitution(0);
}

void PickUp::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{
    //pickups only really care about collision with the character
    using namespace NodeCollision;
    //Actor::HandleNodeCollision(eventType, eventData);
    //
    Node* otherNode = static_cast<Node*>(eventData[P_OTHERNODE].GetPtr());
    //RigidBody* otherBody = static_cast<RigidBody*>(eventData[P_OTHERBODY].GetPtr());
    //MemoryBuffer contacts(eventData[P_CONTACTS].GetBuffer());

    //Actor* actor = static_cast<Actor*>(otherNode->GetComponent<Character>());//    !!!!!!!  I NEED TO REMOVE THE NEED TO KNOW ABOUT CHARACTER CLASS
    Pawn* actor = otherNode->GetComponent<Pawn>();//actually only give this to a character
    //Actor* actor = otherNode->GetComponent<Actor>();
    //GetSubsystem<DebugHud>()->SetAppStats("pickup:", String( otherNode->GetNumComponents() ) );
    if(actor != NULL)
    {
        Actor::HandleNodeCollision(eventType,eventData);
        collected_=true;
        //LOGINFO("ACTOR CHARACTER COLLISION");
    }
}