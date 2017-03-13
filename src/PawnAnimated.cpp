#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Serializable.h>

#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "PawnAnimated.h"
#include "Controller.h"

#include "RagDoll.h"

#include <Urho3D/DebugNew.h>
#include <Urho3D/IO/Log.h>


PawnAnimated::PawnAnimated(Context* context) :
    Pawn(context)
{
}
PawnAnimated::~PawnAnimated(){}

void PawnAnimated::Setup()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    //node_->SetPosition(Vector3(0.0f, 1.0f, 0.0f));//objectNode

    // Create the rendering component + animation controller
    AnimatedModel* object = node_->CreateComponent<AnimatedModel>();
    //StaticModel* object = node_->CreateComponent<StaticModel>();
    object->SetModel(cache->GetResource<Model>("Models/"+mesh_));
    //object->SetMaterial(cache->GetResource<Material>("Materials/Jack.xml"));
    object->SetCastShadows(true);

    animationController_ = node_->CreateComponent<AnimationController>();

    SetRigidBody();
    body_->SetAngularFactor(Vector3::ZERO);
    body_->SetCollisionEventMode(COLLISION_ALWAYS);// Set the rigidbody to signal collision also when in rest, so that we get ground collisions properly

    //we still need to setup the collisionshape in the child class
    
    //here we are setting the ragdoll object waiting to accept some commands to build out
    ragdoll_ = node_->CreateComponent<RagDoll>();//

}
void PawnAnimated::RegisterObject(Context* context)
{
    context->RegisterFactory<PawnAnimated>("Framework");
}
void PawnAnimated::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{
    Pawn::HandleNodeCollision(eventType, eventData);
}