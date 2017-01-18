#ifndef IK_H
#define IK_H

#pragma once

#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;

/*namespace Urho3D
{

class Node;
class Scene;

}*/

class IK : public LogicComponent
{
    URHO3D_OBJECT(IK,LogicComponent);
    
public:
    IK(Context* context);
    ~IK();

    static void RegisterObject(Context* context);

    //virtual void Update(float timeStep);
    virtual void CreateChain(const String bone);
    virtual void SetTarget(Vector3 targetPos);
    virtual void ToggleDebug(){drawDebug_=!drawDebug_;};

    //editor functions
    virtual void OnSetAttribute(const AttributeInfo& attr, const Variant& src);
    void SetEffectorName(const String& effectorName);
    const String& GetEffectorName() const { return effectorName_; };
    void SetDEBUG(bool de);
    bool GetDEBUG() const { return drawDebug_; }
    
private:

    //void HandleSceneDrawableUpdateFinished(StringHash eventType, VariantMap& eventData);
    bool Solve(Vector3 targetPos);
    void DefineM(const Vector3 p, const Vector3 d);
    Vector3 Rot(const Matrix3 M, Vector3 src);

    Matrix3 Minv;
    Matrix3 Mfwd;

    bool drawDebug_;

    //String boneName_;//leftFootName
    //String rightFootName;
    //Vector3 axis_;//legAxis
    //float unevenThreshold_; // Set this threshold according to the delta between feet height in idle position/animation
    bool doIK_; // Allow to disable Foot IK, which is only relevant when the character is grounded

    Node* effector_;//leftFoot
    String effectorName_;//stores the name of the node for the editor component stuff
    //Node* rightFoot;
    //Node* rootBone;
    //float length_;//leftLegLength
    //float rightLegLength;
    //float originalRootHeight;
    //Quaternion initialRot_;//leftFootInitialRot
    Vector3 targetPos_;//set with function
    //Quaternion rightFootInitialRot;
    
};
#endif