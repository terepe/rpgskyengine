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
	renderChildMesh(mWorld, eModelRenderType);
}

void CModelComplex::renderParticles(E_MATERIAL_RENDER_TYPE eParticleRenderType)const
{
	CModelObject::renderParticles(eParticleRenderType);
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapChildModel.begin();it!=m_mapChildModel.end();it++)
	{
		it->second->renderParticles(eParticleRenderType);
	}
}

void CModelComplex::drawSkeleton(CTextRender* pTextRender)const
{
	CModelObject::drawSkeleton(pTextRender);
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapChildModel.begin();it!=m_mapChildModel.end();it++)
	{
		it->second->drawSkeleton(pTextRender);
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
	std::map<std::string,CModelObject*>::iterator it = m_mapChildModel.find(szBoneName);
	if (it != m_mapChildModel.end())
	{
		if (it->second->getModelFilename()==szFilename)
		{
			return;
		}
		else
		{
			delete it->second;
			m_mapChildModel.erase(it);
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

CModelObject* CModelComplex::getChildModel(const char* szBoneName)
{
	std::map<std::string,CModelObject*>::iterator it = m_mapChildModel.find(szBoneName);
	if (it != m_mapChildModel.end())
	{
		return it->second;
	}
	return NULL;
}

void CModelComplex::removwChildModel(const char* szBoneName)
{
	std::map<std::string,CModelObject*>::iterator it = m_mapChildModel.find(szBoneName);
	if (it != m_mapChildModel.end())
	{
		m_mapChildModel.erase(it);
	}
}

void CModelComplex::delChildModel(const char* szBoneName)
{
	std::map<std::string,CModelObject*>::iterator it = m_mapChildModel.find(szBoneName);
	if (it != m_mapChildModel.end())
	{
		delete it->second;
		m_mapChildModel.erase(it);
	}
}

void CModelComplex::exchangeChildModelPosition(const char* szBoneName1, const char* szBoneName2)
{
	CModelObject* pModel1 = getChildModel(szBoneName1);
	CModelObject* pModel2 = getChildModel(szBoneName2);

	if (pModel1)
	{
		m_mapChildModel[szBoneName2] = pModel1;
	}
	else
	{
		removwChildModel(szBoneName2);
	}

	if (pModel2)
	{
		m_mapChildModel[szBoneName1] = pModel2;
	}
	else
	{
		removwChildModel(szBoneName1);
	}
}

void CModelComplex::renderChildMesh(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eModelRenderType)const
{
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