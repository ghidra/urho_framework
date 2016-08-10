#pragma once

#include <Urho3D/Scene/Component.h>

#include "../../framework/src/Projectile.h"

using namespace Urho3D;

namespace Urho3D
{

class Node;
class Scene;

}

class ProjectileTargeting : public Component
{
    URHO3D_OBJECT(ProjectileTargeting, Component);

public:
    ProjectileTargeting(Context* context);
    ~ProjectileTargeting();
    static void RegisterObject(Context* context);
    void Setup();
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandleNodeCollision(StringHash eventType, VariantMap& eventData);

    WeakPtr<Projectile> projectile_; /// The Projectile we target for.
    WeakPtr<Node> target_; /// The Node we are targeting.
    float period_; /// The time between target checks.
    float nextPeriod_; /// The time left before the next target check.
    bool readied_; /// Target check ready.
};