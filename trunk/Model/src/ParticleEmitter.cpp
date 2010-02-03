#include "ParticleEmitter.h"
#include "RenderSystem.h"
#include "Graphics.h"

#include "Particle.h"
#include "ModelData.h"

// 零时加的????上大三大四
//#include "Sound.h"

#define MAX_PARTICLES 10000


float frand()
{
	return rand()/(float)RAND_MAX;
}

float randfloat(float lower, float upper)
{
	return lower + (upper-lower)*(rand()/(float)RAND_MAX);
}

int randint(int lower, int upper)
{
	return lower + (int)((upper-lower)*frand());
}

Vec4D fromARGB(uint32 color)
{
	const float a = ((color & 0xFF000000) >> 24) / 255.0f;
	const float r = ((color & 0x00FF0000) >> 16) / 255.0f;
	const float g = ((color & 0x0000FF00) >>  8) / 255.0f;
	const float b = ((color & 0x000000FF)) / 255.0f;
    return Vec4D(r,g,b,a);
}


// 基于“伸展”产生旋转的矩阵
Matrix SpreadMat;
void CalcSpreadMatrix(float Spread1,float Spread2, float w, float l)
{
	int i;
	float a[2],c[2],s[2];
	Matrix	Temp;

	SpreadMat=Matrix::UNIT;

	a[0]=randfloat(-Spread1,Spread1)/2.0f;
	a[1]=randfloat(-Spread2,Spread2)/2.0f;

	SpreadMat.m[0][0]*=l;
	SpreadMat.m[1][1]*=l;
	SpreadMat.m[2][2]*=w;

	for(i=0;i<2;i++)
	{		
		c[i]=cos(a[i]);
		s[i]=sin(a[i]);
	}
	Temp=Matrix::UNIT;
	Temp.m[1][1]=c[0];
	Temp.m[2][1]=s[0];
	Temp.m[2][2]=c[0];
	Temp.m[1][2]=-s[0];

	SpreadMat*=Temp;

	Temp=Matrix::UNIT;
	Temp.m[0][0]=c[1];
	Temp.m[1][0]=s[1];
	Temp.m[1][1]=c[1];
	Temp.m[0][1]=-s[1];

	SpreadMat*=Temp;

}

void CParticleEmitter::InitTile(Vec2D *tc, int nID)
{
	Vec2D otc[4];
	Vec2D a,b;
	int x = nID % m_nCols;
	int y = nID / m_nCols;
	a.x = x * (1.0f / m_nCols);
	b.x = (x+1) * (1.0f / m_nCols);
	a.y = y * (1.0f / m_nRows);
	b.y = (y+1) * (1.0f / m_nRows);

	otc[0] = a;
	otc[1].x = b.x;otc[1].y = a.y;
	otc[2] = b;
	otc[3].x = a.x;otc[3].y = b.y;

	for (int i=0; i<4; i++)
	{// 有必要这样以来吗？m_nOrder？
		tc[(i+4-m_nOrder) & 3] = otc[i];
	}
}

