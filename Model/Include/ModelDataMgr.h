#pragma once
#include "DataPlugsMgr.h"
#include "ModelData.h"
#include "TSingleton.h"

class CModelDataMgr: public CManager<CModelData>,public TSingleton<CModelDataMgr>
{
public:
	CModelDataMgr();
	unsigned long	RegisterModel(const std::string& strFilename);
	bool			loadModel(CModelData& modelData,const std::string& strFilename);
	CDataPlugsMgr&	getDataPlugsMgr();
private:
	CDataPlugsMgr	m_DataPlugsMgr;
};

class iRenderNodePlugBase:public CDataPlugBase
{
public:
	iRenderNodePlugBase(){};
	virtual ~iRenderNodePlugBase(){};

	virtual int	addChild(CRenderNode * pRenderNode, bool bShowDlg, bool bSpecifyFileName) = 0;
	virtual bool importData(CRenderNode * pRenderNode, const char* szFilename)=0;
	virtual bool exportData(CRenderNode * pRenderNode, const char* szFilename)=0;
};

class iRenderNodePlugBase:public CDataPlugBase
{
public:
	iRenderNodePlugBase(){};
	virtual ~iRenderNodePlugBase(){};

	CRenderNode * createRenderNode(CSkeletonData& data)// Particles
	{
			CSkeletonNode* pSkeletonNode = new CSkeletonNode;
			pSkeletonNode->setSkeletonData(&data);
			return pSkeletonNode;
	}
	
	CRenderNode * createRenderNode(ParticleData& data)// Particles
	{
			CParticleEmitter* pParticleEmitter = new CParticleEmitter;
			pParticleEmitter->init(&data));
			return pParticleEmitter;
	}

	virtual CRenderNode * createRenderNode(CLodMesh& data)// skin
	{
			CSkinModel* pSkinModel = new CSkinModel;
			pSkinModel->setMesh(&data);
			return pSkinModel;
	}
};
