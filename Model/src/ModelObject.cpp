#include "ModelObject.h"
#include "RenderSystem.h"
#include "Graphics.h"
#include "ModelDataMgr.h"
#include "ParticleDataMgr.h"

CModelObject::CModelObject()
,m_pModelData(NULL)
,m_bCreated(false)
,m_nModelID(0)
{
}

CModelObject::~CModelObject()
{
	CModelDataMgr::getInstance().del(m_nModelID);
	m_pModelData = NULL;
	S_DEL(m_pVB);
}

void CModelObject::create()
{
	if (NULL==m_pModelData)
	{
		return;
	}
	if (!m_pModelData->getLoaded())
	{
		m_pModelData->setItemName(m_strModelFilename);
 		if (CModelDataMgr::getInstance().loadModel(*m_pModelData,m_strModelFilename))
		{
			m_pModelData->Init();
		}
		else
		{
			//S_DEL(m_pModelData);
		}
		m_pModelData->setLoaded(true);
	}
	{
		CLodMesh& mesh = m_pModelData->m_Mesh;
		m_BBox	= mesh.getBBox();

		if (m_pModelData->m_Skeleton.m_Bones.size()>0)
		{
			CSkeletonData& skeleton = m_pModelData->m_Skeleton;
			m_setBonesMatrix.resize(skeleton.m_Bones.size());
			skeleton.CalcBonesMatrix("0",0,m_setBonesMatrix);
			long timeCount;
			skeleton.getAnimation(0,m_strAnimName,timeCount);
		}
		
		// Particles
		for (size_t i=0;i<m_pModelData->m_setParticle.size();++i)
		{
			CParticleEmitter* pParticleEmitter = new CParticleEmitter;
			pParticleEmitter->init(&CParticleDataMgr::getInstance().getItem(m_pModelData->m_setParticle[i].c_str()));
			char szParName[255];
			sprintf(szParName,"par%d",i);
			pParticleEmitter->setName(szParName);
			addChild(pParticleEmitter);
		}

		// 设置默认LOD
		SetLOD(0);

		// 如果是几何体动画 则进行重建VB
		if (m_pModelData->m_Mesh.m_bSkinMesh)
		{
			m_pVB = GetRenderSystem().GetHardwareBufferMgr().CreateVertexBuffer(m_pModelData->m_Mesh.getSkinVertexCount(), m_pModelData->m_Mesh.getSkinVertexSize(), CHardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
			//hr = GetRenderSystem().GetDevice()->CreateVertexBuffer(m_pModelData->GetSkinVertexBufferSize(), D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_NORMAL, D3DPOOL_DEFAULT, &m_pVB, NULL);
		}
	}
	m_bCreated=true;
}

int CModelObject::getModelDataID()const
{
	return m_nModelID;
}

CModelData* CModelObject::getModelData()const
{
	return m_pModelData;
}

void CModelObject::Register(const char* szFilename)
{
	m_nModelID = CModelDataMgr::getInstance().RegisterModel(szFilename);
	m_pModelData = CModelDataMgr::getInstance().getItem(m_nModelID);
	m_strModelFilename = szFilename;
}

bool CModelObject::load(const char* szFilename)
{
	Register(szFilename);
	create();
	return true;
}

void CModelObject::frameMove(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	// ----
	// # Dynamic loading
	// ----
	if (!isCreated())
	{
		create();
	}
	// ----
	CRenderNode::frameMove(mNewWorld,fTime,fElapsedTime);
}