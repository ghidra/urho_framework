#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Serializable.h>
#include <Urho3D/Scene/Scene.h>

#include <Urho3D/Physics/PhysicsWorld.h>//for the raycasting
#include <Urho3D/Math/Ray.h>

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
    damage_(1.0f),
    collision_size_(3.0f),
    ray_test_(false)
{
    //SetUpdateEventMask(USE_FIXEDUPDATE | USE_FIXEDPOSTUPDATE);
    SetUpdateEventMask(USE_FIXEDUPDATE);
    mesh_ = String("Sphere.mdl");
    collision_layer_ = 2;
    collision_mask_ = 56;
    side_ = SIDE_PLAYER;
}
Projectile::~Projectile(){}
void Projectile::RegisterObject(Context* context)
{
    context->RegisterFactory<Projectile>("Framework");

    URHO3D_ACCESSOR_ATTRIBUTE("Speed", GetSpeed, SetSpeed, float, 200.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Range", GetRange, SetRange, float, 100.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Damage", GetDamage, SetDamage, float, 1.0f, AM_DEFAULT);
}

void Projectile::FixedUpdate(float timeStep)
{
    Actor::FixedUpdate(timeStep);
    //get the position


    Vector3 pos = node_->GetWorldPosition();
    dir_ = body_->GetLinearVelocity();
    //Vector3 travelled = pos_last_-pos;
    float resize = dir_.Length()*timeStep;
    
    //for fast moving bullets we need to do some raycasting to make sure we dont go through

    //get the physics world to do some raycasting
    /*TURN OFF THE RAY CASTING FOR NOW
    if(ray_test_)
    {
        PhysicsRaycastResult result;
        PhysicsWorld* pw = node_->GetScene()->GetComponent<PhysicsWorld>();
        pw->RaycastSingle( result,Ray(body_->GetPosition(), dir_), speed_ *timeStep, body_->GetCollisionMask() );
        if(result.body_ != NULL)
        {
            //result.body_->ApplyImpulse(dir_);
            Impact(result.body_->GetNode(),result.position_,dir_);
        }
    }
    */

    //if(node_!=NULL)
    //{

    //shape_->SetPosition( Vector3(0.0f,(resize-collision_size_half_),0.0f) );
    //shape_->SetSize(Vector3(collision_size_,resize+collision_size_,collision_size_));
    
    //i need to consider stretching this based on movement
    //debug_->Hud("vel",String(resize) );

    //delete based on range
    Vector3 diff = pos_born_-pos;
    if(diff.Length()>range_)      
        MarkForRemoval();
        //node_->Remove();

    //pos_last_ = pos;
}
void Projectile::SetupBase()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    StaticModel* object = node_->CreateComponent<StaticModel>();
    object->SetModel(cache->GetResource<Model>("Models/"+mesh_));
    //object->SetMaterial(cache->GetResource<Material>("Materials/Jack.xml"));
    object->SetCastShadows(true);
    node_->SetScale(0.25f);

    SetRigidBody();
    CreateCollisionShape();

    //make it a trigger only
    RigidBody* body = node_->GetComponent<RigidBody>();
    body->SetTrigger(true);

    //shape_ = node_->CreateComponent<CollisionShape>();
    shape_->SetCapsule( collision_size_, collision_size_ );
    shape_->SetRotation( Quaternion::IDENTITY );//maybe it will carsh less
    //shape_->SetBox( Vector3(collision_size_, collision_size_,collision_size_) );

    pos_born_ = node_->GetWorldPosition();
    pos_last_ = node_->GetWorldPosition();
    collision_size_half_ = collision_size_/2.0f;

    //start checking for collision
    SubscribeToEvent(GetNode(), E_NODECOLLISION, URHO3D_HANDLER(Projectile, HandleNodeCollision));
}
void Projectile::Setup(const Vector3 direction)
{
    SetupBase();
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
    dir_ = direction*speed_;
    //RigidBody* body = node_->GetComponent<RigidBody>();
    body_->SetLinearVelocity(dir_);
}

void Projectile::Setup(VariantMap& parms)
{
    SetupBase();

    Vector3 direction = Vector3::FORWARD;
    bool usegravity = true;

    if( parms.Contains("direction") ) direction = parms["direction"].GetVector3();
    if( parms.Contains("range") ) range_ = parms["range"].GetFloat();
    if( parms.Contains("speed") ) speed_ = parms["speed"].GetFloat();
    if( parms.Contains("usegravity") ) usegravity= parms["usegravity"].GetBool();
    if( parms.Contains("raytest") ) ray_test_= parms["raytest"].GetBool();

    //collision_layer_ = 2;
    //collision_mask_ = 56;

    if( parms.Contains("collision_layer") ) collision_layer_= parms["collision_layer"].GetInt();
    if( parms.Contains("collision_mask") ) collision_mask_= parms["collision_mask"].GetInt();
    SetCollisionLayers(collision_layer_,collision_mask_);

    //RigidBody* body = node_->GetComponent<RigidBody>();
    if(!usegravity) body_->SetUseGravity(false);
    dir_ = direction*speed_;
    body_->SetLinearVelocity(dir_);
}

void Projectile::SetOwner(SharedPtr<Weapon> owner)
{
    owner_=owner;
}

void Projectile::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{
    //Weapons only really care about collision with the character
    using namespace NodeCollision;
    Actor::HandleNodeCollision(eventType, eventData);
    //
    Node* otherNode = static_cast<Node*>(eventData[P_OTHERNODE].GetPtr());
    //RigidBody* otherBody = static_cast<RigidBody*>(eventData[P_OTHERBODY].GetPtr());
    //MemoryBuffer contacts(eventData[P_CONTACTS].GetBuffer());

    //Actor* actor = static_cast<Actor*>(otherNode->GetComponent<Actor>());
    Actor* actor = otherNode->GetDerivedComponent<Actor>();
    //debug_->Hud("projectile hit",otherNode->GetName());

    //Impact(otherNode->GetComponent<RigidBody>(),contactPosition_,dir_);
    Impact(otherNode,contactPosition_,dir_);
    //if(actor != NULL)
    //{
        //Actor::HandleNodeCollision(eventType,eventData);
        //collected_=true;
        //LOGINFO("ACTOR CHARACTER COLLISION");
    //}
}
void Projectile::Impact(Node* node, const Vector3 pos, const Vector3 dir)
{
    //RigidBody* body = node->GetComponent<RigidBody>();
    Actor* actor = node->GetDerivedComponent<Actor>();
    //if(body!=NULL) body->ApplyImpulse(dir*0.001f);
    if(actor!=NULL)
    {
        actor->TakeDamage(damage_,pos,dir,level_);
        if(owner_!=NULL)//if we were passed a weapon owner, then call OnProjectileHit method
            owner_->OnProjectileHitActor(actor);
    }
    
    MarkForRemoval();
    //node_->Remove();
        
}
/*void Projectile::Impact(RigidBody* body, const Vector3 pos, const Vector3 dir)
{
    body->ApplyImpulse(dir);
    if(node_!=NULL)
        node_->Remove();
}*/