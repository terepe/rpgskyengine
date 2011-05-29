#include "RenderNodeMgr.h"
#include "IORead.h"
#include "FileSystem.h"
#include "RenderSystem.h"

CRenderNodeMgr::CRenderNodeMgr()
{
	m_DataPlugsMgr.loadPlugs("Plugins\\*.dll");
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
iRenderNode* CRenderNodeMgr::loadRenderNode(const char* szFilename)
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

iRenderNode * CRenderNodeMgr::createRenderNode(iSkeletonData* pData)
{
	CSkeletonNode* pSkeletonNode = new CSkeletonNode;
	pSkeletonNode->setSkeletonData((CSkeletonData*)pData);
	return pSkeletonNode;
}

iRenderNode * CRenderNodeMgr::createRenderNode(ParticleData* pData)
{
	CParticleEmitter* pParticleEmitter = new CParticleEmitter;
	pParticleEmitter->init(pData);
	return pParticleEmitter;
}

iRenderNode * CRenderNodeMgr::createRenderNode(iLodMesh* pData)
{
	CSkinModel* pSkinModel = new CSkinModel;
	pSkinModel->setMesh((CLodMesh*)pData);
	return pSkinModel;
}

iSkeletonData* CRenderNodeMgr::getSkeletonData(const char* szName)
{
	std::map<std::string, CSkeletonData>::iterator it = m_mapSkeletonData.find(szName);
	if (it!=m_mapSkeletonData.end())
	{
		return (iSkeletonData*)&it->second;
	}
	return NULL;
}

ParticleData* CRenderNodeMgr::getParticleData(const char* szName)
{
	std::map<std::string, ParticleData>::iterator it = m_mapParticleData.find(szName);
	if (it!=m_mapParticleData.end())
	{
		return (ParticleData*)&it->second;
	}
	return NULL;
}

iLodMesh* CRenderNodeMgr::getLodMesh(const char* szName)
{
	std::map<std::string, CLodMesh>::iterator it = m_mapLodMesh.find(szName);
	if (it!=m_mapLodMesh.end())
	{
		return (iLodMesh*)&it->second;
	}
	return NULL;
}

CMaterial* CRenderNodeMgr::getMaterial(const char* szName)
{
	return &GetRenderSystem().getMaterialMgr().getItem(szName);
}

iSkeletonData* CRenderNodeMgr::createSkeletonData(const char* szName)
{
	return (iSkeletonData*)&m_mapSkeletonData[szName];
}

ParticleData* CRenderNodeMgr::createParticleData(const char* szName)
{
	return (ParticleData*)&m_mapParticleData[szName];
}

iLodMesh* CRenderNodeMgr::createLodMesh(const char* szName)
{
	return (iLodMesh*)&m_mapLodMesh[szName];
}

CMaterial* CRenderNodeMgr::createMaterial(const char* szName)
{
	return &GetRenderSystem().getMaterialMgr().getItem(szName);
}
