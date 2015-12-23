#include <Urho3D/Urho3D.h>
#include "SimplexNoise.h"

#include <Urho3D/DebugNew.h>
#include <Urho3D/IO/Log.h>

SimplexNoise::SimplexNoise(Context* context) :
	Object(context),
    period_(256)
{
	const unsigned plist[256] = 
	{
    	151,160,137,91,90,15, 
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23, 
		190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33, 
		88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,166, 
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244, 
		102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196, 
		135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123, 
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42, 
		223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9, 
		129,22,39,253,9,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228, 
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107, 
		49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254, 
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};
	//hack assign the goddamned values to the array
	for (unsigned i = 0; i<256; ++i)
	{
		permutation_[i]=plist[i];
	}

	const Vector3 glist[16] = 
	{
		Vector3(1,1,0),Vector3(-1,1,0),Vector3(1,-1,0),Vector3(-1,-1,0), 
		Vector3(1,0,1),Vector3(-1,0,1),Vector3(1,0,-1),Vector3(-1,0,-1), 
		Vector3(0,1,1),Vector3(0,-1,1),Vector3(0,1,-1),Vector3(0,-1,-1),
		Vector3(1,1,0),Vector3(0,-1,1),Vector3(-1,1,0),Vector3(0,-1,-1)
	};

	for (unsigned i = 0; i<16; ++i)
	{
		GRAD3_[i]=glist[i];
	}

	/*const Vector4 GRAD4_[32] = 
	{
		Vector4(0,1,1,1), Vector4(0,1,1,-1), Vector4(0,1,-1,1), Vector4(0,1,-1,-1), 
		Vector4(0,-1,1,1), Vector4(0,-1,1,-1), Vector4(0,-1,-1,1), Vector4(0,-1,-1,-1), 
		Vector4(1,0,1,1), Vector4(1,0,1,-1), Vector4(1,0,-1,1), Vector4(1,0,-1,-1), 
		Vector4(-1,0,1,1), Vector4(-1,0,1,-1), Vector4(-1,0,-1,1), Vector4(-1,0,-1,-1), 
		Vector4(1,1,0,1), Vector4(1,1,0,-1), Vector4(1,-1,0,1), Vector4(1,-1,0,-1), 
		Vector4(-1,1,0,1), Vector4(-1,1,0,-1), Vector4(-1,-1,0,1), Vector4(-1,-1,0,-1), 
		Vector4(1,1,1,0), Vector4(1,1,-1,0), Vector4(1,-1,1,0), Vector4(1,-1,-1,0), 
		Vector4(-1,1,1,0), Vector4(-1,1,-1,0), Vector4(-1,-1,1,0), Vector4(-1,-1,-1,0)
	};

	const Vector4 SIMPLEX_[64] = 
	{
		Vector4(0,1,2,3),Vector4(0,1,3,2),Vector4(0,0,0,0),Vector4(0,2,3,1),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(1,2,3,0), 
		Vector4(0,2,1,3),Vector4(0,0,0,0),Vector4(0,3,1,2),Vector4(0,3,2,1),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(1,3,2,0), 
		Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(0,0,0,0), 
		Vector4(1,2,0,3),Vector4(0,0,0,0),Vector4(1,3,0,2),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(2,3,0,1),Vector4(2,3,1,0), 
		Vector4(1,0,2,3),Vector4(1,0,3,2),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(2,0,3,1),Vector4(0,0,0,0),Vector4(2,1,3,0), 
		Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(0,0,0,0), 
		Vector4(2,0,1,3),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(3,0,1,2),Vector4(3,0,2,1),Vector4(0,0,0,0),Vector4(3,1,2,0), 
		Vector4(2,1,0,3),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(0,0,0,0),Vector4(3,1,0,2),Vector4(0,0,0,0),Vector4(3,2,0,1),Vector4(3,2,1,0)
	};*/

	F2_ = 0.5 * (sqrtf(3.0) - 1.0);
	G2_ = (3.0 - sqrtf(3.0)) / 6.0;
	F3_ = 1.0 / 3.0;
	G3_ = 1.0 / 6.0;
}

SimplexNoise::~SimplexNoise(){}

