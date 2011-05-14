#include "ModelDataMgr.h"
#include "IORead.h"
#include "FileSystem.h"

CRenderNodeMgr::CRenderNodeMgr()
{
	m_DataPlugsMgr.loadPlugs("Plugins\\*.dme");
}
/*
unsigned long CModelDataMgr::RegisterModel(const std::string& strFilename)
{
	if (strFilename.length()==0)
	{
		return 0;
	}
	if(!IOReadBase::Exists(strFilename))// Check the filename
	{
		return 0;
	}
	if (find(strFilename))// Has the same model.
	{
		return addRef(strFilename);
	}

	CSkinModel* pModel = new CSkinModel();

	return add(strFilename, pModel);&CRenderNodeMgr::getInstance()
}
*/
CRenderNode* CRenderNodeMgr::loadRenderNode(const char* szFilename)
{
	// 判断格式--根据文件后缀名
	std::string strExt = GetExtension(szFilename);
	CModelPlugBase* pModelPlug = (CModelPlugBase*)m_DataPlugsMgr.getPlugByExtension(strExt.c_str());
	if (pModelPlug)
	{
		return pModelPlug->importData(this,szFilename);
	}
	return false;
}

CRenderNode * CRenderNodeMgr::createRenderNode(iSkeletonData* data)
{
	CSkeletonNode* pSkeletonNode = new CSkeletonNode;
	pSkeletonNode->setSkeletonData((CSkeletonData*)data);
	return pSkeletonNode;
}
	
	CRenderNode * CRenderNodeMgr::createRenderNode(ParticleData* data)
	{
			CParticleEmitter* pParticleEmitter = new CParticleEmitter;
			pParticleEmitter->init(data);
			return pParticleEmitter;
	}

	CRenderNode * CRenderNodeMgr::createRenderNode(iLodMesh* data)
	{
			CSkinModel* pSkinModel = new CSkinModel;
			pSkinModel->setMesh((CLodMesh*)data);
			return pSkinModel;
	}