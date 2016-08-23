#pragma once

#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Math/StringHash.h>

namespace Urho3D {
class Node; }

class AutoRemover : public Urho3D::Component {
  URHO3D_OBJECT(AutoRemover, Component);

public:
  AutoRemover(Urho3D::Context* context);
  ~AutoRemover();
  static void RegisterObject(Urho3D::Context* context);
  virtual void OnNodeSet(Urho3D::Node* node);

public:
  float timeToLive_;
  Urho3D::AutoRemoveMode mode_; // From Component.h

protected:
  void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};
