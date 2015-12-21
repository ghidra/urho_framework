#ifndef NOISEBASE_H
#define NOISEBASE_H

#pragma once

#include <Urho3D/Core/Object.h>

using namespace Urho3D;

class NoiseBase : public Object
{
    URHO3D_OBJECT(NoiseBase,Object);

public:

    NoiseBase(Context* context);
    ~NoiseBase();

    void Init(unsigned p = 0);
    //void Randomize(const unsigned p);

protected:

	unsigned permutation[512];
	unsigned period;
    
    const Vector3 _GRAD3[16];
    const Vector4 _GRAD4[32];
    const Vector4 _SIMPLEX[64];

	float _F2;
	float _G2;
	float _F3;
	float _G3;   
};
#endif