Particle CParticleEmitter::NewPlaneParticle(const Matrix& mWorld, int time, float w, float l, float spd, float var, float spr, float spr2)
{
	Particle p;

	//Spread Calculation
	Matrix mWorldRot = mWorld;
	mWorldRot._14=0;
	mWorldRot._24=0;
	mWorldRot._34=0;

	CalcSpreadMatrix(spr,spr,1.0f,1.0f);
	Matrix mRot = mWorldRot * SpreadMat;

	// Flags
	// 1041 = Halo
	// 49 = particle moving up?
	// 29 = particle being static
	// 25 = flame on weapon - move up/along the weapon
	// 17 = glow on weapon - static, random direction
	// 17 = aurastone - ?
	// 1 = perdition blade
	if (flags == 1041) // Trans Halo 透明光晕
	{
		p.vPos =mWorld * (m_vPos + Vec3D(randfloat(-l,l), 0, randfloat(-w,w)));

		float t = randfloat(0,2*PI);

		p.vPos = Vec3D(0.0f, m_vPos.y + 0.15f, m_vPos.z) + Vec3D(cos(t)/8, 0.0f, sin(t)/8); // Need to manually correct for the halo - why?

		// var isn't being used, which is set to 1.0f,  whats the importance of this?
		// why does this set of values differ from other particles

		Vec3D vDir(0.0f, 1.0f, 0.0f);

		p.vSpeed = vDir.normalize() * spd * randfloat(0, var);
	}
	/*else if (flags == 25 && pBone->m_pBoneAnim->parent<1)// Weapon Flame 武器光芒
	{
		p.vPos = pBone->m_vTransPivot * (m_vPos + Vec3D(randfloat(-l,l), randfloat(-l,l), randfloat(-w,w)));
		Vec3D vDir = mRot * Vec3D(0.0f, 1.0f, 0.0f);
		//Vec3D vDir = sys->model->bones[sys->pBone->parent].mRot * sys->mWorldRot * Vec3D(0.0f, 1.0f, 0.0f);
		//p.speed = vDir.normalize() * spd;

	}
	else if (flags == 25 && pBone->m_pBoneAnim->parent > 0) // Weapon with built-in Flame (Avenger lightsaber!) 武器内嵌光芒
	{
		p.vPos =mWorld * (m_vPos + Vec3D(randfloat(-l,l), randfloat(-l,l), randfloat(-w,w)));
		Vec3D vDir = Vec3D(mWorld.m[1][0],mWorld.m[1][1], mWorld.m[1][2]) * Vec3D(0.0f, 1.0f, 0.0f);
		p.vSpeed = vDir.normalize() * spd * randfloat(0, var*2);

	}
	else if (flags == 17 && pBone->m_pBoneAnim->parent<1) // Weapon Glow 武器发光
	{
		p.vPos = pBone->m_vTransPivot * (m_vPos + Vec3D(randfloat(-l,l), randfloat(-l,l), randfloat(-w,w)));

	}*/
	else
	{
		p.vPos = m_vPos + Vec3D(randfloat(-l,l), 0, randfloat(-w,w));
		p.vPos = mWorld * p.vPos;

		Vec3D vDir = mRot * Vec3D(0,1,0);
		p.vDown = Vec3D(0,-1.0f,0); // vDir * -1.0f;
		p.vSpeed = vDir.normalize() * spd * (1.0f+randfloat(-var,var));
	}

	if(!m_bBillboard)
	{
		Vec3D look = p.vDir;
		look.x=p.vDir.y;
		look.y=p.vDir.z;
		look.z=p.vDir.x;

		Vec3D up = (look % p.vDir).normalize();
		Vec3D right = (up % p.vDir).normalize();
		up = (p.vDir % right).normalize();

		// calculate the billboard matrix
		p.mFace.m[0][1] = right.x;
		p.mFace.m[1][1] = right.y;
		p.mFace.m[2][1] = right.z;
		p.mFace.m[0][2] = up.x;
		p.mFace.m[1][2] = up.y;
		p.mFace.m[2][2] = up.z;
		p.mFace.m[0][0] = p.vDir.x;
		p.mFace.m[1][0] = p.vDir.y;
		p.mFace.m[2][0] = p.vDir.z;
	}

	p.fLife = 0;
	p.fMaxLife = m_Lifespan.getValue(time);

	p.vOrigin = p.vPos;

	p.nTile = randint(0, m_nRows*m_nCols-1);
	return p;
}

