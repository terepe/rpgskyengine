#include "ModelObject.h"

#include "RenderSystem.h"
#include "Graphics.h"
#include "ParticleEmitter.h"
#include "Particle.h"
#include "ModelDataMgr.h"

CModelObject::CModelObject() :
m_idLightMapTex(0),
m_bLightmap(false),
m_pModelData(NULL),
m_bCreated(false)
{
	m_nModelID = 0;
	m_pVB = NULL;
	m_nAnimTime = 0;
	m_fTrans = 1;
	m_fAlpha = 1;

	m_fRad = 0;
	m_uSkinID = 0;
	m_uLodLevel = 0;
}

CModelObject::~CModelObject()
{
	CModelDataMgr::getInstance().del(m_nModelID);
	m_pModelData = NULL;
	S_DEL(m_pVB);
}

bool CModelObject::isCreated()
{
	return m_bCreated;
}

void CModelObject::create()
{
	if (NULL==m_pModelData)
	{
		return;
	}
	if (!m_pModelData->isLoaded())
	{
		if (m_pModelData->LoadFile(m_strModelFilename))
		{
			m_pModelData->Init();
		}
		else
		{
			//S_DEL(m_pModelData);
		}
	}
	{
		CLodMesh& mesh = m_pModelData->m_Mesh;
		m_BBox	= mesh.getBBox();

		if (m_pModelData->m_Skeleton.m_Bones.size()>0)
		{
			CSkeleton& skeleton = m_pModelData->m_Skeleton;
			skeleton.CreateBones(m_Bones);
			skeleton.CalcBonesMatrix("",0,m_Bones);
		}

		// Particles
		m_setParticleGroup.resize(m_pModelData->m_setParticleEmitter.size());
		for (uint32 i = 0; i < m_setParticleGroup.size(); i++)
		{
			m_setParticleGroup[i].Init(&m_pModelData->m_setParticleEmitter[i], NULL);
		}

		// 
		SetSkin(m_uSkinID);

		//if(m_pModelData->m_AnimList.size()>0)
		{
			//m_AnimMgr = new AnimManager(&m_pModelData->m_AnimList[0]);
		}
		// 设置默认LOD
		SetLOD(0);

		// 如果是几何体动画 则进行重建VB
		if (m_pModelData->m_Mesh.m_bSkinMesh)
		{
			m_pVB = GetRenderSystem().GetHardwareBufferMgr().CreateVertexBuffer(m_pModelData->m_Mesh.GetSkinVertexCount(), m_pModelData->m_Mesh.GetSkinVertexSize(), CHardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
			//hr = GetRenderSystem().GetDevice()->CreateVertexBuffer(m_pModelData->GetSkinVertexBufferSize(), D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_NORMAL, D3DPOOL_DEFAULT, &m_pVB, NULL);
		}
	}
	m_bCreated=true;
}

const BBox& CModelObject::getBBox()const
{
	return m_BBox;
}

int CModelObject::getModelDataID()const
{
	return m_nModelID;
}

CModelData* CModelObject::getModelData()const
{
	return m_pModelData;
}

void CModelObject::Register(const std::string& strFilename)
{
	m_nModelID = CModelDataMgr::getInstance().RegisterModel(strFilename);
	m_pModelData = CModelDataMgr::getInstance().GetModel(m_nModelID);
	m_strModelFilename = strFilename;
}

bool CModelObject::load(const std::string& strFilename)
{
	Register(strFilename);
	create();
	return true;
}

void CModelObject::CalcBones(const std::string& strAnim, int time)
{
	if (m_pModelData)
	{
		m_pModelData->m_Skeleton.CalcBonesMatrix(strAnim,time,m_Bones);
	}
	//// Character specific bone animation calculations.
	//if (charModelDetails.isChar)
	//{	
	//	// Animate the "core" rotations and transformations for the rest of the model to adopt into their transformations
	//	if (boneLookup[BONE_ROOT] > -1)	{
	//		for (int i=0; i<=boneLookup[BONE_ROOT]; i++) {
	//			bones[i].calcMatrix(bones, anim, time);
	//		}
	//	}

	//	// Find the close hands animation id
	//	int closeFistID = 0;
	//	for (unsigned int i=0; i<m_pModelData->m_Info.nAnimationCount; i++) {
	//		if (anims[i].animID==15) {  // closed fist
	//			closeFistID = i;
	//			break;
	//		}
	//	}

	//	// Animate key skeletal bones except the fingers which we do later.
	//	// -----
	//	int a, t;

	//	// if we have a "secondary animation" selected,  animate upper body using that.
	//	if (m_AnimMgr->GetSecondaryID() > -1) {
	//		a = m_AnimMgr->GetSecondaryID();
	//		t = m_AnimMgr->GetSecondaryFrame();
	//	} else {
	//		a = anim;
	//		t = time;
	//	}

	//	for (size_t i=0; i<5; i++) { // only goto 5, otherwise it affects the hip/waist rotation for the lower-body.
	//		if (boneLookup[i] > -1)
	//			bones[boneLookup[i]].calcMatrix(bones, a, t);
	//	}

	//	if (m_AnimMgr->GetMouthID() > -1) {
	//		// Animate the head and jaw
	//		if (boneLookup[BONE_HEAD] > -1)
	//				bones[boneLookup[BONE_HEAD]].calcMatrix(bones, m_AnimMgr->GetMouthID(), m_AnimMgr->GetMouthFrame());
	//		if (boneLookup[BONE_JAW] > -1)
	//				bones[boneLookup[BONE_JAW]].calcMatrix(bones, m_AnimMgr->GetMouthID(), m_AnimMgr->GetMouthFrame());
	//	} else {
	//		// Animate the head and jaw
	//		if (boneLookup[BONE_HEAD] > -1)
	//				bones[boneLookup[BONE_HEAD]].calcMatrix(bones, a, t);
	//		if (boneLookup[BONE_JAW] > -1)
	//				bones[boneLookup[BONE_JAW]].calcMatrix(bones, a, t);
	//	}

	//	// still not sure what 18-26 bone lookups are but I think its more for things like wrist, etc which are not as visually obvious.
	//	for (size_t i=18; i<27; i++) {
	//		if (boneLookup[i] > -1)
	//			bones[boneLookup[i]].calcMatrix(bones, a, t);
	//	}
	//	// =====

	//	
	//	
	//	if (charModelDetails.closeRHand) {
	//		a = closeFistID;
	//		t = anims[closeFistID].timeStart+1;
	//	} else {
	//		a = anim;
	//		t = time;
	//	}

	//	for (unsigned int i=0; i<5; i++) {
	//		if (boneLookup[BONE_RFINGER1 + i] > -1) 
	//			bones[boneLookup[BONE_RFINGER1 + i]].calcMatrix(bones, a, t);
	//	}

	//	if (charModelDetails.closeLHand) {
	//		a = closeFistID;
	//		t = anims[closeFistID].timeStart+1;
	//	} else {
	//		a = anim;
	//		t = time;
	//	}

	//	for (unsigned int i=0; i<5; i++) {
	//		if (boneLookup[BONE_LFINGER1 + i] > -1)
	//			bones[boneLookup[BONE_LFINGER1 + i]].calcMatrix(bones, a, t);
	//	}
	//}
	//else
	//{
	//	for (int i=0; i<boneLookup[BONE_ROOT]; i++)
	//	{
	//		bones[i].calcMatrix(bones, anim, time);
	//	}
	//}
}

void CModelObject::Animate(const std::string& strAnimName)
{
	if (NULL==m_pModelData)
	{
		return;
	}
	int t=0;
// 	ModelAnimation &a = m_pModelData->m_AnimList[strAnimName];
// 	int tmax = (a.timeEnd-a.timeStart);
// 	if (tmax==0) 
// 		tmax = 1;

	if (/*isWMO == true*/0) {
		t = globalTime;
		//t %= tmax;
		//t += a.timeStart;
	} else
		t = m_AnimMgr.uFrame;

	// 骨骼动画
	if ((m_Bones.size()>0)  && (m_nAnimTime != t || m_strAnimName != strAnimName))
	{
		CalcBones(strAnimName, t);
	}
	m_nAnimTime = t;
	m_strAnimName = strAnimName;

	// 几何体动画
	if (m_pModelData->m_Mesh.m_bSkinMesh)
	{
		m_pModelData->m_Mesh.skinningMesh(m_pVB, m_Bones);
	}

	// 灯动画？
	//for (size_t i=0; i<m_pModelData->m_LightAnims.size();; i++) {
	//	if (m_LightAnims[i].parent>=0) {
	//		m_LightAnims[i].tpos = m_Bones[lights[i].parent].m_mat * lights[i].pos;
	//		m_LightAnims[i].tdir = m_Bones[lights[i].parent].m_mRot * lights[i].dir;
	//	}
	//}

	// 粒子动画
	for (uint32 i = 0; i < m_setParticleGroup.size(); i++)
	{
		// random time distribution for teh win ..?
		int pt = globalTime;//a.timeStart + (t + (int)(tmax*m_pModelData->m_setParticleEmitter[i].tofs)) % tmax;

		m_setParticleGroup[i].Setup(pt);
	}

	// 条带动画
	//for (size_t i=0; i<m_pModelData->m_Info.nRibbonEmitterCount; i++) {
	//	ribbons[i].setup(t);
	//}
}

void CModelObject::OnFrameMove(float fElapsedTime)
{
	m_AnimMgr.Tick(int(fElapsedTime*1000));
	Animate(m_strAnimName);
}

void CModelObject::SetLOD(uint32 uLodID)
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

void CModelObject::SetSkin(uint32 uSkinID)
{

}

void CModelObject::SetAnim(const std::string& strAnimName)
{
	if (m_pModelData)
	{
		if(m_strAnimName!=strAnimName)
		{
			m_strAnimName = strAnimName;

			long timeCount;
			if (m_pModelData->getSkeleton().getAnimation(strAnimName,timeCount))
			{
				m_AnimMgr.uFrame=0;
				m_AnimMgr.uTotalFrames = timeCount;
			}
		}
	}
}

void CModelObject::SetLightMap(const std::string& strFilename)
{
	m_idLightMapTex = GetRenderSystem().GetTextureMgr().RegisterTexture(strFilename);
	GetRenderSystem().GetTextureMgr().releaseBuffer(m_idLightMapTex);
	m_bLightmap = true;
}

// These aren't really needed in the model viewer.. only wowmapviewer
//void CObject::lightsOn(int lbase)
//{
//	// setup lights
//	for (unsigned int i=0, l=lbase; i<m_pModelData->m_Info.nLightCount; i++) 
//		m_LightAnims[i].setup(m_nAnimTime, l++);
//}
//
//// These aren't really needed in the model viewer.. only wowmapviewer
//void CObject::lightsOff(int lbase)
//{
////	for (unsigned int i=0, l=lbase; i<m_pModelData->m_Info.nLightCount; i++) 
////		glDisable(l++);
//}

// Updates our particles within models.
void CModelObject::updateEmitters(const Matrix& mWorld, float fElapsedTime)
{
	for (size_t i=0; i<m_setParticleGroup.size(); i++)
	{
		CBone* pBone = &m_Bones[m_pModelData->m_setParticleEmitter[i].m_nBoneID];
		m_pModelData->m_setParticleEmitter[i].update(mWorld*pBone->m_mat,m_setParticleGroup[i],fElapsedTime);
	}
}

#include "Timer.h"

bool CModelObject::Prepare()const
{
	if(NULL==m_pModelData)
	{
		return false;
	}
	return m_pModelData->m_Mesh.SetMeshSource(m_uLodLevel,m_pVB);
}

void CModelObject::drawMesh(E_MATERIAL_RENDER_TYPE eModelRenderType)const
{
	if (NULL==m_pModelData)
	{
		return;
	}
	if (eModelRenderType==MATERIAL_RENDER_NOTHING)
	{
		return;
	}
	m_pModelData->drawMesh(eModelRenderType,m_uLodLevel,m_pVB);
}

void CModelObject::renderMesh(E_MATERIAL_RENDER_TYPE eModelRenderType)const
{
	if (NULL==m_pModelData)
	{
		return;
	}
	if (eModelRenderType==MATERIAL_RENDER_NOTHING)
	{
		return;
	}
	m_pModelData->renderMesh(eModelRenderType,m_uLodLevel,m_pVB,m_fTrans,m_nAnimTime);
}

void CModelObject::renderParticles(E_MATERIAL_RENDER_TYPE eParticleRenderType)const
{
	if (eParticleRenderType!=MATERIAL_RENDER_NOTHING)
	{
		for (size_t i = 0;i < m_setParticleGroup.size();++i)
		{
			m_setParticleGroup[i].render(eParticleRenderType);
		}
		// draw ribbons
		//for (size_t i=0; i<m_pModelData->m_Info.nRibbonEmitterCount; i++) {
		//	ribbons[i].draw();
		//}
	}
}

void CModelObject::render(E_MATERIAL_RENDER_TYPE eModelRenderType,E_MATERIAL_RENDER_TYPE eParticleRenderType)const
{
	renderMesh(eModelRenderType);
	renderParticles(eParticleRenderType);
}

void CModelObject::drawSkeleton()const
{
	if (m_pModelData)
	{
		m_pModelData->m_Skeleton.Render(m_Bones);
	}
}