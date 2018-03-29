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

#pragma once

using namespace Urho3D;

class NGenome : public Object
{
	URHO3D_OBJECT(NGenome,Object);
public:
	NGenome(Context* context, std::vector<float> w, float f = 0.0f, unsigned g = 0);

	std::vector<float> weights_;
	float fitness_;
	unsigned generation_;

	String DebugString();
	void MarkForRemoval();
	bool MarkedForRemoval(){return markedForRemoval_;};
	//overload '<' used for sorting
	//friend bool operator <(const NGenome& lhs, const NGenome& rhs){return (lhs.fitness_ < rhs.fitness_);}
private:
	bool markedForRemoval_;
};

#endif