Particle CParticleEmitter::NewSphereParticle(const Matrix& mWorld, int time, float w, float l, float spd, float var, float spr, float spr2)
{
	Particle p;
	Vec3D vDir;
	float radius;

	radius = randfloat(0,1);

	// Old method
	//float t = randfloat(0,2*PI);

	// New
	// Spread should never be zero for sphere particles ?
	float t;
	if (spr == 0)
		t = randfloat(-PI,PI);
	else
		t = randfloat(-spr,spr);

	//Spread Calculation
	Matrix mWorldRot = mWorld;
	mWorldRot._14=0;
	mWorldRot._24=0;
	mWorldRot._34=0;

	CalcSpreadMatrix(spr*2,spr2*2,w,l);

	Matrix mRot=mWorldRot*SpreadMat;

	// New
	// Length should never technically be zero ?
	//if (l==0)
	//	l = w;

	// New method
	// Vec3D bdir(w*cosf(t), 0.0f, l*sinf(t));
	// --

	// TODO: fix shpere emitters to work properly
	/* // Old Method
	//Vec3D bdir(l*cosf(t), 0, w*sinf(t));
	//Vec3D bdir(0, w*cosf(t), l*sinf(t));


	float theta_range = sys->m_Spread.getValue(anim, time);
	float theta = -0.5f* theta_range + randfloat(0, theta_range);
	Vec3D bdir(0, l*cosf(theta), w*sinf(theta));

	float phi_range = sys->m_Lat.getValue(anim, time);
	float phi = randfloat(0, phi_range);
	rotate(0,0, &bdir.z, &bdir.x, phi);
	*/

	if (flags == 57 || flags == 313) { // Faith Halo
		Vec3D bdir(w*cosf(t)*1.6f, 0.0f, l*sinf(t)*1.6f);

		p.vPos = m_vPos + bdir;
		p.vPos = mWorld * p.vPos;

		if (bdir.lengthSquared()==0) 
			p.vSpeed = Vec3D(0.0f,0.0f,0.0f);
		else {
			vDir = mWorldRot * (bdir.normalize());//mRot * Vec3D(0, 1.0f,0);
			p.vSpeed = vDir.normalize() * spd * (1.0f+randfloat(-var,var));   // ?
		}

	} else {
		Vec3D bdir;
		float temp;

		bdir = mRot * Vec3D(0,1,0) * radius;
		temp = bdir.z;
		bdir.z = bdir.y;
		bdir.y = temp;

		p.vPos =mWorld * m_vPos + bdir;


		//p.vPos = sys->m_vPos + bdir;
		//p.vPos = sys->mWorld * p.vPos;


		if ((bdir.lengthSquared()==0) && ((flags&0x100)!=0x100))
		{
			p.vSpeed = Vec3D(0.0f,0.0f,0.0f);
			vDir = mWorldRot * Vec3D(0,1,0);
		}
		else {
			if(flags&0x100)
				vDir = mWorldRot * Vec3D(0,1,0);
			else
				vDir = bdir.normalize();

			p.vSpeed = vDir.normalize() * spd * (1.0f+randfloat(-var,var));   // ?
		}
	}

	p.vDir =  vDir.normalize();//mRot * Vec3D(0, 1.0f,0);
	p.vDown = Vec3D(0,-1.0f,0);

	p.fLife = 0;
	p.fMaxLife = m_Lifespan.getValue(time);
	p.vOrigin = p.vPos;
	p.nTile = randint(0, m_nRows*m_nCols-1);
	return p;
}

