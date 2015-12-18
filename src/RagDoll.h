#ifndef RAGDOLL_H
#define RAGDOLL_H

#pragma once

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Physics/Constraint.h>
#include "Pawn.h"

class Pawn;

class RagDoll : public LogicComponent
{
    URHO3D_OBJECT(RagDoll,LogicComponent);
    
public:
    RagDoll(Context* context);
    ~RagDoll();

    static void RegisterObject(Context* context);

    void Activate();//just turn it all on

    void Setup(Pawn* pawn);//maybe also pass in the number of "bones" to hold for memory allocation

    void Bone(const String& start, const String& stop, ShapeType type, const Vector3& size);
    void Bone(const String& start, const float length, ShapeType type, const Vector3& size);

    void Constrain(const String& boneName, const String& parentName, ConstraintType type, const Vector3& axis, const Vector3& parentAxis, const Vector2& highLimit, const Vector2& lowLimit, bool disableCollision = true);


private:
    //virtual void HandleNodeCollision(StringHash eventType, VariantMap& eventData);

    void SetShape(CollisionShape* cs, const unsigned type, const Vector3 size);
    //const String states_[50];
    //SharedPtr<Pawn> pawn_; 
    Vector< SharedPtr<Node> > boneNode_;//the root node of the rigid components .Push() and .Size()
};
#endif