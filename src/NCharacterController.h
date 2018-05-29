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

	enum NotifyAction
	{
		NA_HIT_ENEMY,
		NA_HIT_FRIENDLY,
		NA_TOOK_DAMAGE
	};

	NCharacterController(Context* context,NGenePool* genepool);
	~NCharacterController();

	virtual void HandleUpdate();
	virtual std::vector<float> HandleUpdate(std::vector<float> in);//this is called from outside this class
	virtual void Notify(enum NotifyAction action, float value);//this is called from whom we are controlling, of what the character accomplished from list


protected:
	
	SharedPtr<NGenome> genome_;
    SharedPtr<NeuralNet> brain_;
    SharedPtr<NGenePool> genePool_;
};
#endif