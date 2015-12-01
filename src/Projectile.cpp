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

#include "Projectile.h"
//#include "../piece/Character.h"

#include <Urho3D/DebugNew.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Engine/DebugHud.h>


Projectile::Projectile(Context* context) :
    Actor(context),
    range_(40.0f),
    damage_(1.0f)
{
    // Only the scene update event is needed: unsubscribe from the rest for optimization
    SetUpdateEventMask(USE_FIXEDUPDATE);
    mesh_ = String("Sphere.mdl");
    collision_layer_ = 2;
    collision_mask_ = 56;
    side_ = SIDE_PLAYER;

}
Projectile::~Projectile(){}
void Projectile::RegisterObject(Context* context)
{
    context->RegisterFactory<Projectile>();

}
void Projectile::Start()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    //node_->SetPosition(Vector3(4.0f, 1.0f, 0.0f));//objectNode

    // Create the rendering component + animation controller
    //AnimatedModel* object = node_->CreateComponent<AnimatedModel>();
    StaticModel* object = node_->CreateComponent<StaticModel>();
    object->SetModel(cache->GetResource<Model>("Models/"+mesh_));
    //object->SetMaterial(cache->GetResource<Material>("Materials/Jack.xml"));
    object->SetCastShadows(true);
    node_->SetScale(0.25f);

    //readyForUpdate_=true;
    RigidBody* body = node_->CreateComponent<RigidBody>();
    body->SetCollisionLayer(collision_layer_);
    body->SetCollisionMask(collision_mask_);
    body->SetMass(1.0f);
    body->SetTrigger(true);

    pos_born_ = node_->GetWorldPosition();
}
void Projectile::FixedUpdate(float timeStep)
{
    //if(readyForUpdate_)
        //Thats it, i need to wait until the fucking thing is set before I call this thing in here
        Actor::FixedUpdate(timeStep);
    //GetSubsystem<DebugHud>()->SetAppStats("projectile_position:", String(node_->GetWorldPosition()) );
    //something
}
void Projectile::Setup(const Vector3 direction)
{
    //GetSubsystem<DebugHud>()->SetAppStats("projectile:", String("trying to make projectile") );

	/*ResourceCache* cache = GetSubsystem<ResourceCache>();

    //node_->SetPosition(Vector3(4.0f, 1.0f, 0.0f));//objectNode

    // Create the rendering component + animation controller
    //AnimatedModel* object = node_->CreateComponent<AnimatedModel>();
    StaticModel* object = node_->CreateComponent<StaticModel>();
    object->SetModel(cache->GetResource<Model>("Models/"+mesh_));
    //object->SetMaterial(cache->GetResource<Material>("Materials/Jack.xml"));
    object->SetCastShadows(true);
    */
    //node_->CreateComponent<AnimationController>();

    // Create rigidbody, and set non-zero mass so that the body becomes dynamic
    /*RigidBody* body = node_->CreateComponent<RigidBody>();
    body->SetCollisionLayer(collision_layer_);
    body->SetCollisionMask(collision_mask_);
    body->SetMass(1.0f);
    body->SetTrigger(true);
    */
    RigidBody* body = node_->GetComponent<RigidBody>();
    body->SetLinearVelocity(direction*80.0f);

    // Set zero angular factor so that physics doesn't turn the character on its own.
    // Instead we will control the character yaw manually
    /*body->SetAngularFactor(Vector3::ZERO);

    // Set the rigidbody to signal collision also when in rest, so that we get ground collisions properly
    body->SetCollisionEventMode(COLLISION_ALWAYS);*/
    
    
    //GetSubsystem<DebugHud>()->SetAppStats("projectile_position:", String(pos_born_) );
    //GetSubsystem<DebugHud>()->SetAppStats("projectile_position:", String(node_->GetWorldPosition()) );

    //use the pases in weapon info to determine the ways to set this things

    

    //body->SetLinearVelocity(direction*80.0f);
}
/*void Weapon::Attach(Node* bone)
{

}*/

void Projectile::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{
    //Weapons only really care about collision with the character
   /* using namespace NodeCollision;
    //Actor::HandleNodeCollision(eventType, eventData);
    //
    Node* otherNode = static_cast<Node*>(eventData[P_OTHERNODE].GetPtr());
    //RigidBody* otherBody = static_cast<RigidBody*>(eventData[P_OTHERBODY].GetPtr());
    //MemoryBuffer contacts(eventData[P_CONTACTS].GetBuffer());

    Actor* actor = static_cast<Actor*>(otherNode->GetComponent<Character>());
    if(actor != NULL)
    {
        Actor::HandleNodeCollision(eventType,eventData);
        //collected_=true;
        //LOGINFO("ACTOR CHARACTER COLLISION");
    }*/
}