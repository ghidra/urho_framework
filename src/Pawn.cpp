#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>

//#include <Urho3D/Graphics/AnimatedModel.h>
//#include <Urho3D/Graphics/StaticModel.h>
//#include <Urho3D/Graphics/Model.h>
//#include <Urho3D/Resource/ResourceCache.h>

#include "Pawn.h"
//#include "../core/ApplicationInput.h"
#include "Controller.h"

#include "State.h"
//#include "RagDoll.h"

#include <Urho3D/DebugNew.h>
#include <Urho3D/IO/Log.h>


Pawn::Pawn(Context* context) :
    Actor(context),
    moveForce_(3.0f),
    inAirMoveForce_(0.02f),
    brakeForce_(0.1f),
    jumpForce_(20.0f),
    //jumpDownForce_(0.5f),
    velocity_(Vector3()),
    yawSensitivity_(0.1f),
    inAirThresholdTime_(0.1f),
    okToJump_(true),
    inAirTimer_(0.0f),
    state_machine_(new StateMachine(context))
{
    // Only the scene update event is needed: unsubscribe from the rest for optimization
    //SetUpdateEventMask(USE_FIXEDUPDATE);
}
Pawn::~Pawn(){}

//void Actor::RegisterObject(Context* context)
//{
//    context->RegisterFactory<Actor>();
//}

/*void Pawn::Possess(ApplicationInput* applicationInput)
{
    applicationInput_ = applicationInput;
}*/

void Pawn::Possess(Controller* controller)
{
    controller_ = controller;
    state_machine_->Setup(controller_,static_cast<Pawn*>(this));
}

void Pawn::EquipWeapon(Weapon* weapon)
{
    weapon_ = weapon;
}
void Pawn::SetState(State* state)
{
    //state_ = state;
    //state_->Enter(this);
    state_machine_->SetState(state);
}

void Pawn::FixedUpdate(float timeStep)
{
    Actor::FixedUpdate(timeStep);
    //rigid body data

    if(node_->HasComponent<RigidBody>())
    {
        velocity_ = body_->GetLinearVelocity();
    }
    planeVelocity_ = Vector3(velocity_.x_, 0.0f, velocity_.z_);
    jumpVelocity_ = Vector3(0.0f, velocity_.y_, 0.0f);
}

/*void Pawn::Setup()
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

}*/

void Pawn::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{
    Actor::HandleNodeCollision(eventType, eventData);
    //
}