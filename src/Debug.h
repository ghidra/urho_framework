#ifndef DEBUG_H
#define DEBUG_H

#pragma once

#include <Urho3D/Core/Object.h>
#include <Urho3D/Engine/DebugHud.h>

using namespace Urho3D;

class Debug : public Object
{
	//this class exists, because i'm too lazyto deal with the includes on every file
    URHO3D_OBJECT(Debug,Object);
    
public:
    Debug(Context* context);
    ~Debug();

    void Hud(const String label, const String value);
    void LogWarning(const String value);
};
#define dbg(LABEL, STRING) context_->GetSubsystem<DebugHud>()->SetAppStats(LABEL,STRING)
#endif