void CParticleEmitter::update(const Matrix& mWorld, CParticleGroup& particleGroup, float fElapsedTime)
{
	int& nTime = particleGroup.m_nTime;
	ParticleList& Particles = particleGroup.m_Particles;

	// spawn new particles
	if (pType==1||pType==2)
	{
		float fRate = m_Rate.getValue(nTime);
		float fToSpawn;
		{
			float fLife = 1.0f;
			//fLife = m_Lifespan.getValue(m_nTime);
			fToSpawn = (fElapsedTime * fRate / fLife) + particleGroup.m_fRem;
		}

		if (fToSpawn < 1.0f)// 时间不足于产生一个粒子
		{
			particleGroup.m_fRem = fToSpawn;
			if (particleGroup.m_fRem<0) 
				particleGroup.m_fRem = 0;
		}
		else
		{
			int nToSpawn = (int)fToSpawn;

			// 检测粒子是否超量
			if ((nToSpawn + Particles.size()) > MAX_PARTICLES)
			{
				nToSpawn = (int)Particles.size() - MAX_PARTICLES;
			}

			particleGroup.m_fRem = fToSpawn - (float)nToSpawn;

			bool bEnabled = m_Enabled.getValue(nTime)!=0;
			if (bEnabled)
			{
				float w = m_Areal.getValue(nTime) * 0.5f;
				float l = m_Areaw.getValue(nTime) * 0.5f;
				float spd = m_Speed.getValue(nTime);
				float var = m_Variation.getValue(nTime);
				float spr = m_Spread.getValue(nTime);
				float spr2 =m_Lat.getValue(nTime);

				for (int i=0; i<nToSpawn; i++)
				{
					if (pType==1)
					{
						Particle p = NewPlaneParticle(mWorld, nTime, w, l, spd, var, spr, spr2);
						Particles.push_back(p);
					}
					else if (pType==2)
					{
						Particle p = NewSphereParticle(mWorld, nTime, w, l, spd, var, spr, spr2);
						Particles.push_back(p);
					}

				}
			}
		}
	}
	float mspeed = 1.0f;
	float fGrav = m_Gravity.getValue(nTime);
	float fDeaccel = m_Deacceleration.getValue(nTime);
	for (ParticleList::iterator it = Particles.begin(); it != Particles.end();)
	{
	Particle &p = *it;
	// 计算出当前速度
	p.vSpeed += (p.vDown * fGrav  - p.vDir * fDeaccel) * fElapsedTime;

	if (m_fSlowdown>0)// ????????????????
	{
	mspeed = expf(-1.0f * m_fSlowdown * p.fLife);
	}
	p.vPos += p.vSpeed * mspeed * fElapsedTime;

	p.fLife += fElapsedTime;
	float rlife = p.fLife / p.fMaxLife;
	// 通过当前生命时间计算出当前粒子大小和颜色
	p.fSize = lifeRamp<float>(rlife, m_fLifeMid, m_Sizes[0], m_Sizes[1], m_Sizes[2]);
	p.color = lifeRamp<Color32>(rlife, m_fLifeMid, m_Colors[0], m_Colors[1], m_Colors[2]);

	// 杀死过期粒子
	if (rlife >= 1.0f) 
	{
	Particles.erase(it++);
	}
	else 
	{
	++it;
	}
	}
}

