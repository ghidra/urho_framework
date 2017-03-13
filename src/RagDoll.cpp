#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>//will not complie without this?
#include <Urho3D/Core/Context.h>

#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Physics/PhysicsEvents.h>

#include "RagDoll.h"

#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/RigidBody.h>

#include <Urho3D/DebugNew.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Engine/DebugHud.h>

RagDoll::RagDoll(Context* context):
    LogicComponent(context)
{
}
RagDoll::~RagDoll(){}

void RagDoll::RegisterObject(Context* context)
{
    context->RegisterFactory<RagDoll>("Framework");
}
void RagDoll::OnSetAttribute(const AttributeInfo& attr, const Variant& src)
{
    Serializable::OnSetAttribute(attr, src);
}

void RagDoll::Activate()
{
	//turn the whole thing on
	//pawn_->GetNode()->RemoveComponent<RigidBody>();
    //pawn_->GetNode()->RemoveComponent<CollisionShape>();
	//pawn_->GetNode()->RemoveComponent(pawn_->GetBody());//remove the main components
    //pawn_->GetNode()->RemoveComponent(pawn_->GetShape());//remove the main components
    //if(node_->HasComponent<RigidBody>())
    //{
    
    //GetSubsystem<DebugHud>()->SetAppStats("state:", name_ );
    node_->RemoveComponent<RigidBody>();
    node_->RemoveComponent<CollisionShape>();
    
    //}

    AnimatedModel* model = node_->GetComponent<AnimatedModel>();
    Skeleton& skeleton = model->GetSkeleton();
    for (unsigned i = 0; i < skeleton.GetNumBones(); ++i)
    	skeleton.GetBone(i)->animated_ = false;

   	for (unsigned i = 0; i < boneNode_.Size(); ++i)
   	{
        //URHO3D_LOGINFO(String(i));
   		RigidBody* rb = boneNode_[i]->GetComponent<RigidBody>();
   		rb->SetTrigger(false);
        rb->SetMass(1.0f);
   	}
}

/*void RagDoll::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{
    // Check collision contacts and see if character is standing on ground (look for a contact that has near vertical normal)
    using namespace NodeCollision;
    
    MemoryBuffer contacts(eventData[P_CONTACTS].GetBuffer());

    while (!contacts.IsEof())
    {
        Vector3 contactPosition_ = contacts.ReadVector3();
        Vector3 contactNormal_ = contacts.ReadVector3();
        float contactDistance_ = contacts.ReadFloat();
        float contactImpulse_ = contacts.ReadFloat();
        
        // If contact is below node center and mostly vertical, assume it's a ground contact
        if (contactPosition_.y_ < (node_->GetPosition().y_ + 1.0f))
        {
            float level = Abs(contactNormal_.y_);
            //if (level > 0.75)
            //    onGround_ = true;
        }
    }
}*/

//////------

void RagDoll::Bone(const String& start, const String& stop, ShapeType type, const Vector3& size)
{


	//Node* joint = pawn_->GetNode()->GetChild(start,true);
	//Node* jointend = pawn_->GetNode()->GetChild(stop,true);
    Node* joint = node_->GetChild(start,true);
    Node* jointend = node_->GetChild(stop,true);

	Vector3 begin = joint->GetWorldPosition();
	Vector3 end = jointend->GetWorldPosition();

	Vector3 vec = begin-end;
	float length = vec.Length();
	Vector3 nsize = size+Vector3(0.0f,length,0.0f);;

	//----make the components
	RigidBody* rb = joint->CreateComponent<RigidBody>();
	CollisionShape* cs = joint->CreateComponent<CollisionShape>();

	SetShape(cs,type,nsize);
	boneNode_.Push(SharedPtr<Node>(joint));
	rb->SetTrigger(true);
}
void RagDoll::Bone(const String& start, const float length, ShapeType type, const Vector3& size)
{
	//Node* joint = pawn_->GetNode()->GetChild(start,true);
    Node* joint = node_->GetChild(start,true);
	Vector3 nsize = size + Vector3(0.0f,length,0.0f);
	//----make the components
	RigidBody* rb = joint->CreateComponent<RigidBody>();
	CollisionShape* cs = joint->CreateComponent<CollisionShape>();
	SetShape(cs,type,size);
	boneNode_.Push(SharedPtr<Node>(joint));
	rb->SetTrigger(true);
}
//private
//-----
void RagDoll::SetShape(CollisionShape* cs, const unsigned type, const Vector3 size)
{
	switch(type)
	{
		case SHAPE_CAPSULE:
			cs->SetCapsule(size.x_,size.y_,Vector3(0.0f,size.y_/2.0f,0.0f));
			break;
		case SHAPE_BOX:
			cs->SetBox(size,Vector3(0.0f,size.y_/2.0f,0.0f));
			break;
	}
}
//-----
//constraint
void RagDoll::Constrain(const String& boneName, const String& parentName, ConstraintType type, const Vector3& axis, const Vector3& parentAxis, const Vector2& highLimit, const Vector2& lowLimit, bool disableCollision)
{
    //Node* boneNode = pawn_->GetNode()->GetChild(boneName, true);
    //Node* parentNode = pawn_->GetNode()->GetChild(parentName, true);
    Node* boneNode = node_->GetChild(boneName, true);
    Node* parentNode = node_->GetChild(parentName, true);
    if (!boneNode)
    {
        //URHO3D_LOGWARNING("Could not find bone " + boneName + " for creating ragdoll constraint");
        return;
    }
    if (!parentNode)
    {
        //URHO3D_LOGWARNING("Could not find bone " + parentName + " for creating ragdoll constraint");
        return;
    }

    Constraint* constraint = boneNode->CreateComponent<Constraint>();
    constraint->SetConstraintType(type);
    // Most of the constraints in the ragdoll will work better when the connected bodies don't collide against each other
    constraint->SetDisableCollision(disableCollision);
    // The connected body must be specified before setting the world position
    constraint->SetOtherBody(parentNode->GetComponent<RigidBody>());
    // Position the constraint at the child bone we are connecting
    constraint->SetWorldPosition(boneNode->GetWorldPosition());
    // Configure axes and limits
    constraint->SetAxis(axis);
    constraint->SetOtherAxis(parentAxis);
    constraint->SetHighLimit(highLimit);
    constraint->SetLowLimit(lowLimit);
}