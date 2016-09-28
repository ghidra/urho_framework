#include "ProjectileTargeting.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>

using namespace Urho3D;

ProjectileTargeting::ProjectileTargeting(Context* context):
  Component(context), readied_(true), period_(1.0f)
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
    collisionShape_ = node_->GetComponent<CollisionShape>();
    if (!collisionShape_)
        URHO3D_LOGWARNING("ProjectileTargeting Setup() without CollisionShape in node.");

    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(ProjectileTargeting, HandleSceneUpdate));
    SubscribeToEvent(GetNode(), E_NODECOLLISION, URHO3D_HANDLER(ProjectileTargeting, HandleNodeCollision));
}

void ProjectileTargeting::HandleSceneUpdate(StringHash eventType, VariantMap& eventData)
{
    if (readied_)
        return; // Don't start period timer until collision is triggered.

    float dt(eventData[SceneUpdate::P_TIMESTEP].GetFloat());
    nextPeriod_ -= dt;
    if (nextPeriod_ <= 0)
    {
        readied_ = true;
        if (collisionShape_)
            collisionShape_->SetEnabled(true);
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
    readied_ = false;
    if (collisionShape_)
        collisionShape_->SetEnabled(false);
}
