#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>

#include "State.h"

#include <Urho3D/DebugNew.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Engine/DebugHud.h>

State::State(Context* context):
    Object(context),
    name_(String("unknown"))
{
}
State::~State(){}

State* State::HandleInput(Controls& ctrl)
{
    return NULL;
}
void State::Enter(Pawn* pawn)
{
	pawn_  = pawn;
}
void State::Update(float timeStep)
{
}
void State::Exit()
{
}

float State::Fit(float v, float l1, float h1, float l2=0.0f, float h2=1.0f)
{
    return Clamp( l2 + (v - l1) * (h2 - l2) / (h1 - l1), l2,h2);
}
void State::Debug(const String label, const String value){
    GetSubsystem<DebugHud>()->SetAppStats(label+" : ", value );
}