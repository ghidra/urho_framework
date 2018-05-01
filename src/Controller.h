#ifndef CONTROLLER_H
#define CONTROLLER_H

#pragma once

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Core/Object.h>
#include <vector>

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

    virtual void HandleUpdate();//this is called from outside this class
	virtual std::vector<float> HandleUpdate(std::vector<float> in);//this is a mock version of incmoing data for nural net processing

    Controls controls_;

};
#endif