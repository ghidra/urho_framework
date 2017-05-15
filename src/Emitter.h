#pragma once

#include <Urho3D/Urho3D.h>
#include <Urho3D/Container/Str.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Math/Vector3.h>

namespace Urho3D {
class Node;
class StringHash; }

class Emitter : public Urho3D::Component
{
  URHO3D_OBJECT(Emitter, Urho3D::Component);

public:
  Emitter(Urho3D::Context* context);
  ~Emitter();

  static void RegisterObject(Urho3D::Context* context);

  void Emit();

public:
  float timeToLive_;
  float periodMin_;
  float periodMax_;
  unsigned emitRate_; // Maximum number of emits per update; need at least 1.
  Urho3D::Vector3 angularVelocityMin_;
  Urho3D::Vector3 angularVelocityMax_;
  Urho3D::Vector3 linearVelocityMin_;
  Urho3D::Vector3 linearVelocityMax_;
  float angularDamping_;
  float linearDamping_;
  Urho3D::Vector3 scaleMin_;
  Urho3D::Vector3 scaleMax_;
  Urho3D::Vector3 gravity_;
  float massMin_;
  float massMax_;
  float durationMin_; // Duration of emitted objects, minimum.
  float durationMax_; // Duration of emitted objects, maximum.
  float friction_;
  unsigned collisionLayer_; // 0 = no CollisionShape
  unsigned collisionMask_;
  bool castShadows_;
  unsigned emitLimit_;
  Urho3D::String modelName_;
  Urho3D::String materialName_;

  unsigned emitted_;
  float periodNext_; /// The time left before the next check.

protected:
  void OnNodeSet(Urho3D::Node* node);
  void HandleSceneUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};
