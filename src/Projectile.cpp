#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Scene.h>

#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Material.h>

#include <Urho3D/Physics/PhysicsEvents.h>

#include "Projectile.h"


Projectile::Projectile(Context* context) :
    Actor(context),
    speed_(200.0f),
    range_(100.0f),
    damage_(1.0f)
{
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

    StaticModel* object = node_->CreateComponent<StaticModel>();
    object->SetModel(cache->GetResource<Model>("Models/"+mesh_));
    //object->SetMaterial(cache->GetResource<Material>("Materials/Jack.xml"));
    object->SetCastShadows(true);
    node_->SetScale(0.25f);

    SetRigidBody();

    shape_ = node_->CreateComponent<CollisionShape>();
    shape_->SetCapsule(3.0f, 10.0f, Vector3(0.0f, 5.0f, 0.0f));

    pos_born_ = node_->GetWorldPosition();
}
void Projectile::FixedUpdate(float timeStep)
{
    Actor::FixedUpdate(timeStep);
    //delete based on range
    Vector3 pos = node_->GetPosition();
    Vector3 diff = pos_born_-pos;
    if(diff.Length()>range_)
        node_->Remove();

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
    body->SetLinearVelocity(direction*speed_);
}

void Projectile::Setup(VariantMap& parms)
{
    Vector3 direction = Vector3::UP;
    bool usegravity = true;

    if( parms.Contains("direction") ) direction = parms["direction"].GetVector3();
    if( parms.Contains("range") ) range_ = parms["range"].GetFloat();
    if( parms.Contains("speed") ) speed_ = parms["speed"].GetFloat();
    if( parms.Contains("usegravity") ) usegravity= parms["usegravity"].GetBool();

    RigidBody* body = node_->GetComponent<RigidBody>();
    if(!usegravity)
        body->SetUseGravity(false);
    body->SetLinearVelocity(direction*speed_);
}

void Projectile::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{
    //Weapons only really care about collision with the character
    using namespace NodeCollision;
    //Actor::HandleNodeCollision(eventType, eventData);
    //
    Node* otherNode = static_cast<Node*>(eventData[P_OTHERNODE].GetPtr());
    //RigidBody* otherBody = static_cast<RigidBody*>(eventData[P_OTHERBODY].GetPtr());
    //MemoryBuffer contacts(eventData[P_CONTACTS].GetBuffer());

    //Actor* actor = static_cast<Actor*>(otherNode->GetComponent<Actor>());
    Actor* actor = otherNode->GetDerivedComponent<Actor>();
    debug_->Hud("projectile hit",otherNode->GetName());
    //if(actor != NULL)
    //{
        //Actor::HandleNodeCollision(eventType,eventData);
        //collected_=true;
        //LOGINFO("ACTOR CHARACTER COLLISION");
    //}
}