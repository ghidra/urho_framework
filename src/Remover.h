#pragma once

#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Component.h>

namespace Urho3D
{
class Node;
}

class Remover : public Urho3D::Component
{
  URHO3D_OBJECT(Remover, Component);

public:
  Remover(Urho3D::Context* context);
  ~Remover();
  static void RegisterObject(Urho3D::Context* context);

  /// Remove node after this time, only if the value is non-negative.
  float timeToLive_;

  void OnSetAttribute(const Urho3D::AttributeInfo& attr, const Urho3D::Variant& src);
  float GetDuration() const {return timeToLive_;};
  void SetDuration(float value){timeToLive_=value;};

protected:
  void OnNodeSet(Urho3D::Node* node);

  void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};
