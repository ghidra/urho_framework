#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Serializable.h>

#include "Spawn.h"

Spawn::Spawn(Context* context) :
    Actor(context),
    //duration_(-0.1f),
    amount_(1),
    spawning_timer_(0.0f),
    spawning_interval_(0.8f),
    spawn_rate_(1.0f/spawning_interval_)
    //markedForRemoval_(false)
{
    debug_ = new Debug(context_);
}
Spawn::~Spawn(){}

void Spawn::RegisterObject(Context* context)
{
    context->RegisterFactory<Spawn>("Framework");

    URHO3D_ACCESSOR_ATTRIBUTE("Duration", GetDuration, SetDuration, float, -0.1f, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Amount", int, amount_, 1, AM_DEFAULT);
}
////EDITOR STUFF
void Spawn::OnSetAttribute(const AttributeInfo& attr, const Variant& src)
{
    Serializable::OnSetAttribute(attr, src);
}
////

void Spawn::FixedUpdate(float timeStep)
{
    timeIncrement_+=timeStep;

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

// void Spawn::MarkForRemoval()
// {
//     markedForRemoval_=true;
// }

// void Spawn::CheckForRemoval()
// {
//     if(markedForRemoval_ && node_!=NULL)
//     {
//         node_->Remove();
//     }
// }