float SimplexNoise::Noise(const Vector2 pos, const Vector2 scl, const Vector2 offset)
{
	float ax=(pos.x_+offset.x_)*scl.x_;
	float ay=(pos.y_+offset.y_)*scl.y_;

	float s = (ax + ay) * F2_;
	float i = floor(ax + s);
	float j = floor(ay + s);
	float t = (i + j) * G2_;
	float x0 = ax - (i - t);
	float y0 = ay - (j - t);

	unsigned  i1,j1;
	if (x0 > y0)
	{
		i1 = 1; 
		j1 = 0;
	}
	else
	{
		i1 = 0;
		j1 = 1;
	}
	
	float x1 = x0 - float(i1) + G2_;
	float y1 = y0 - float(j1) + G2_;
	float x2 = x0 + G2_ * 2.0f - 1.0f;
	float y2 = y0 + G2_ * 2.0f - 1.0f;

	unsigned int ii = unsigned(i) % period_;
	unsigned int jj = unsigned(j) % period_;
	unsigned gi0 = permutation_[ii + permutation_[jj]] % 12;
	unsigned gi1 = permutation_[ii + i1 + permutation_[jj + j1]] % 12;
	unsigned gi2 = permutation_[ii + 1 + permutation_[jj + 1]] % 12;
	
	//URHO3D_LOGWARNING("ii = "+String(unsigned(i))+"%"+String(period_)+"="+String(ii));
	//URHO3D_LOGWARNING("jj = "+String(unsigned(j))+"%"+String(period_)+"="+String(jj));
	//URHO3D_LOGWARNING(String( permutation_[jj] ) );
	//URHO3D_LOGWARNING(String(gi0));
	//URHO3D_LOGWARNING(String(gi1));
	//URHO3D_LOGWARNING(String(gi2));

	float noise = 0.0f;
	float tt = 0.5f - (x0*x0) - (y0*y0);
	if (tt > 0.0f)
	{
		Vector3 g = GRAD3_[gi0];
		noise = pow(tt,4.0f) * (g.x_ * x0 + g.y_ * y0);
	}
	else
	{
		noise = 0.0f;
	}
	
	tt = 0.5f - (x1*x1) - (y1*y1);
	if (tt > 0.0f)
	{
		Vector3 g = GRAD3_[gi1];
		noise += pow(tt,4.0f) * (g.x_ * x1 + g.y_ * y1);
	}
	tt = 0.5f - (x2*x2) - (y2*y2);
	if (tt > 0.0f)
	{
		Vector3 g = GRAD3_[gi2];
		noise += pow(tt,4.0f) * (g.x_ * x2 + g.y_ * y2);
	}


	return noise * 70.0f; //scale noise to [-1, 1]
	//return 2.0f;
}
float SimplexNoise::Noise(const Vector2 pos, const float scl, const Vector2 offset)
{
	return Noise(pos,Vector2(scl,scl),offset);
}
float SimplexNoise::Noise(const Vector2 pos, const float scl, const float offset)
{
	return Noise(pos,Vector2(scl,scl),Vector3(offset,offset));
}
Vector2 SimplexNoise::Gradient(const Vector2 pos, const float range, const Vector2 scl, const Vector2 offset)
{
    float nx = Noise(pos,scl,offset);
    float ny = Noise(pos,scl,offset+Vector2(0.0f,range));
    return Vector2(nx,ny);
}
Vector2 SimplexNoise::Gradient(const Vector2 pos, const float range, const float scl, const Vector2 offset)
{
    return Gradient(pos,range,Vector2(scl,scl),offset);
}
Vector2 SimplexNoise::Gradient(const Vector2 pos, const float range, const float scl, const float offset)
{
    return Gradient(pos,range,Vector2(scl,scl),Vector2(offset,offset));
}

//-----------------

