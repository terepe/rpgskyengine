#include "ModelObject.h"
#include "RenderSystem.h"
#include "Graphics.h"
#include "ModelDataMgr.h"

CModelObject::CModelObject()
:m_uLightMapTex(0)
,m_bLightmap(false)
,m_pModelData(NULL)
,m_bCreated(false)
,m_nModelID(0)
,m_pVB(NULL)
,m_nAnimTime(0)
,m_fTrans(1.0f)
,m_fAlpha(1.0f)
,m_fRad(0.0f)
,m_uSkinID(0)
,m_uLodLevel(0)
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
			CSkeleton& skeleton = m_pModelData->m_Skeleton;
			m_setBonesMatrix.resize(skeleton.m_Bones.size());
			skeleton.CalcBonesMatrix("0",0,m_setBonesMatrix);
			long timeCount;
			skeleton.getAnimation(0,m_strAnimName,timeCount);
		}
		
		// Particles
		for (size_t i=0;i<m_pModelData->m_setParticleData.size();++i)
		{
			CParticleEmitter* pParticleEmitter = new CParticleEmitter;
			pParticleEmitter->init(&m_pModelData->m_setParticleData[i]);
			char szParName[255];
			sprintf(szParName,"par%d",i);
			pParticleEmitter->setName(szParName);
			addChild(pParticleEmitter);
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

//void CModelObject::loadSkinModel(const char* szName,const char* szFilename)
//{
//	CSkinModel* pSkinModel = new CSkinModel();
//	pSkinModel->Register(szFilename);
//	pSkinModel->create();
//	addChild(szName, pSkinModel);
//}
//
//void CModelObject::loadChildModel(const char* szName, const char* szBoneName, const char* szFilename)
//{
//	if (strlen(szFilename)==0)
//	{
//		delChild(szName);
//		return;
//	}
//	// ----
//	LIST_RENDER_NODEL::iterator it = m_mapChildObj.find(szName);
//	if (it != m_mapChildObj.end())
//	{
//// 		if (it->second.pChildObj->getModelFilename()==szFilename)
//// 		{
//// 			return;
//// 		}
//// 		else
//		{
//			delete it->second;
//			m_mapChildObj.erase(it);
//		}
//	}
//
//	CModelObject* pModelObject = new CModelObject();
//	pModelObject->Register(szFilename);
//	pModelObject->create();
//	//pModelObject->m_strParentBoneName = szBoneName
//	addChild(szName, pModelObject);
//}

void CModelObject::CalcBones(const char* szAnim, int time)
{
	if (m_pModelData)
	{
		m_pModelData->m_Skeleton.CalcBonesMatrix(szAnim,time,m_setBonesMatrix);
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

void CModelObject::Animate(const char* szAnimName)
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
		//t = globalTime;
		//t %= tmax;
		//t += a.timeStart;
	} else
		t =			m_AnimMgr.uFrame;

	// 骨骼动画
	if ((m_setBonesMatrix.size()>0)  && (m_nAnimTime != t || m_strAnimName != szAnimName))
	{
		CalcBones(szAnimName, t);
	}
	m_nAnimTime		= t;
	m_strAnimName	= szAnimName;
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
	animate(fElapsedTime);
	// ----
	Matrix mNewWorld = mWorld*m_mWorldMatrix;
	// ----
	if (m_pParent&&m_pParent->getType()==NODE_MODEL)
	{
		CModelObject* pModel = (CModelObject*)m_pParent;
		// ----
		if (m_nBindingBoneID==-1)
		{
			m_nBindingBoneID = pModel->m_pModelData->m_Skeleton.getBoneIDByName(m_strBindingBoneName.c_str());
		}
		// ----
		if (m_nBindingBoneID!=-1)
		{
			Matrix mBoneLocal = pModel->m_pModelData->m_Skeleton.m_Bones[m_nBindingBoneID].mInvLocal;
			mBoneLocal.Invert();
			Matrix mBone = pModel->m_setBonesMatrix[m_nBindingBoneID]*mBoneLocal;
			mNewWorld *= mBone;
		}
	}
	// ----
	CRenderNode::frameMove(mNewWorld,fTime,fElapsedTime);
}

void CModelObject::animate(float fElapsedTime)
{
	m_AnimMgr.Tick(int(fElapsedTime*1000));

	Animate(m_strAnimName.c_str());

	// 几何体动画
	if (m_pModelData->m_Mesh.m_bSkinMesh)
	{
		m_pModelData->m_Mesh.skinningMesh(m_pVB, m_setBonesMatrix);
	}

	// 灯动画？
	//for (size_t i=0; i<m_pModelData->m_LightAnims.size();; i++) {
	//	if (m_LightAnims[i].parent>=0) {
	//		m_LightAnims[i].tpos = m_setBonesMatrix[lights[i].parent].m_mat * lights[i].pos;
	//		m_LightAnims[i].tdir = m_setBonesMatrix[lights[i].parent].m_mRot * lights[i].dir;
	//	}
	//}

	// 条带动画
	//for (size_t i=0; i<m_pModelData->m_Info.nRibbonEmitterCount; i++) {
	//	ribbons[i].setup(t);
	//}
}

void CModelObject::SetLOD(unsigned long uLodID)
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

void CModelObject::SetSkin(unsigned long uSkinID)
{

}

void CModelObject::setAnim(int nID)
{
	if (m_pModelData)
	{
		char szAnimName[256];
		sprintf_s(szAnimName,"%d",nID);
		setAnim(szAnimName);
	}
}

void CModelObject::setAnim(const char* szAnimName)
{
	if (m_pModelData)
	{
		if(m_strAnimName!=szAnimName)
		{
			m_strAnimName = szAnimName;

			long timeCount;
			if (m_pModelData->getSkeleton().getAnimation(szAnimName,timeCount))
			{
				m_AnimMgr.uFrame=0;
				m_AnimMgr.uTotalFrames = timeCount;
				m_AnimMgr.CurLoop=0;
			}
		}
	}
}

void CModelObject::SetLightMap(const char* szFilename)
{
	m_uLightMapTex = GetRenderSystem().GetTextureMgr().RegisterTexture(szFilename);
	GetRenderSystem().GetTextureMgr().releaseBuffer(m_uLightMapTex);
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

bool CModelObject::Prepare()const
{
	if(NULL==m_pModelData)
	{
		return false;
	}
	return m_pModelData->m_Mesh.SetMeshSource(m_uLodLevel,m_pVB);
}

void CModelObject::render(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType)const
{
	if (eRenderType==MATERIAL_NONE)
	{
		return;
	}
	// ----
	Matrix mNewWorld = mWorld*m_mWorldMatrix;
	// ----
	if (m_pParent&&m_pParent->getType()==NODE_MODEL)
	{
		/*CModelObject* pModel = (CModelObject*)m_pParent;
		// ----
		if (m_nBindingBoneID==-1)
		{
			m_nBindingBoneID = pModel->m_pModelData->m_Skeleton.getBoneIDByName(m_strBindingBoneName.c_str());
		}
		// ----
		if (m_nBindingBoneID!=-1)
		{
			Matrix mBoneLocal = pModel->m_pModelData->m_Skeleton.m_Bones[m_nBindingBoneID].mInvLocal;
			mBoneLocal.Invert();
			Matrix mBone = pModel->m_setBonesMatrix[m_nBindingBoneID]*mBoneLocal;
			mNewWorld *= mBone;
		}*/
	}
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


void CModelObject::drawSkeleton(CTextRender* pTextRender)const
{
	if (m_pModelData)
	{
		m_pModelData->m_Skeleton.render(m_setBonesMatrix, pTextRender);
	}
	// ----
	for (LIST_RENDER_NODE::const_iterator it=m_mapChildObj.begin();it!=m_mapChildObj.end();it++)
	{
		//it->second.pChildObj->drawSkeleton(pTextRender);
	}
}