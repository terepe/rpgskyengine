#pragma once
#include "DataPlugsMgr.h"
#include "TSingleton.h"
#include "SkeletonNode.h"
#include "Particle.h"
#include "SkinModel.h"

class CModelDataMgr:public TSingleton<CModelDataMgr>
{
public:
	CModelDataMgr();
	CRenderNode*	loadModel(const char* szFilename);
	CDataPlugsMgr&	getDataPlugsMgr();
private:
	CDataPlugsMgr	m_DataPlugsMgr;
};


class CRenderNodeMgr:public iRenderNodeMgr, public TSingleton<CRenderNodeMgr>
{
public:
	CRenderNodeMgr(){};
	virtual ~CRenderNodeMgr(){};

	CRenderNode * createRenderNode(iSkeletonData* data)// Particles
	{
			CSkeletonNode* pSkeletonNode = new CSkeletonNode;
			pSkeletonNode->setSkeletonData((CSkeletonData*)data);
			return pSkeletonNode;
	}
	
	CRenderNode * createRenderNode(ParticleData* data)// Particles
	{
			CParticleEmitter* pParticleEmitter = new CParticleEmitter;
			pParticleEmitter->init(data);
			return pParticleEmitter;
	}

	virtual CRenderNode * createRenderNode(iLodMesh* data)// skin
	{
			CSkinModel* pSkinModel = new CSkinModel;
			pSkinModel->setMesh((CLodMesh*)data);
			return pSkinModel;
	}
};
