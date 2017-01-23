#ifndef IKTARGET_H
#define IKTARGET_H

#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "IK.h"

using namespace Urho3D;

class IKTarget : public LogicComponent
{
    URHO3D_OBJECT(IKTarget,LogicComponent);
    
public:
    IKTarget(Context* context);
    ~IKTarget();

    static void RegisterObject(Context* context);
    void DelayedStart();

    virtual void SetIK(const String parentNode);
    virtual void SetIK(IK* ik);

    //editor functions
    virtual void OnSetAttribute(const AttributeInfo& attr, const Variant& src);
    void SetIKNodeName(const String& IKNodeName);//for setting via gui
    const String& GetIKNodeName() const { return IKNodeName_; };
    
private:

    void HandleSceneDrawableUpdateFinished(StringHash eventType, VariantMap& eventData);

    IK* ik_;
    String IKNodeName_;//stores the name of the node for the editor component stuff
    
};
#endif