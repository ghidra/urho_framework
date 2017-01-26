#include <Urho3D/Precompiled.h>

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Scene.h>

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Scene/SceneEvents.h>


#include "IKTarget.h"

#include <Urho3D/Graphics/DebugRenderer.h>

#include <Urho3D/DebugNew.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Engine/DebugHud.h>


IKTarget::IKTarget(Context* context) :
    LogicComponent(context)
{
    //SetUpdateEventMask(USE_UPDATE);
}
IKTarget::~IKTarget(){}

void IKTarget::RegisterObject(Context* context)
{
    context->RegisterFactory<IKTarget>("Framework");

    URHO3D_ACCESSOR_ATTRIBUTE("IK Node", GetIKNodeName, SetIKNodeName, String, String::EMPTY, AM_DEFAULT);

}
void IKTarget::DelayedStart()
{
    SubscribeToEvent(E_SCENEDRAWABLEUPDATEFINISHED, URHO3D_HANDLER(IKTarget, HandleSceneDrawableUpdateFinished));
}

void IKTarget::SetIK(const String parentNode)
{
	Node* n = node_->GetScene()->GetChild(parentNode,true);
	IK* ik = n->GetComponent<IK>();

	if ( ik != NULL )
		SetIK(ik);
}
void IKTarget::SetIK(IK* ik)
{
	URHO3D_LOGWARNING("----WE GOT SOMETHING TO MOVE IN IKTARGET----");
	ik_ = ik;
}

void IKTarget::HandleSceneDrawableUpdateFinished(StringHash eventType, VariantMap& eventData)
{
    //solve for ik
    if(ik_ != NULL)
    {
    	ik_->SetTarget(node_->GetWorldPosition());
    	//URHO3D_LOGWARNING( String(node_->GetWorldPosition()) );
    }      
}

////EDITOR STUFF
void IKTarget::OnSetAttribute(const AttributeInfo& attr, const Variant& src)
{
	Serializable::OnSetAttribute(attr, src);
}
void IKTarget::SetIKNodeName(const String& IKNodeName)
{
	IKNodeName_=IKNodeName;
	SetIK(IKNodeName);
	MarkNetworkUpdate();
}
