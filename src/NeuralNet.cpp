#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>

#include "NeuralNet.h"



//*************************** methods for Neuron **********************
//
//---------------------------------------------------------------------
Neuron::Neuron(Context* context, unsigned numInputs):
	Object(context), 
	numInputs_(numInputs+1)										
{
	//we need an additional weight for the bias hence the +1
	for (unsigned i=0; i<numInputs+1; ++i)
		weights_.push_back( Random()-Random() );//set up the weights with an initial random value
		//m_vecWeight.Push(RandomClamped());//set up the weights with an initial random value
}




//************************ methods for NeuronLayer **********************

//-----------------------------------------------------------------------
//	ctor creates a layer of neurons of the required size by calling the 
//	SNeuron ctor the rqd number of times
//-----------------------------------------------------------------------
NeuronLayer::NeuronLayer(Context* context, unsigned numNeurons, unsigned numInputsPerNeuron):
	Object(context),
	numNeurons_(numNeurons)
{
	for (unsigned i=0; i<numNeurons; ++i)
		neurons_.Push(new Neuron(context_,numInputsPerNeuron));
}




//************************ methods forCNeuralNet ************************

//------------------------------default ctor ----------------------------
//
//	creates a ANN based on the default values in params.ini
//-----------------------------------------------------------------------
NeuralNet::NeuralNet(Context* context,const unsigned numInputs,const unsigned numOutputs,const unsigned neuronsPerHiddenLyr,const unsigned numHiddenLayers) :
	Object(context),
	numInputs_(numInputs),
	numOutputs_(numOutputs),
	numHiddenLayers_(numHiddenLayers),
	neuronsPerHiddenLyr_(neuronsPerHiddenLyr)
{
	//m_NumInputs = CParams::iNumInputs;
	//m_NumOutputs = CParams::iNumOutputs;
	//m_NumHiddenLayers =	CParams::iNumHidden;
	//m_NeuronsPerHiddenLyr =	CParams::iNeuronsPerHiddenLayer;

	CreateNet();

}

//------------------------------createNet()------------------------------
//
//	this method builds the ANN. The weights are all initially set to 
//	random values -1 < w < 1
//------------------------------------------------------------------------
void NeuralNet::CreateNet()
{
	//create the layers of the network
	if (numHiddenLayers_ > 0)
	{
		//create first hidden layer
	  	layers_.Push(new NeuronLayer(context_, neuronsPerHiddenLyr_, numInputs_));
    
    	for (unsigned i=0; i<numHiddenLayers_-1; ++i)
    	{
			layers_.Push(new NeuronLayer(context_, neuronsPerHiddenLyr_, neuronsPerHiddenLyr_));
    	}
    	//create output layer
	  	layers_.Push(new NeuronLayer(context_, numOutputs_, neuronsPerHiddenLyr_));
	}
  	else
  	{
	  	//create output layer
		layers_.Push(new NeuronLayer(context_, numOutputs_, numInputs_));
  	}
}

//---------------------------------GetWeights-----------------------------
//
//	returns a vector containing the weights
//
//------------------------------------------------------------------------
std::vector<float> NeuralNet::GetWeights() const
{
	//this will hold the weights
	std::vector<float> weights;
	
	//for each layer
	for (unsigned i=0; i<numHiddenLayers_ + 1; ++i)
	{

		//for each neuron
		for (unsigned j=0; j<layers_[i]->numNeurons_; ++j)
		{
			//for each weight
			for (unsigned k=0; k<layers_[i]->neurons_[j]->numInputs_; ++k)
			{
				weights.push_back(layers_[i]->neurons_[j]->weights_[k]);
			}
		}
	}

	return weights;
}

//-----------------------------------PutWeights---------------------------
//
//	given a vector of doubles this function replaces the weights in the NN
//  with the new values
//
//------------------------------------------------------------------------
void NeuralNet::PutWeights(std::vector<float> &weights)
{
	unsigned cWeight = 0;
	
	//for each layer
	for (unsigned i=0; i<numHiddenLayers_ + 1; ++i)
	{

		//for each neuron
		for (unsigned j=0; j<layers_[i]->numNeurons_; ++j)
		{
			//for each weight
			for (unsigned k=0; k<layers_[i]->neurons_[j]->numInputs_; ++k)
			{
				layers_[i]->neurons_[j]->weights_[k] = weights[cWeight++];
			}
		}
	}

	return;
}

//---------------------------------GetNumberOfWeights---------------------
//
//	returns the total number of weights needed for the net
//
//------------------------------------------------------------------------
unsigned NeuralNet::GetNumberOfWeights() const
{

	unsigned weights = 0;
	
	//for each layer
	for (unsigned i=0; i<numHiddenLayers_ + 1; ++i)
	{

		//for each neuron
		for (unsigned j=0; j<layers_[i]->numNeurons_; ++j)
		{
			//for each weight
			for (unsigned k=0; k<layers_[i]->neurons_[j]->numInputs_; ++k)
			
				weights++;
			
		}
	}

	return weights;
}

//-------------------------------Update-----------------------------------
//
//	given an input vector this function calculates the output vector
//
//------------------------------------------------------------------------
std::vector<float> NeuralNet::Update(std::vector<float> &inputs)
{
	//stores the resultant outputs from each layer
	std::vector<float> outputs;

	unsigned cWeight = 0;
	
	//first check that we have the correct amount of inputs
	if (inputs.size() != numInputs_)
  	{
		//just return an empty vector if incorrect.
		return outputs;
  	}
	
	//For each layer....
	for (unsigned i=0; i<numHiddenLayers_ + 1; ++i)
	{		
		if ( i > 0 )
    {
			inputs = outputs;
    }

		outputs.clear();
		
		cWeight = 0;

		//for each neuron sum the (inputs * corresponding weights).Throw 
		//the total at our sigmoid function to get the output.
		for (unsigned j=0; j<layers_[i]->numNeurons_; ++j)
		{
			float netinput = 0;

			unsigned	NumInputs = layers_[i]->neurons_[j]->numInputs_;
			
			//for each weight
			for (unsigned k=0; k<NumInputs - 1; ++k)
			{
				//sum the weights x inputs
				netinput += layers_[i]->neurons_[j]->weights_[k] * inputs[cWeight++];
			}

			//add in the bias
			netinput += layers_[i]->neurons_[j]->weights_[NumInputs-1] * 0.5;//CParams::dBias;

			//we can store the outputs from each layer as we generate them. 
      		//The combined activation is first filtered through the sigmoid 
      		//function
			//outputs.Push(Sigmoid(netinput, CParams::dActivationResponse));
			outputs.push_back(Sigmoid(netinput, 0.5));

			cWeight = 0;
		}
	}

	return outputs;
}

//-------------------------------Sigmoid function-------------------------
//
//------------------------------------------------------------------------
float NeuralNet::Sigmoid(float netinput, float response)
{
	return ( 1 / ( 1 + exp(-netinput / response)));
}

