#pragma once
#include "DataPlugsMgr.h"
#include "ModelData.h"
#include "TSingleton.h"

class CModelDataMgr: public CManager<CModelData>,public TSingleton<CModelDataMgr>
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

	CRenderNode * createRenderNode(CSkeletonData& data)// Particles
	{
			CSkeletonNode* pSkeletonNode = new CSkeletonNode;
			pSkeletonNode->setSkeletonData(&data);
			return pSkeletonNode;
	}
	
	CRenderNode * createRenderNode(ParticleData& data)// Particles
	{
			CParticleEmitter* pParticleEmitter = new CParticleEmitter;
			pParticleEmitter->init(&data);
			return pParticleEmitter;
	}

	virtual CRenderNode * createRenderNode(CLodMesh& data)// skin
	{
			CSkinModel* pSkinModel = new CSkinModel;
			pSkinModel->setMesh(&data);
			return pSkinModel;
	}
};
