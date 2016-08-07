#include "ProjectileTargeting.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Scene/Scene.h>

using namespace Urho3D;

ProjectileTargeting::ProjectileTargeting(Context* context):
  Component(context), readied_(true), period_(300.0f)
{
}
ProjectileTargeting::~ProjectileTargeting()
{
}
//-------------------
//-------------------

void ProjectileTargeting::RegisterObject(Context* context)
{
    context->RegisterFactory<ProjectileTargeting>();
}


void ProjectileTargeting::Setup()
{
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(ProjectileTargeting, HandleUpdate));
    SubscribeToEvent(GetNode(), E_NODECOLLISION, URHO3D_HANDLER(ProjectileTargeting, HandleNodeCollision));
}

void ProjectileTargeting::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    if (readied_)
        return; // Don't start period timer until collision is triggered.

    float dt(eventData[Update::P_TIMESTEP].GetFloat());
    nextPeriod_ -= dt;
    if (nextPeriod_ <= 0)
    {
        readied_ = true;
        node_->GetComponent<CollisionShape>()->SetEnabled(true);
        nextPeriod_ += period_; // Wrap timer.
    }
}


void ProjectileTargeting::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{
    if (!readied_)
        return;

    Node* otherNode = static_cast<Node*>(eventData[NodeCollision::P_OTHERNODE].GetVoidPtr());
    if (!otherNode)
        return;

    target_ = otherNode;
    //GetSubsystem<DebugHud>()->SetAppStats("targeting target ", target_->GetName());
    readied_ = false;
    node_->GetComponent<CollisionShape>()->SetEnabled(false);
}
