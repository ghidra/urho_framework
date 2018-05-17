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
	NGenome(Context* context, std::vector<float> w, float f = 0.0f, unsigned g = 0);
	//static void RegisterObject(Context* context);

	std::vector<float> weights_;
	float fitness_;
	unsigned generation_;

	String DebugString();
	void MarkForRemoval();
	bool MarkedForRemoval(){return markedForRemoval_;};

	bool Save(Serializer& dest) const override;
	bool SaveXML(XMLElement& dest) const override;
	//overload '<' used for sorting
	//friend bool operator <(const NGenome& lhs, const NGenome& rhs){return (lhs.fitness_ < rhs.fitness_);}
	

private:
	bool markedForRemoval_;

	mutable VariantMap weightsVariant_;//for serialization
	
};

#endif