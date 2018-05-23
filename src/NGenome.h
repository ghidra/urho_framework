#ifndef NGENOME_H
#define NGENOME_H

#include <vector>
//these are all for the weightsstring method
#include <math.h>
#include <string>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <iostream>

#include <Urho3D/Scene/Serializable.h>
#include <Urho3D/IO/Serializer.h>
#include <Urho3D/Resource/XMLFile.h>//for calling the writing out of the nn serilization
#include <Urho3D/Resource/XMLElement.h>

#pragma once

using namespace Urho3D;

class NGenome : public Serializable
{
	URHO3D_OBJECT(NGenome, Serializable);
public:
	//NGenome(Context* context);
	NGenome(Context* context, unsigned id = 0, std::vector<float> w = std::vector<float>{}, float f = 0.0f, unsigned g = 0);
	NGenome(Context* context, const XMLElement& source);//this is called when loading serilaized data from genepool
	static void RegisterObject(Context* context);

	std::vector<float> weights_;
	float fitness_;
	unsigned generation_;

	String DebugString();
	void MarkForRemoval();
	bool MarkedForRemoval(){return markedForRemoval_;};

	bool Save(Serializer& dest) const override;
	bool SaveXML(XMLElement& dest) const override;
	bool LoadXML(const XMLElement& dest) override;
	//overload '<' used for sorting
	//friend bool operator <(const NGenome& lhs, const NGenome& rhs){return (lhs.fitness_ < rhs.fitness_);}
	

private:
	bool markedForRemoval_;
	unsigned id_;//id from genepool counter
	mutable VariantVector weightsVariant_;//for serialization
	
};

#endif