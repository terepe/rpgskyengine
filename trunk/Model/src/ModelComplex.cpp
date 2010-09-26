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

void CModelComplex::renderMesh(E_MATERIAL_RENDER_TYPE eModelRenderType)const
{
	CModelObject::renderMesh(eModelRenderType);
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapSkinModel.begin();it!=m_mapSkinModel.end();it++)
	{
		it->second->renderMesh(eModelRenderType);
	}
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapChildModel.begin();it!=m_mapChildModel.end();it++)
	{
		it->second->renderMesh(eModelRenderType);
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

void CModelComplex::loadSkinModel(const std::string& strName,const std::string& strFilename)
{
	if (m_mapSkinModel.find(strName)!=m_mapSkinModel.end())
	{
		if (m_mapSkinModel[strName]->getModelFilename()==strFilename)
		{
			return;
		}
	}
	if (strFilename.length()>0)
	{
		CModelObject* pModelObject = new CModelObject();
		pModelObject->Register(strFilename);
		pModelObject->create();
		m_mapSkinModel[strName]=pModelObject;
		//return true;
	}
	//return false;
}