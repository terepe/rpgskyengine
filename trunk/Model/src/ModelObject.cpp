#include "ModelObject.h"
#include "RenderSystem.h"
#include "Graphics.h"
#include "ModelDataMgr.h"
#include "ParticleDataMgr.h"
#include "Particle.h"
#include "SkinModel.h"

CModelObject::CModelObject()
:m_pModelData(NULL)
,m_bCreated(false)
,m_nModelDataID(0)
{
}

CModelObject::~CModelObject()
{
	CModelDataMgr::getInstance().del(m_nModelDataID);
	m_pModelData = NULL;
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
		// Skeleton
		if (m_pModelData->m_Skeleton.m_Bones.size()>0)
		{
			setSkeletonData(&m_pModelData->m_Skeleton);
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
		// skin
		{
			CSkinModel* pSkinModel = new CSkinModel;
			pSkinModel->setMesh(&m_pModelData->m_Mesh);
			addChild(pSkinModel);
		}
	m_bCreated=true;
}

void CModelObject::Register(const char* szFilename)
{
	m_nModelDataID = CModelDataMgr::getInstance().RegisterModel(szFilename);
	m_pModelData = CModelDataMgr::getInstance().getItem(m_nModelDataID);
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
	CSkeletonNode::frameMove(mWorld,fTime,fElapsedTime);
}