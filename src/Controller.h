#ifndef CONTROLLER_H
#define CONTROLLER_H

#pragma once

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Core/Object.h>

using namespace Urho3D;

namespace Urho3D
{
    class Controls;
    class Engine;
    class Node;
}

const int CTRL_UP = 1;
const int CTRL_DOWN = 2;
const int CTRL_LEFT = 4;
const int CTRL_RIGHT = 8;
const int CTRL_FIRE = 16;
const int CTRL_JUMP = 32;
const int CTRL_ALL = 64;


class Controller : public Object
{
    URHO3D_OBJECT(Controller,Object);

public:
    /// Construct.
    Controller(Context* context);
    ~Controller();

    void HandleUpdate(StringHash eventType, VariantMap& eventData);//this is called from outside this class

    Controls controls_;
    Controls previous_controls_;

    //Node* cameraNode_;
};
#endif