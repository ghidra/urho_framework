#include "AutoRemover.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Math/StringHash.h>
#include <Urho3D/Engine/EngineEvents.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>

using namespace Urho3D;

AutoRemover::AutoRemover(Context* context) :
  Component(context)
  , timeToLive_(1.0f), mode_(AutoRemoveMode::REMOVE_NODE) {
}

AutoRemover::~AutoRemover() {}

void AutoRemover::RegisterObject(Context* context) {
  context->RegisterFactory<AutoRemover>(); }

void AutoRemover::OnNodeSet(Node* node) {
  if (!node) {
    return; }

  SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(AutoRemover, HandleUpdate)); }

void AutoRemover::HandleUpdate(StringHash eventType, VariantMap& eventData) {
  float dt(eventData[Update::P_TIMESTEP].GetFloat());
  // Disappear when duration expired.
  if (timeToLive_ >= 0.0f) {
    timeToLive_ -= dt;
    if (timeToLive_ <= 0.0f) {
      DoAutoRemove(mode_); } } }
