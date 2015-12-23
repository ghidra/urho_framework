#ifndef SIMPLEXNOISE_H
#define SIMPLEXNOISE_H

#pragma once

#include <Urho3D/Core/Object.h>

using namespace Urho3D;

class SimplexNoise : public Object
{
    URHO3D_OBJECT(SimplexNoise,Object);

public:

    SimplexNoise(Context* context);
    ~SimplexNoise();

    float Noise(const Vector2 pos, const Vector2 scl = Vector2(1.0f,1.0f), const Vector2 offset = Vector2(0.0f,0.0f));
    float Noise(const Vector2 pos, const float scl = 1.0f, const Vector2 offset = Vector2(0.0f,0.0f));
    float Noise(const Vector2 pos, const float scl = 1.0f, const float offset = 0.0f);
    Vector2 Gradient(const Vector2 pos, const float range, const Vector2 scl = Vector2(1.0f,1.0f), const Vector2 offset = Vector2(0.0f,0.0f));
    Vector2 Gradient(const Vector2 pos, const float range, const float scl = 1.0f, const Vector2 offset = Vector2(0.0f,0.0f));
    Vector2 Gradient(const Vector2 pos, const float range, const float scl = 1.0f, const float offset = 0.0f);

    float Noise(const Vector3 pos, const Vector3 scl = Vector3(1.0f,1.0f,1.0f), const Vector3 offset = Vector3(0.0f,0.0f,0.0f));
    float Noise(const Vector3 pos, const float scl = 1.0f, const Vector3 offset = Vector3(0.0f,0.0f,0.0f));
    float Noise(const Vector3 pos, const float scl = 1.0f, const float offset = 0.0f);
    Vector3 Gradient(const Vector3 pos, const float range, const Vector3 scl = Vector3(1.0f,1.0f,1.0f), const Vector3 offset = Vector3(0.0f,0.0f,0.0f));
    Vector3 Gradient(const Vector3 pos, const float range, const float scl = 1.0f, const Vector3 offset = Vector3(0.0f,0.0f,0.0f));
    Vector3 Gradient(const Vector3 pos, const float range, const float scl = 1.0f, const float offset = 0.0f);

protected:

	unsigned permutation_[256];
	unsigned period_;
    
    Vector3 GRAD3_[16];
    //const Vector4 GRAD4_[32];
    //const Vector4 SIMPLEX_[64];

	float F2_;
	float G2_;
	float F3_;
	float G3_;   
};
#endif