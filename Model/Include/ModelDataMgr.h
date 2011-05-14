#pragma once
#include "DataPlugsMgr.h"
#include "TSingleton.h"
#include "SkeletonNode.h"
#include "Particle.h"
#include "SkinModel.h"

class CRenderNodeMgr:public iRenderNodeMgr, public TSingleton<CRenderNodeMgr>
{
public:
	CRenderNodeMgr();
	virtual ~CRenderNodeMgr(){};
	CRenderNode*	loadRenderNode(const char* szFilename);
	virtual CRenderNode * createRenderNode(iSkeletonData* data);// Particles
	virtual CRenderNode * createRenderNode(ParticleData* data);// Particles
	virtual CRenderNode * createRenderNode(iLodMesh* data);// skin
private:
	CDataPlugsMgr	m_DataPlugsMgr;
};
