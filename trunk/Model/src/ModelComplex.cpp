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
		if (it->second->m_pMesh)
		{
			if (it->second->m_pMesh->m_bSkinMesh)
			{
				it->second->m_pMesh->SkinMesh(it->second->m_pVB, m_Bones);
			}
		}
	}
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapChildModel.begin();it!=m_mapChildModel.end();it++)
	{
		it->second->OnFrameMove(fElapsedTime);
	}
}

void CModelComplex::DrawSubsHasNoAlphaTex()const
{
	CModelObject::DrawSubsHasNoAlphaTex();
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapSkinModel.begin();it!=m_mapSkinModel.end();it++)
	{
		it->second->DrawSubsHasNoAlphaTex();
	}
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapChildModel.begin();it!=m_mapChildModel.end();it++)
	{
		it->second->DrawSubsHasNoAlphaTex();
	}
}

void CModelComplex::DrawModelEdge()const
{
	CModelObject::DrawModelEdge();
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapSkinModel.begin();it!=m_mapSkinModel.end();it++)
	{
		it->second->DrawModelEdge();
	}
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapChildModel.begin();it!=m_mapChildModel.end();it++)
	{
		it->second->DrawModelEdge();
	}
}


void CModelComplex::renderMesh(E_MATERIAL_RENDER_TYPE eModelRenderType)const
{
	CModelObject::renderMesh(eModelRenderType);
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

void CModelComplex::draw()const
{
	CModelObject::draw();
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapSkinModel.begin();it!=m_mapSkinModel.end();it++)
	{
		it->second->draw();
	}
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapChildModel.begin();it!=m_mapChildModel.end();it++)
	{
		it->second->draw();
	}
}

void CModelComplex::DrawBones()const
{
	CModelObject::DrawBones();
	for (std::map<std::string,CModelObject*>::const_iterator it=m_mapChildModel.begin();it!=m_mapChildModel.end();it++)
	{
		it->second->DrawBones();
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