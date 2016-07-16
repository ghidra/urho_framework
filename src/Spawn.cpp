#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>

#include "Spawn.h"

Spawn::Spawn(Context* context) :
    LogicComponent(context),
    duration_(-0.1f),
    amount_(1),
    markedForRemoval_(false)
{
    debug_ = new Debug(context_);
}
Spawn::~Spawn(){}

void Spawn::FixedUpdate(float timeStep)
{
    if (duration_ >= 0.0f){
      duration_ -= timeStep;
      if (duration_ <= 0.0f)
        MarkForRemoval();
    }

}

void Spawn::Setup(const Vector3 pos)
{
    node_->SetPosition(pos);
}

void Spawn::MarkForRemoval()
{
    markedForRemoval_=true;
}

void Spawn::CheckForRemoval()
{
    if(markedForRemoval_ && node_!=NULL)
    {
        node_->Remove();
    }
}