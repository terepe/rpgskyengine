#pragma once
#include "Manager.h"
#include "matrix.h"
#include "Color.h"

class CParticleEmitter;

struct Particle
{
	Vec3D vPos, vSpeed, vDown, vOrigin, vDir;
	Matrix mFace;
	//Vec3D tpos;
	float fSize, fLife, fMaxLife;
	int nTile;
	Color32 color;
};

typedef std::list<Particle> ParticleList;

class CParticleGroup
{
public:
	CParticleEmitter* m_pEmitter;	// 粒子发射器指针
	CBone* m_pBone;					// 绑定的骨骼
	ParticleList m_Particles;		// 粒子堆
	uint32 m_uTexID;					// 粒子纹理ID
	int  m_nTime;			// 时间
	float m_fRem;					// 人体伦琴单位当量 辐射量
public:
	void Init(CParticleEmitter* pEmitter, CBone* pBone);
	void update(float fElapsedTime);
	void SetTex(int nTexID);
	void Setup(int nTime);
	//void SetTex(float dt);
	void draw()const;
	CParticleGroup(): m_pEmitter(NULL), m_pBone(NULL), m_uTexID(0)
	{
		m_nTime = 0;
		m_fRem	= 0;
	}
};
