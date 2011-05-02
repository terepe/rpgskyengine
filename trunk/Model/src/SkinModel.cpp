#include "SkinModel.h"
#include "RenderSystem.h"
#include "SkeletonNode.h"
#include "LodMesh.h"

CSkinModel::CSkinModel()
	:m_pVB(NULL)
	,m_uLightMapTex(0)
	,m_bLightmap(false)
	,m_uLodLevel(0)
{
}

CSkinModel::~CSkinModel()
{
	S_DEL(m_pVB);
}

void CSkinModel::frameMove(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	if (m_pParent&&m_pParent->getType()==NODE_SKELETON)
	{
		CSkeletonNode* pSkeletonNode = (CSkeletonNode*)m_pParent;
		// ----
		if (m_pMesh->m_bSkinMesh)
		{
			m_pMesh->skinningMesh(m_pVB, pSkeletonNode->getBonesMatrix());
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
	/*if (m_pModelData)
	{
		m_pModelData->renderMesh(eRenderType,m_uLodLevel,m_pVB,m_fTrans,m_nAnimTime);
	}*/
	// ----
	CRenderNode::render(mNewWorld, eRenderType);
}

bool CSkinModel::intersectSelf(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax)
{
	if (!m_pMesh)
	{
		return false;
	}
	Vec3D vNewRayPos = vRayPos;
	Vec3D vNewRayDir = vRayDir;
	transformRay(vNewRayPos,vNewRayDir,m_mWorldMatrix);
	if (m_pMesh->intersect(vNewRayPos , vNewRayDir))
	{
		return true;
	}
	return false;
}

void CSkinModel::setMesh(CLodMesh* pMesh)
{
	m_pMesh = pMesh;
	// ----
	if(!m_pMesh)
	{
		return;
	}
	// ----
	// # 设置默认LOD
	// ----
	SetLOD(0);
	// ----
	// # 如果是几何体动画 则进行重建VB
	// ----
	if (m_pMesh->m_bSkinMesh)
	{
		m_pVB = GetRenderSystem().GetHardwareBufferMgr().CreateVertexBuffer(m_pMesh->getSkinVertexCount(), m_pMesh->getSkinVertexSize(), CHardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	}
}

bool CSkinModel::Prepare()const
{
	if(!m_pMesh)
	{
		return false;
	}
	return m_pMesh->SetMeshSource(m_uLodLevel,m_pVB);
}

void CSkinModel::SetLOD(unsigned long uLodID)
{
	if (!m_pMesh)
	{
		return;
	}
	if (m_pMesh->m_Lods.size()>uLodID)
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