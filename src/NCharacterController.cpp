#include <Urho3D/Urho3D.h>
#include <Urho3D/Input/Controls.h>

#include "NCharacterController.h"

NCharacterController::NCharacterController(Context* context, NGenePool* genepool)
	:Controller(context)
	,genePool_(genepool)
{
	brain_ = new NeuralNet(context_,10,6,8);
    genePool_->GiveNumberOfWeights(brain_->GetNumberOfWeights());
    genome_ = genePool_->GetSpecimen();
    brain_->PutWeights(genome_->weights_);
}
NCharacterController::~NCharacterController() {}

void NCharacterController::HandleUpdate()
{
	Controller::HandleUpdate();
}
std::vector<float> NCharacterController::HandleUpdate(std::vector<float> in)
{
	HandleUpdate();
	return brain_->Update(in);// std::vector<float>();
}
void NCharacterController::Notify(enum NotifyAction action, float value)
{
	//we can do what we want with this information
}