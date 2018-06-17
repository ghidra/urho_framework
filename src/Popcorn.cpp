#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Serializable.h>

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
    material_("DefaultGrey"),
    static_(false)
{
    debug_ = new Debug(context_);
}
Popcorn::~Popcorn(){}

void Popcorn::SetStatic(bool enable )
{
    static_=enable;
}

void Popcorn::RegisterObject(Context* context)
{
    context->RegisterFactory<Popcorn>("Framework");

    URHO3D_ACCESSOR_ATTRIBUTE("Duration", GetDuration, SetDuration, float, 0.4f, AM_DEFAULT);

}
////EDITOR STUFF
void Popcorn::OnSetAttribute(const AttributeInfo& attr, const Variant& src)
{
    Serializable::OnSetAttribute(attr, src);
}
/////
void Popcorn::FixedUpdate(float timeStep)
{
    if(!static_)
    {
        //lets move the object accourding to our forces
        velocity_ += (gravity_*timeStep) + (force_*timeStep);
        Vector3 p = node_->GetWorldPosition();
        node_->SetWorldPosition( p + velocity_ );

        //get the rotation information
        float rot_speed = angular_.Length();
        if(rot_speed>0.001f)
        {
            Quaternion rot = Quaternion( 240.0f * rot_speed * timeStep, angular_.Normalized() );
            //node_->SetWorldRotation(  rot * node_->GetWorldRotation() );
            node_->SetWorldRotation( rot );
        }
    }

    //now deal with Actor type time increment
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
