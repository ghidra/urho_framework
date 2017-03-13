#include "Remover.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Serializable.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Math/StringHash.h>
#include <Urho3D/Engine/EngineEvents.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>

using namespace Urho3D;

Remover::Remover(Context* context) :
  Component(context)
  , timeToLive_(-1)
{
}

Remover::~Remover() {
}

void Remover::RegisterObject(Context* context)
{
  context->RegisterFactory<Remover>("Framework");
  URHO3D_ACCESSOR_ATTRIBUTE("Time to Live", GetDuration, SetDuration, float, -1.0f, AM_DEFAULT);
}
void Remover::OnSetAttribute(const AttributeInfo& attr, const Variant& src)
{
    Serializable::OnSetAttribute(attr, src);
}

void Remover::OnNodeSet(Node* node)
{
    if (!node)
    {
        return;
    }
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Remover, HandleUpdate));
}

void Remover::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
  using namespace Update;
  float dt(eventData[P_TIMESTEP].GetFloat());
  if (timeToLive_ >= 0)
  {
    timeToLive_ -= dt;
    if (timeToLive_ <= 0)
    {
      node_->Remove();
    }
  }
}