//Particle PlaneParticleEmitter::newParticle(int anim, int time, float w, float l, float spd, float var, float spr, float spr2)
//{
//    Particle p;
//
//	//Spread Calculation
//	Matrix mRot;
//
//	CalcSpreadMatrix(spr,spr,1.0f,1.0f);
//	mRot=sys->mWorldRot*SpreadMat;
//	
//	
//
//	// Flags
//	// 1041 = Halo
//	// 49 = particle moving up?
//	// 29 = particle being static
//	// 25 = flame on weapon - move up/along the weapon
//	// 17 = glow on weapon - static, random direction
//	// 17 = aurastone - ?
//	// 1 = perdition blade
//	if (sys->flags == 1041) { // Trans Halo
//		p.pos = sys->m_pParentBone->mat * (sys->m_vPos + Vec3D(randfloat(-l,l), 0, randfloat(-w,w)));
//
//		float t = randfloat(0,2*PI);
//
//		p.pos = Vec3D(0.0f, sys->pos.y + 0.15f, sys->pos.z) + Vec3D(cos(t)/8, 0.0f, sin(t)/8); // Need to manually correct for the halo - why?
//		
//		// var isn't being used, which is set to 1.0f,  whats the importance of this?
//		// why does this set of values differ from other particles
//
//		Vec3D dir(0.0f, 1.0f, 0.0f);
//
//		p.speed = dir.normalize() * spd * randfloat(0, var);
//	} else if (sys->flags == 25 && sys->m_pParentBone->parent<1) { // Weapon Flame
//		p.pos = sys->m_pParentBone->pivot * (sys->pos + Vec3D(randfloat(-l,l), randfloat(-l,l), randfloat(-w,w)));
//		Vec3D dir = mRot * Vec3D(0.0f, 1.0f, 0.0f);
//		//Vec3D dir = sys->model->bones[sys->m_pBoneParent->parent].mRot * sys->m_pBoneParent->mRot * Vec3D(0.0f, 1.0f, 0.0f);
//		//p.speed = dir.normalize() * spd;
//
//	} else if (sys->flags == 25 && sys->m_pBoneParent->parent > 0) { // Weapon with built-in Flame (Avenger lightsaber!)
//		p.pos = sys->m_pBoneParent->mat * (sys->pos + Vec3D(randfloat(-l,l), randfloat(-l,l), randfloat(-w,w)));
//		Vec3D dir = Vec3D(sys->m_pBoneParent->mat.m[1][0],sys->m_pBoneParent->mat.m[1][1], sys->m_pBoneParent->mat.m[1][2]) * Vec3D(0.0f, 1.0f, 0.0f);
//		p.speed = dir.normalize() * spd * randfloat(0, var*2);
//
//	} else if (sys->flags == 17 && sys->m_pBoneParent->parent<1) { // Weapon Glow
//		p.pos = sys->m_pBoneParent->pivot * (sys->pos + Vec3D(randfloat(-l,l), randfloat(-l,l), randfloat(-w,w)));
//		
//	} else {
//		p.pos = sys->pos + Vec3D(randfloat(-l,l), 0, randfloat(-w,w));
//		p.pos = sys->m_pBoneParent->mat * p.pos;
//
//		Vec3D dir = mRot * Vec3D(0,1,0);
//		p.down = Vec3D(0,-1.0f,0); // dir * -1.0f;
//		p.speed = dir.normalize() * spd * (1.0f+randfloat(-var,var));
//	}
//
//	if(!sys->m_bBillboard)
//	{
//		Vec3D look = p.dir;
//		look.x=p.dir.y;
//		look.y=p.dir.z;
//		look.z=p.dir.x;
//
//		Vec3D up = (look % p.dir).normalize();
//		Vec3D right = (up % p.dir).normalize();
//		up = (p.dir % right).normalize();
//		
//		// calculate the billboard matrix
//		p.mface.m[0][1] = right.x;
//		p.mface.m[1][1] = right.y;
//		p.mface.m[2][1] = right.z;
//		p.mface.m[0][2] = up.x;
//		p.mface.m[1][2] = up.y;
//		p.mface.m[2][2] = up.z;
//		p.mface.m[0][0] = p.dir.x;
//		p.mface.m[1][0] = p.dir.y;
//		p.mface.m[2][0] = p.dir.z;
//	}
//
//	p.life = 0;
//	p.maxlife = sys->m_Lifespan.getValue(anim, time);
//
//	p.origin = p.pos;
//
//	p.tile = randint(0, sys->m_nRows*sys->m_nCols-1);
//	if (p.tile == sys->m_nRows*sys->m_nCols)
//	{
//		p.tile--;
//	}
//	return p;
//}
//
//Particle SphereParticleEmitter::newParticle(int anim, int time, float w, float l, float spd, float var, float spr, float spr2)
//{
//    Particle p;
//	Vec3D dir;
//	float radius;
//
//	radius = randfloat(0,1);
//	
//	// Old method
//	//float t = randfloat(0,2*PI);
//
//	// New
//	// Spread should never be zero for sphere particles ?
//	float t;
//	if (spr == 0)
//		t = randfloat(-PI,PI);
//	else
//		t = randfloat(-spr,spr);
//
//	//Spread Calculation
//	Matrix mRot;
//
//	CalcSpreadMatrix(spr*2,spr2*2,w,l);
//	mRot=sys->m_pBoneParent->mRot*SpreadMat;
//
//	// New
//	// Length should never technically be zero ?
//	//if (l==0)
//	//	l = w;
//
//	// New method
//	// Vec3D bdir(w*cosf(t), 0.0f, l*sinf(t));
//	// --
//
//	// TODO: fix shpere emitters to work properly
//	/* // Old Method
//	//Vec3D bdir(l*cosf(t), 0, w*sinf(t));
//	//Vec3D bdir(0, w*cosf(t), l*sinf(t));
//
//	
//	float theta_range = sys->m_Spread.getValue(anim, time);
//	float theta = -0.5f* theta_range + randfloat(0, theta_range);
//	Vec3D bdir(0, l*cosf(theta), w*sinf(theta));
//
//	float phi_range = sys->m_Lat.getValue(anim, time);
//	float phi = randfloat(0, phi_range);
//	rotate(0,0, &bdir.z, &bdir.x, phi);
//	*/
//
//	if (sys->flags == 57 || sys->flags == 313) { // Faith Halo
//		Vec3D bdir(w*cosf(t)*1.6, 0.0f, l*sinf(t)*1.6);
//
//		p.pos = sys->m_vPos + bdir;
//		p.pos = sys->m_pBoneParent->mat * p.pos;
//
//		if (bdir.lengthSquared()==0) 
//			p.speed = Vec3D(0.0f,0.0f,0.0f);
//		else {
//			dir = sys->m_pBoneParent->mRot * (bdir.normalize());//mRot * Vec3D(0, 1.0f,0);
//			p.speed = dir.normalize() * spd * (1.0f+randfloat(-var,var));   // ?
//		}
//
//	} else {
//		Vec3D bdir;
//		float temp;
//
//		bdir = mrot * Vec3D(0,1,0) * radius;
//		temp = bdir.z;
//		bdir.z = bdir.y;
//		bdir.y = temp;
//
//		p.pos = sys->m_pBoneParent->mat * sys->m_vPos + bdir;
//			
//
//		//p.pos = sys->m_vPos + bdir;
//		//p.pos = sys->m_pBoneParent->mat * p.pos;
//		
//
//		if ((bdir.lengthSquared()==0) && ((sys->flags&0x100)!=0x100))
//		{
//			p.speed = Vec3D(0.0f,0.0f,0.0f);
//			dir = sys->m_pBoneParent->mrot * Vec3D(0,1,0);
//		}
//		else {
//			if(sys->flags&0x100)
//				dir = sys->m_pBoneParent->mrot * Vec3D(0,1,0);
//			else
//				dir = bdir.normalize();
//
//			p.speed = dir.normalize() * spd * (1.0f+randfloat(-var,var));   // ?
//		}
//	}
//
//	p.dir =  dir.normalize();//mrot * Vec3D(0, 1.0f,0);
//	p.down = Vec3D(0,-1.0f,0);
//
//	p.life = 0;
//	p.maxlife = sys->m_Lifespan.getValue(time);
//
//	p.origin = p.pos;
//
//	p.tile = randint(0, sys->m_nRows*sys->m_nCols-1);
//	return p;
//}

