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
	CRenderNode*			loadRenderNode(const char* szFilename);
	virtual CRenderNode*	createRenderNode(iSkeletonData* pData);	// Particles
	virtual CRenderNode*	createRenderNode(ParticleData* pData);		// Particles
	virtual CRenderNode*	createRenderNode(iLodMesh* pData);			// skin
private:
	CDataPlugsMgr	m_DataPlugsMgr;
};
