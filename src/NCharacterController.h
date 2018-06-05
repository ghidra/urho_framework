#ifndef NCHARACTERCONTROLLER_H
#define NCHARACTERCONTROLLER_H

#pragma once

#include "Controller.h"

#include "NeuralNet.h"
#include "NGenePool.h"
#include "NGenome.h"

using namespace Urho3D;

namespace Urho3D
{
	class Controls;
	class Engine;
	class Node;
}
class NCharacterController : public Controller
{
	URHO3D_OBJECT(NCharacterController, Controller);

public:

	NCharacterController(Context* context,NGenePool* genepool);
	~NCharacterController();

	virtual void HandleUpdate();
	virtual std::vector<float> HandleUpdate(std::vector<float> in);//this is called from outside this class

	virtual void RefreshGenome();

protected:
	
	SharedPtr<NGenome> genome_;
    SharedPtr<NeuralNet> brain_;
    SharedPtr<NGenePool> genePool_;
};
#endif