#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>//will not complie without this?

#include "Debug.h"

#include <Urho3D/DebugNew.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Engine/DebugHud.h>

Debug::Debug(Context* context):
    Object(context)
{
}
Debug::~Debug(){}

void Debug::Hud(const String label, const String value)
{
	GetSubsystem<DebugHud>()->SetAppStats(label+" : ", value );
}
void Debug::LogWarning(const String value)
{
	URHO3D_LOGWARNING(value);
}