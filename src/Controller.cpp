#include <Urho3D/Urho3D.h>
#include <Urho3D/Input/Controls.h>

#include "Controller.h"

Controller::Controller(Context* context):
    Object(context)
{
}
Controller::~Controller(){}

void Controller::HandleUpdate()
{
    //using namespace Update;
    controls_.Set(CTRL_UP | CTRL_DOWN | CTRL_LEFT | CTRL_RIGHT | CTRL_FIRE |CTRL_JUMP | CTRL_ALL, false);
}
