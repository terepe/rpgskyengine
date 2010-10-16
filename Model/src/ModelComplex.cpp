#include "ModelComplex.h"

CModelComplex::CModelComplex()
{
}

CModelComplex::~CModelComplex()
{
}

void CModelComplex::OnFrameMove(float fElapsedTime)
{
	CModelObject::OnFrameMove(fElapsedTime);
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapSkinModel.begin();it!=m_mapSkinModel.end();it++)
	{
		const CModelData* pModelData = it->second->getModelData();
		if (pModelData)
		{
			if (pModelData->m_Mesh.m_bSkinMesh)
			{
				pModelData->m_Mesh.skinningMesh(it->second->m_pVB, m_setBonesMatrix);
			}
		}
	}
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapChildModel.begin();it!=m_mapChildModel.end();it++)
	{
		it->second->OnFrameMove(fElapsedTime);
	}
}

void CModelComplex::drawMesh(E_MATERIAL_RENDER_TYPE eModelRenderType)const
{
	CModelObject::drawMesh(eModelRenderType);
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapSkinModel.begin();it!=m_mapSkinModel.end();it++)
	{
		it->second->drawMesh(eModelRenderType);
	}
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapChildModel.begin();it!=m_mapChildModel.end();it++)
	{
		it->second->drawMesh(eModelRenderType);
	}
}

void CModelComplex::drawMeshWithTexture(E_MATERIAL_RENDER_TYPE eModelRenderType)const
{
	CModelObject::drawMeshWithTexture(eModelRenderType);
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapSkinModel.begin();it!=m_mapSkinModel.end();it++)
	{
		it->second->drawMeshWithTexture(eModelRenderType);
	}
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapChildModel.begin();it!=m_mapChildModel.end();it++)
	{
		it->second->drawMeshWithTexture(eModelRenderType);
	}
}

#include "RenderSystem.h"
void CModelComplex::renderMesh(E_MATERIAL_RENDER_TYPE eModelRenderType)const
{
	CModelObject::renderMesh(eModelRenderType);
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapSkinModel.begin();it!=m_mapSkinModel.end();it++)
	{
		it->second->renderMesh(eModelRenderType);
	}
	Matrix mWorld;
	GetRenderSystem().getWorldMatrix(mWorld);
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapChildModel.begin();it!=m_mapChildModel.end();it++)
	{
		int nBoneID = m_pModelData->m_Skeleton.getBoneIDByName(it->first.c_str());
		if (nBoneID!=-1)
		{
			Matrix mBoneLocal = m_pModelData->m_Skeleton.m_Bones[nBoneID].mInvLocal;
			mBoneLocal.Invert();
			Matrix mBone=m_setBonesMatrix[nBoneID]*mBoneLocal;
			GetRenderSystem().setWorldMatrix(mWorld*mBone);
			it->second->renderMesh(eModelRenderType);
		}
	}
}

void CModelComplex::renderParticles(E_MATERIAL_RENDER_TYPE eParticleRenderType)const
{
	CModelObject::renderParticles(eParticleRenderType);
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapChildModel.begin();it!=m_mapChildModel.end();it++)
	{
		it->second->renderParticles(eParticleRenderType);
	}
}

void CModelComplex::drawSkeleton()const
{
	CModelObject::drawSkeleton();
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapChildModel.begin();it!=m_mapChildModel.end();it++)
	{
		it->second->drawSkeleton();
	}
}

void CModelComplex::loadSkinModel(const char* szName,const char* szFilename)
{
	if (m_mapSkinModel.find(szName)!=m_mapSkinModel.end())
	{
		if (m_mapSkinModel[szName]->getModelFilename()==szFilename)
		{
			return;
		}
	}
	if (strlen(szFilename)>0)
	{
		CModelObject* pModelObject = new CModelObject();
		pModelObject->Register(szFilename);
		pModelObject->create();
		m_mapSkinModel[szName]=pModelObject;
	}
}

void CModelComplex::loadChildModel(const char* szBoneName,const char* szFilename)
{
	if (m_mapChildModel.find(szBoneName)!=m_mapChildModel.end())
	{
		if (m_mapChildModel[szBoneName]->getModelFilename()==szFilename)
		{
			return;
		}
	}
	if (strlen(szFilename)>0)
	{
		CModelObject* pModelObject = new CModelObject();
		pModelObject->Register(szFilename);
		pModelObject->create();
		m_mapChildModel[szBoneName]=pModelObject;
	}
}