void RibbonEmitter::setup(int time)
{
	Vec3D ntpos;// = m_pParentBone->mat * pos;
	Vec3D ntup;// = m_pParentBone->mat * (pos + Vec3D(0,0,1));
	ntup -= ntpos;
	ntup.normalize();
	float dlen = (ntpos-tpos).length();

	m_nTime = time;

	// move first segment
	RibbonSegment &first = *segs.begin();
	Vec3D vAnim = ntpos;
	//Vec3D vTemp2 = first.pos;

	first.up = ntup;
	first.pos = ntpos;
	first.len = seglen;

	if (segs.size()*seglen<length && (segs.back().pos - tpos).length()>seglen)
	{
		RibbonSegment newseg;
		newseg.pos = segs.back().pos;
		//newseg.up = ntup;
		newseg.len = seglen;
		segs.push_back(newseg);
	}

	std::list<RibbonSegment>::iterator it = segs.begin();
	it++;
	for (; it != segs.end(); it++)
	{

		//it->up *= 0.8;
		Vec3D vN = vAnim-it->pos;
		it->up += (Vec3D(0.0f,0.01f,0.0f) + vN);

		it->up.normalize();
		//if (vN.length()<seglen*2)
		//	break;
		//vN += (it->pos - vTemp1);//+Vec3D(0,-0.001,0);

		vN = it->up*0.01f - vN;

		vN.normalize();
		vAnim += vN*seglen;
		it->pos = vAnim;

		//it->pos += it->up*0.01;
		//vAnim = it->pos;
	}




	// kill stuff from the end
	//float l = 0;
	//bool erasemode = false;
	//for (std::list<RibbonSegment>::iterator it = segs.begin(); it != segs.end();) {
	//	if (!erasemode) {
	//		l += it->len;
	//		if (l > length) {
	//			it->len = l - length;
	//			erasemode = true;
	//		}
	//		++it;
	//	} else {
	//		segs.erase(it++);
	//	}
	//}

	//tpos = ntpos;
	tcolor = Vec4D(color.getValue(time), opacity.getValue(time)/32767.0f);

	tabove = above.getValue(time);
	tbelow = below.getValue(time);

/*
	Vec3D ntpos = m_pParentBone->mat * pos;
	Vec3D ntup = m_pParentBone->mat * (pos + Vec3D(0,0,1));
	ntup -= ntpos;
	ntup.normalize();
	float dlen = (ntpos-tpos).length();


	m_nTime = time;

	// move first segment
	RibbonSegment &first = *segs.begin();
	if (first.len > seglen) {
		// add new segment
		first.back = (tpos-ntpos).normalize();
		first.len0 = first.len;
		RibbonSegment newseg;
		newseg.pos = ntpos;
		newseg.up = ntup;
		newseg.len = dlen;
		segs.push_front(newseg);
	} else {
		first.up = ntup;
		first.pos = ntpos;
		first.len += dlen;
	}


	// kill stuff from the end
	float l = 0;
	bool erasemode = false;
	for (std::list<RibbonSegment>::iterator it = segs.begin(); it != segs.end();) {
		if (!erasemode) {
			l += it->len;
			if (l > length) {
				it->len = l - length;
				erasemode = true;
			}
			++it;
		} else {
			segs.erase(it++);
		}
	}

	tpos = ntpos;
	tcolor = Vec4D(color.getValue(time), opacity.getValue(time));

	tabove = above.getValue(time);
	tbelow = below.getValue(time);*/

}

