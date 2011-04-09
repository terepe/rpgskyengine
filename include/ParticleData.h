#pragma once
#include "InterfaceModel.h"
#include "Vec2D.h"
#include "Matrix.h"

float	frand();
float	randfloat(float lower, float upper);
int		randint(int lower, int upper);

struct	Particle;
class	CParticleEmitter;
class	CBone;

class CParticleData: public iParticleData
{
public:
	void InitTile(Vec2D *tc, int nID);
	// ¸üÐÂ
	void update(const Matrix& mWorld, CParticleEmitter& particleEmitter, float dt);
	Particle NewPlaneParticle(const Matrix& mWorld, int time, float w, float l, float spd, float var, float spr, float spr2);
	Particle NewSphereParticle(const Matrix& mWorld, int time, float w, float l, float spd, float var, float spr, float spr2);
};