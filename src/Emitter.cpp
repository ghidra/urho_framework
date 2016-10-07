#include "Emitter.h"
#include "Remover.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Math/StringHash.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/IO/Log.h>

//#include <Urho3D/Scene/Animatable.h>
//#include <Urho3D/Scene/ObjectAnimation.h>
//#include <Urho3D/Scene/ValueAnimation.h>
#define dbg(LABEL, STRING) context_->GetSubsystem<DebugHud>()->SetAppStats(LABEL,STRING)

using namespace Urho3D;

Emitter::Emitter(Context* context) :
  Component(context)
, timeToLive_(-1.0f)
, durationMin_(10.0f)
, durationMax_(10.0f)
, periodMin_(0.1f)
, periodMax_(0.1f)
, emitRate_(1)
, scaleMin_(Vector3::ONE)
, scaleMax_(Vector3::ONE)
, angularVelocityMin_(Vector3::ZERO)
, angularVelocityMax_(Vector3::ZERO)
, linearVelocityMin_(Vector3::ZERO)
, linearVelocityMax_(Vector3::ZERO)
, angularDamping_(0.0f)
, linearDamping_(0.0f)
, massMin_(1.0f)
, massMax_(1.0f)
, castShadows_(true)
, emitLimit_(0)
, gravity_(Vector3(0.0f,-9.81f,0.0f))
, friction_(0.0f)
, collisionLayer_(0)
, collisionMask_(1)
, emitted_(0)
, periodNext_(0)
, modelName_("Models/Box.mdl")
, materialName_("Materials/DefaultGrey.xml")
{}

Emitter::~Emitter() {}

void Emitter::RegisterObject(Context* context) {
  context->RegisterFactory<Emitter>(); }

void Emitter::OnNodeSet(Node* node) {
  if (!node) {
    return; }

  SubscribeToEvent(E_PHYSICSPRESTEP, URHO3D_HANDLER(Emitter, HandleSceneUpdate));
}

void Emitter::HandleSceneUpdate(StringHash eventType, VariantMap& eventData) {
  if (!node_->IsEnabled()) {
    return; }
  float dt(eventData[SceneUpdate::P_TIMESTEP].GetFloat());

  // If limited duration, timestep.
  if (timeToLive_ >= 0) {
    timeToLive_ -= dt;
    // If duration expired, remove the whole emitter system.
    if (timeToLive_ <= 0) {
      node_->Remove();
      return; } }

  periodNext_ -= dt;
  if (periodNext_ <= 0) {
    for (unsigned i = 0; i < emitRate_ && emitted_ < emitLimit_; ++i) {
      Emit(); }
    periodNext_ = Random(periodMin_, periodMax_); }
}


void Emitter::Emit() {
  Node* n(node_->CreateChild("emit"));

  Remover* r(n->CreateComponent<Remover>(LOCAL));//CreateMode::
  r->timeToLive_ = Random(durationMin_, durationMax_);

  RigidBody* body(n->CreateComponent<RigidBody>());
  if (massMin_ < massMax_) {
    body->SetMass(Random(massMin_,massMax_)); }
  else {
    body->SetMass(massMin_); }

  body->SetGravityOverride(gravity_);

  if (angularVelocityMin_ != angularVelocityMax_) {
    float velAx, velAy, velAz;
    velAx = Random(angularVelocityMin_.x_,angularVelocityMax_.x_);
    velAy = Random(angularVelocityMin_.y_,angularVelocityMax_.y_);
    velAz = Random(angularVelocityMin_.z_,angularVelocityMax_.z_);
    body->SetAngularVelocity(Vector3(velAx,velAy,velAz)); }
  else {
    body->SetAngularVelocity(angularVelocityMin_); }

  if (linearVelocityMin_ != linearVelocityMax_) {
    float velLx, velLy, velLz;
    velLx = Random(linearVelocityMin_.x_,linearVelocityMax_.x_);
    velLy = Random(linearVelocityMin_.y_,linearVelocityMax_.y_);
    velLz = Random(linearVelocityMin_.z_,linearVelocityMax_.z_);
    body->SetLinearVelocity(Vector3(velLx,velLy,velLz)); }
  else {
    body->SetLinearVelocity(linearVelocityMin_); }

  body->SetFriction(friction_);
  body->SetAngularDamping(angularDamping_);
  body->SetLinearDamping(linearDamping_);

  body->SetCollisionLayerAndMask(collisionLayer_, collisionMask_);

  ResourceCache* cache(GetSubsystem<ResourceCache>());

  StaticModel* m(n->CreateComponent<StaticModel>());
  m->SetModel(cache->GetResource<Model>(modelName_));
  m->SetMaterial(cache->GetResource<Material>(materialName_));
  m->SetCastShadows(castShadows_);


///@TODO other shapes
  if (collisionLayer_ != 0) {
    CollisionShape* shape(n->CreateComponent<CollisionShape>());
    shape->SetBox(Vector3::ONE); }


  if (scaleMin_ != scaleMax_) {
    float scaleX(Random(scaleMin_.x_,scaleMax_.x_));
    float scaleY(Random(scaleMin_.y_,scaleMax_.y_));
    float scaleZ(Random(scaleMin_.z_,scaleMax_.z_));
    n->SetScale(Vector3(scaleX,scaleY,scaleZ)); }
  else {
    n->SetScale(scaleMin_); }

  emitted_++;
}
