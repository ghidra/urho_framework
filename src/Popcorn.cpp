#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>

#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>

#include <Urho3D/IO/MemoryBuffer.h>

#include "Popcorn.h"

Popcorn::Popcorn(Context* context) :
    LogicComponent(context),
    mesh_(String("Box.mdl")),
    duration_(0.4f),
    material_("DefaultGrey")
{
    debug_ = new Debug(context_);
}
Popcorn::~Popcorn(){}

void Popcorn::FixedUpdate(float timeStep)
{
    //lets move the object accourding to our forces
    velocity_ += (gravity_*timeStep) + (force_*timeStep);
    Vector3 p = node_->GetWorldPosition();
    node_->SetWorldPosition( p + velocity_ );

    timeIncrement_+=timeStep;
    // Disappear when duration expired
    if (duration_ >= 0.0f){
      duration_ -= timeStep;
      if (duration_ <= 0.0f)
        if(node_!=NULL)
            node_->Remove();
    }
}

void Popcorn::Setup(Vector3 dir, Vector3 g, Vector3 f, Vector3 angular, float ang_var)
{
    velocity_ = dir;
    gravity_ = g;
    force_ = f;

    angular_ = angular;//the angluar velocity. The normalized should be the rotation axis, the lenth the speed
    angular_variance_ = ang_var;


    /*ResourceCache* cache = GetSubsystem<ResourceCache>();

    StaticModel* object = node_->CreateComponent<StaticModel>();
    object->SetModel(cache->GetResource<Model>("Models/"+mesh_));
    if(material_!="")
        object->SetMaterial(cache->GetResource<Material>("Materials/"+material_+".xml"));
    object->SetCastShadows(true);
    */
}

float Popcorn::Fit(float v, float l1, float h1, float l2, float h2)
{
    return Clamp( l2 + (v - l1) * (h2 - l2) / (h1 - l1), l2,h2);
}