float SimplexNoise::Noise(const Vector3 pos, const Vector3 scl, const Vector3 offset)
{
	float ax=(pos.x_+offset.x_)*scl.x_;
	float ay=(pos.y_+offset.y_)*scl.y_;
	float az=(pos.z_+offset.z_)*scl.z_;

	float s = (ax + ay + az) * F3_;
	float i = floor(ax + s);
	float j = floor(ay + s);
	float k = floor(az + s);
	float t = (i + j + k) * G3_;
	float x0 = ax - (i - t);
	float y0 = ay - (j - t);
	float z0 = az - (k - t);
 
 	unsigned i1,j1,k1,i2,j2,k2;
	if (x0 >= y0)
	{
		if (y0 >= z0)
		{
			i1 = 1; 
			j1 = 0; 
			k1 = 0;
			i2 = 1; 
			j2 = 1; 
			k2 = 0;
		}
		else if (x0 >= z0)
		{
			i1 = 1; 
			j1 = 0; 
			k1 = 0;
			i2 = 1; 
			j2 = 0; 
			k2 = 1;
		}
		else
		{
			i1 = 0;
			j1 = 0; 
			k1 = 1;
			i2 = 1; 
			j2 = 0; 
			k2 = 1;
		}
	}
	else
	{
		if (y0 < z0)
		{
			i1 = 0; 
			j1 = 0; 
			k1 = 1;
			i2 = 0; 
			j2 = 1; 
			k2 = 1;
		}
		else if (x0 < z0)
		{
			i1 = 0; 
			j1 = 1; 
			k1 = 0;
			i2 = 0; 
			j2 = 1; 
			k2 = 1;
		}
		else
		{
			i1 = 0; 
			j1 = 1; 
			k1 = 0;
			i2 = 1; 
			j2 = 1; 
			k2 = 0;
		}
	}
	
	float x1 = x0 - float(i1) + G3_;
	float y1 = y0 - float(j1) + G3_;
	float z1 = z0 - float(k1) + G3_;
	float x2 = x0 - float(i2) + 2.0f * G3_;
	float y2 = y0 - float(j2) + 2.0f * G3_;
	float z2 = z0 - float(k2) + 2.0f * G3_;
	float x3 = x0 - 1.0f + 3.0f * G3_;
	float y3 = y0 - 1.0f + 3.0f * G3_;
	float z3 = z0 - 1.0f + 3.0f * G3_;

	unsigned ii = unsigned(i) % period_;
	unsigned jj = unsigned(j) % period_;
	unsigned kk = unsigned(k) % period_;
	unsigned gi0 = permutation_[ii + permutation_[jj + permutation_[kk]]] % 12;
	unsigned gi1 = permutation_[ii + i1 + permutation_[jj + j1 + permutation_[kk + k1]]] % 12;
	unsigned gi2 = permutation_[ii + i2 + permutation_[jj + j2 + permutation_[kk + k2]]] % 12;
	unsigned gi3 = permutation_[ii + 1 + permutation_[jj + 1 + permutation_[kk + 1]]] % 12;

	float noise = 0.0f;
	float tt = 0.6 - (x0*x0) - (y0*y0) - (z0*z0);
	if (tt > 0.0f)
	{
		Vector3 g = GRAD3_[gi0];
		noise = pow(tt,4.0f) * (g.x_ * x0 + g.y_ * y0 + g.z_ * z0);
	}
	else
	{
		noise = 0.0f;
	}
	
	tt = 0.6 - (x1*x1) - (y1*y1) - (z1*z1);
	if (tt > 0)
	{
		Vector3 g = GRAD3_[gi1];
		noise += pow(tt,4.0f) * (g.x_ * x1 + g.y_ * y1 + g.z_ * z1);
	}
	
	tt = 0.6 - (x2*x2) - (y2*y2) - (z2*z2);
	if (tt > 0)
	{
		Vector3 g = GRAD3_[gi2];
		noise += pow(tt,4.0f) * (g.x_ * x2 + g.y_ * y2 + g.z_ * z2);
	}
	tt = 0.6 - (x3*x3) - (y3*y3) - (z3*z3);
	if (tt > 0)
	{
		Vector3 g = GRAD3_[gi3];
		noise += pow(tt,4.0f) * (g.x_ * x3 + g.y_ * y3 + g.z_ * z3);
	}
	
	return noise * 32.0f;
}
float SimplexNoise::Noise(const Vector3 pos, const float scl, const Vector3 offset)
{
	return Noise(pos,Vector3(scl,scl),offset);
}
float SimplexNoise::Noise(const Vector3 pos, const float scl, const float offset)
{
	return Noise(pos,Vector3(scl,scl,scl),Vector3(offset,offset,scl));
}
Vector3 SimplexNoise::Gradient(const Vector3 pos, const float range, const Vector3 scl, const Vector3 offset)
{
    float nx = Noise(pos,scl,offset);
    float ny = Noise(pos,scl,offset+Vector3(0.0f,range,0.0f));
    float nz = Noise(pos,scl,offset+Vector3(0.0f,0.0f,range));
    return Vector3(nx,ny,nz);
}
Vector3 SimplexNoise::Gradient(const Vector3 pos, const float range, const float scl, const Vector3 offset)
{
    return Gradient(pos,range,Vector3(scl,scl,scl),offset);
}
Vector3 SimplexNoise::Gradient(const Vector3 pos, const float range, const float scl, const float offset)
{
    return Gradient(pos,range,Vector3(scl,scl,scl),Vector3(offset,offset,offset));
}