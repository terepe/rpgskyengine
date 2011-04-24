#include "SkinModel.h"

CSkinModel::CSkinModel()
	,m_pVB(NULL)
	:m_uLightMapTex(0)
	,m_bLightmap(false)
	,m_uLodLevel(0)
{
}

CSkinModel::~CSkinModel()
{
}

bool CSkinModel::Prepare()const
{
	if(!m_pMesh)
	{
		return false;
	}
	return m_pMesh->SetMeshSource(m_uLodLevel,m_pVB);
}

void CSkinModel::frameMove(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	//animate(fElapsedTime);
	// ----
	if (m_pParent&&m_pParent->getType()==NODE_MODEL)
	{
		CModelObject* pModel = (CModelObject*)m_pParent;
		// ----
		if (m_pModelData)
		{
			if (m_pMesh->m_bSkinMesh)
			{
				m_pMesh->skinningMesh(m_pVB, pModel->m_setBonesMatrix);
			}
		}
	}
	// ----
	CRenderNode::frameMove(mWorld,fTime,fElapsedTime);
}

void CSkinModel::render(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType)const
{
	if (eRenderType==MATERIAL_NONE)
	{
		return;
	}
	// ----
	Matrix mNewWorld = mWorld*m_mWorldMatrix;
	// ----
	GetRenderSystem().setWorldMatrix(mNewWorld);
	// ----
	if (m_pModelData)
	{
		m_pModelData->renderMesh(eRenderType,m_uLodLevel,m_pVB,m_fTrans,m_nAnimTime);
	}
	// ----
	CRenderNode::render(mNewWorld, eRenderType);
}

void CSkinModel::SetLOD(unsigned long uLodID)
{
	if (NULL==m_pModelData)
	{
		return;
	}
	if (m_pModelData->m_Mesh.m_Lods.size()>uLodID)
	{
		m_uLodLevel = uLodID;
	}
}

void CSkinModel::SetLightMap(const char* szFilename)
{
	m_uLightMapTex = GetRenderSystem().GetTextureMgr().RegisterTexture(szFilename);
	GetRenderSystem().GetTextureMgr().releaseBuffer(m_uLightMapTex);
	m_bLightmap = true;
}