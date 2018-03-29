#ifndef NEURALNET_H
#define NEURALNET_H

#pragma once

#include <vector>
#include <math.h>

//  Author: Mat Buckland 2002
//  Desc: Class for creating a feedforward neural net.

using namespace Urho3D;



//-------------------------------------------------------------------
//	define neuron struct
//-------------------------------------------------------------------
class Neuron : public Object
{
	URHO3D_OBJECT(Neuron,Object);
public:
	Neuron(Context* context,unsigned numInputs);
	//~Neuron();
	
	unsigned numInputs_;//the number of inputs into the neuron
	std::vector< float > weights_;//the weights for each input
	//Neuron(Context* context, unsigned NumInputs);//ctor
};

//---------------------------------------------------------------------
//	struct to hold a layer of neurons.
//---------------------------------------------------------------------
class NeuronLayer : public Object
{
	URHO3D_OBJECT(NeuronLayer,Object);
public:
	NeuronLayer(Context* context, unsigned numNeurons, unsigned numInputsPerNeuron);
	unsigned numNeurons_;//the number of neurons in this layer	
	Vector<Neuron*> neurons_;//the layer of neurons
	
};
//----------------------------------------------------------------------
//	neural net class
//----------------------------------------------------------------------

class NeuralNet : public Object
{
	URHO3D_OBJECT(NeuralNet,Object);

public:

	NeuralNet(Context* context,const unsigned numInputs=2,const unsigned numOutputs=1,const unsigned neuronsPerHiddenLyr=4,const unsigned numHiddenLayers=1);

	void CreateNet();

	
	std::vector<float> GetWeights()const;//gets the weights from the NN
	unsigned GetNumberOfWeights()const;//returns total number of weights in net
	unsigned GetNumberOfOutputs()const{return numOutputs_;};
	void PutWeights(std::vector<float> &weights);//replaces the weights with new ones
	std::vector<float> Update(std::vector<float> &inputs);//calculates the outputs from a set of inputs
	inline float Sigmoid(float activation, float response);//sigmoid response curve

private:
	
	unsigned numInputs_;
	unsigned numOutputs_;
	unsigned numHiddenLayers_;
	unsigned neuronsPerHiddenLyr_;

	//storage for each layer of neurons including the output layer
	Vector< NeuronLayer* > layers_;

};
				



#endif