void RibbonEmitter::draw()
{
	CRenderSystem& R = GetRenderSystem();
	R.SetLightingEnabled(false);
	R.SetAlphaTestFunc(false);
	R.SetDepthBufferFunc(true, false);
	R.SetBlendFunc(true, BLENDOP_ADD, SBF_SOURCE_ALPHA, SBF_ONE);
	R.SetTextureColorOP(0,TBOP_MODULATE, TBS_TEXTURE, TBS_DIFFUSE);
	R.SetTextureAlphaOP(0,TBOP_MODULATE, TBS_TEXTURE, TBS_DIFFUSE);
/*f/*////hghghgf/////yaoyaoyoa//////////////r->SetTexture(0 , model->m_Textures[m_nTexChannel])


	//glBegin(GL_QUAD_STRIP);
	CGraphics* bg = &GetGraphics();
	//bg->Color1dw(static_cast<uint32>tcolor);
	bg->Color(tcolor.getColor());
	bg->Begin(BGMODE_QUADSTRIP, segs.size()*2+2);
	std::list<RibbonSegment>::iterator it = segs.begin();
	float l = 0;
	//signed short* buffer = GetSound().GetOscBuffer();
	int len = 0;//GetSound().GetBufferLength();
	int offset = 0;
	for (; it != segs.end(); ++it) {
        float u = l/length;
		Vec3D vUp = it->up;
		Vec3D ss = vUp;// * (buffer[offset]+ 32768.0f) / 65536.0f*2;
		//vUp =Vec3D(0,1,0);
		//vUp.normalize();

		offset++;

		bg->TexCoord2f(u,0);
		bg->Vertex3fv(it->pos +ss + tabove * vUp);
		bg->TexCoord2f(u,1);
		bg->Vertex3fv(it->pos +ss - tbelow * vUp);


		l += it->len;
	}

	if (segs.size() > 1) {
		// last segment...?
		--it;
		Vec3D vUp = it->up;
		//vUp =Vec3D(0,1,0);
		//vUp.normalize();

		bg->TexCoord2f(1,0);
		bg->Vertex3fv(it->pos + tabove * vUp + (it->len/it->len0) * it->back);
		bg->TexCoord2f(1,1);
		bg->Vertex3fv(it->pos - tbelow * vUp + (it->len/it->len0) * it->back);
	}
	bg->End();

	//glColor4f(1,1,1,1);
	//glEnable(GL_LIGHTING);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDepthMask(GL_TRUE);
}