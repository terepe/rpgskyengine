#pragma once

class RibbonEmitter;

#include "InterfaceModel.h"

#include <list>
#include "Animated.h"
#include "Vec2D.h"
#include "Matrix.h"
#include "Material.h"

float frand();
float randfloat(float lower, float upper);
int randint(int lower, int upper);

struct	Particle;
class	CParticleGroup;
class	CBone;
//class ParticleEmitter {
//protected:
//
//public:
//	ParticleEmitter(CParticleGroup *sys): sys(sys) {}
//	virtual Particle newParticle(int anim, int time, float w, float l, float spd, float var, float spr, float spr2) = 0;
//};
//
//class PlaneParticleEmitter: public ParticleEmitter {
//public:
//	PlaneParticleEmitter(CParticleGroup *sys): ParticleEmitter(sys) {}
//	Particle newParticle(int anim, int time, float w, float l, float spd, float var, float spr, float spr2);
//};
//
//class SphereParticleEmitter: public ParticleEmitter {
//public:
//	SphereParticleEmitter(CParticleGroup *sys): ParticleEmitter(sys) {}
//	Particle newParticle(int anim, int time, float w, float l, float spd, float var, float spr, float spr2);
//};


class CParticleEmitter: public iParticleEmitter
{
public:
	void InitTile(Vec2D *tc, int nID);
	// 更新
	void update(const Matrix& mWorld, CParticleGroup& particleGroup, float dt);
	Particle NewPlaneParticle(const Matrix& mWorld, int time, float w, float l, float spd, float var, float spr, float spr2);
	Particle NewSphereParticle(const Matrix& mWorld, int time, float w, float l, float spd, float var, float spr, float spr2);
};

struct RibbonSegment
{
	Vec3D pos, up, back;
	float len,len0;
};

class RibbonEmitter
{
public:
	Animated<Vec3D> color;
	Animated<short> opacity;
	Animated<float> above, below;

	CBone* m_pParentBone;
	float f1, f2;

	Vec3D pos;

	// 动画ID,时间
	int m_nTime;
	float length, seglen;
	int numsegs;

	Vec3D tpos;
	Vec4D tcolor;
	float tabove, tbelow;

	int m_nTexChannel;

	std::list<RibbonSegment> segs;

public:
	void setup(int time);
	void draw();
};