#include "ModelObject.h"

#include "RenderSystem.h"
#include "Graphics.h"
#include "ParticleEmitter.h"
#include "Particle.h"


CModelObject::CModelObject() :
m_idLightMapTex(0),
m_bLightmap(false),
m_pModelData(NULL),
m_pMesh(NULL),
m_bCreated(false),
m_vAmbient(0.4f,0.4f,0.4f,1.0f),
m_vDiffuse(0.6f,0.6f,0.6f,1.0f)
{
	m_nModelID = -1;
	m_AnimMgr = NULL;
	m_pVB = NULL;
	m_nCurrentAnimID = 0;
	m_nAnimTime = 0;
	m_fTrans = 1;
	m_fAlpha = 1;

	m_ModelType = MT_NORMAL;
	m_fRad = 0;
	m_nLodLevel = 0;
	m_uSkinID = 0;
	m_uLodID = 0;

	m_pSkeleton = NULL;
}

CModelObject::~CModelObject()
{
	GetModelMgr().del(m_nModelID);
	m_pModelData = NULL;

	S_DEL(m_pVB);
}

bool CModelObject::isCreated()
{
	return m_bCreated;
}

void CModelObject::create()
{
	S_DEL(m_AnimMgr);
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
		m_pMesh = &m_pModelData->m_Mesh;
		m_BBox	= m_pModelData->m_Mesh.getBBox();
		//
		if (m_pModelData->m_Skeleton.m_BoneAnims.size()>0)
		{
			m_pSkeleton = &m_pModelData->m_Skeleton;
			m_pSkeleton->CreateBones(m_Bones);
			m_pSkeleton->CalcBonesMatrix(0,m_Bones);
		}

		// Particles
		m_setParticleGroup.resize(m_pModelData->m_setParticleEmitter.size());
		for (uint32 i = 0; i < m_setParticleGroup.size(); i++)
		{
			m_setParticleGroup[i].Init(&m_pModelData->m_setParticleEmitter[i], NULL);
		}
		m_setShowParticle.resize(m_setParticleGroup.size());

		// 
		SetSkin(m_uSkinID);

		if(m_pModelData->m_AnimList.size()>0)
		{
			m_AnimMgr = new AnimManager(&m_pModelData->m_AnimList[0]);
		}
		// 设置默认LOD
		SetLOD(0);

		// 创建VB
		//OnResetDevice();
		{
			// 如果是几何体动画 则进行重建VB
			if (m_pMesh->m_bSkinMesh)
			{
				m_pVB = GetRenderSystem().GetHardwareBufferMgr().CreateVertexBuffer(m_pMesh->GetSkinVertexCount(), m_pMesh->GetSkinVertexSize(), CHardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
				//hr = GetRenderSystem().GetDevice()->CreateVertexBuffer(m_pModelData->GetSkinVertexBufferSize(), D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_NORMAL, D3DPOOL_DEFAULT, &m_pVB, NULL);
			}
		}
	}
	m_bCreated=true;
}

const BBox& CModelObject::getBBox()const
{
	return m_BBox;
}

void CModelObject::Register(const std::string& strFilename)
{
	m_nModelID = GetModelMgr().RegisterModel(strFilename);
	m_pModelData = GetModelMgr().GetModel(m_nModelID);
	m_strModelFilename = strFilename;
}

bool CModelObject::load(const std::string& strFilename)
{
	Register(strFilename);
	create();
	return true;
}

void CModelObject::CalcBones(int time)
{
	m_pSkeleton->CalcBonesMatrix(time, m_Bones);

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

void CModelObject::Animate(int anim)
{
	int t=0;
	ModelAnimation &a = m_pModelData->m_AnimList[anim];
	int tmax = (a.timeEnd-a.timeStart);
	if (tmax==0) 
		tmax = 1;

	if (/*isWMO == true*/0) {
		t = globalTime;
		t %= tmax;
		t += a.timeStart;
	} else
		t = m_AnimMgr->GetFrame();

	// 骨骼动画
	if ((m_Bones.size()>0)  && (m_nAnimTime != t || m_nAnim != anim))
	{
		CalcBones(t);
	}
	m_nAnimTime = t;
	m_nAnim = anim;

	// 几何体动画
	if (m_pMesh->m_bSkinMesh)
	{
		m_pMesh->SkinMesh(m_pVB, m_Bones);
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
		int pt = a.timeStart + (t + (int)(tmax*m_pModelData->m_setParticleEmitter[i].tofs)) % tmax;

		m_setParticleGroup[i].Setup(pt);
	}

	// 条带动画
	//for (size_t i=0; i<m_pModelData->m_Info.nRibbonEmitterCount; i++) {
	//	ribbons[i].setup(t);
	//}
}

bool CModelObject::PassBegin(ModelRenderPass& pass)const
{
		Vec4D ocol = Vec4D(1.0f, 1.0f, 1.0f, m_fTrans);
		Vec4D ecol = Vec4D(0.0f, 0.0f, 0.0f, 0.0f);

		float fOpacity = m_fTrans;
		// emissive colors
		if (m_pModelData->m_TransAnims.size() > 0)
		{
			// opacity
			if (pass.nTransID!=-1)
			{
				fOpacity *= m_pModelData->m_TransAnims[pass.nTransID].trans.getValue(m_nAnimTime)/32767.0f;
			}
		}
		if (fOpacity<=0.0f)
		{
			return false;
		}
		if (-1 != pass.nColorID)
		{
			Vec4D ecol = m_pModelData->m_ColorAnims[pass.nColorID].GetColor(m_nAnimTime);
			ecol.w = 1;
			pass.material.SetEmissiveColor(ocol.getColor());

			//glMaterialfv(GL_FRONT, GL_EMISSION, ecol);
			/*			D3DMATERIAL9 mtrl;
			mtrl.Ambient	= *(D3DXCOLOR*)&ecol;//D3DXCOLOR(0.2,0.2,0.2,0.2);
			mtrl.Diffuse	= *(D3DXCOLOR*)&ecol;//D3DXCOLOR(0.8,0.8,0.8,0.8);
			Vec4D Specular	= Vec4D(m_pModelData->m_ColorAnims[pass.nColorID].color.getValue(m_nAnimTime), 1);
			mtrl.Specular	= *(D3DXCOLOR*)&Specular;
			mtrl.Emissive	= D3DXCOLOR(0,0,0,0);//*(D3DXCOLOR*)&ecol;
			mtrl.Power		= 71;
			R.SetMaterial(&mtrl);
			R.SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);*/
			//R.SetRenderState(D3DRS_SPECULARENABLE, true);
			//R.SetRenderState(D3DRS_LOCALVIEWER, true);
			//R.SetRenderState(D3DRS_NORMALIZENORMALS, true);
			//R.SetRenderState(D3DRS_LOCALVIEWER, false);

		}
		if(m_bLightmap)
		{
			pass.material.uLightMap = m_idLightMapTex;
		}

		// TEXTURE

		// Texture wrapping around the geometry
		//if (swrap)
		//	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		//if (twrap)
		//	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

		// 纹理动画实现
		if (m_pModelData->m_TexAnims.size() && pass.nTexanimID !=-1)
		{
			// 纹理动画
			Matrix matTex;
			m_pModelData->m_TexAnims[pass.nTexanimID].Calc(m_nAnimTime, matTex);
			// 在里面设置纹理矩阵
			GetRenderSystem().setTextureMatrix(0, TTF_COUNT2, matTex);
		}
		// color
		//glColor4fv(ocol);
		//glMaterialfv(GL_FRONT, GL_SPECULAR, ocol);

		//if (!pass.bUnlit&&0) 
		//{
		//	//R.SetLightingEnabled(false);
		//	R.SetShader(m_pModelData->m_nCartoonShaderID);
		//	static int nCartoonTex = GetRenderSystem().GetTextureMgr().RegisterTexture("toonshade.tga");
		//	R.SetTexture(1 , nCartoonTex, 1);
		//	COLOROP = D3DTOP_SELECTARG1;

		//	R.SetTextureFactor(Color32(176,176,176,176));
		//	R.SetTextureColorOP(1,TBOP_MODULATE, TBS_CURRENT, TBS_TEXTURE);
		//}
		return GetRenderSystem().prepareMaterial(pass.material,fOpacity);
}

void CModelObject::PassEnd()const
{
	CRenderSystem& R = GetRenderSystem();
	R.finishMaterial();
	R.setTextureMatrix(0, TTF_DISABLE);
	R.SetTexCoordIndex(0,0);
	R.SetTexCoordIndex(1,0);
	//R.SetRenderState(D3DRS_SPECULARENABLE, false);
	R.SetTexture(1 , -1);
	R.SetShader((CShader*)NULL);
	R.SetTextureColorOP(1,TBOP_DISABLE);
	R.SetTextureAlphaOP(1,TBOP_DISABLE);
}

void CModelObject::OnFrameMove(float fElapsedTime)
{
	if (m_AnimMgr)
	{
		m_AnimMgr->Tick(int(fElapsedTime*1000));
		//m_nCurrentAnimID++;
				Animate(m_nCurrentAnimID);
	}
}

void CModelObject::SetLOD(uint32 uLodID)
{
	if (m_pMesh->m_Lods.size()>uLodID)
	{
		m_uLodID = uLodID;
		m_setShowSubset.resize(m_pMesh->m_Lods[ uLodID ].setSubset.size());
		for (uint32 i = 0; i < m_setShowSubset.size(); i++)
		{
			m_setShowSubset[i] = true;
		}
	}
}

void CModelObject::SetSkin(uint32 uSkinID)
{

}

void CModelObject::SetAnim(uint32 uAnimID)
{
	if (m_pModelData&&m_pModelData->m_AnimList.size() > uAnimID)
	{
		if(m_AnimMgr)
		{
			if(m_nCurrentAnimID!=uAnimID)
			{
				m_nCurrentAnimID = uAnimID;
				//m_AnimMgr->Stop();
				m_AnimMgr->Set(0, uAnimID, -1);
				m_AnimMgr->Play();
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

void CModelObject::SetMaterial(const Vec4D& vAmbient, const Vec4D& vDiffuse)
{
	m_vAmbient = vAmbient;
	m_vDiffuse = vDiffuse;
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
	return m_pMesh&&m_pMesh->SetMeshSource(m_uLodID,m_pVB);
}

void CModelObject::DrawSubsHasNoAlphaTex()const
{
	for (std::map<int,ModelRenderPass>::iterator it = m_pModelData->m_mapPasses.begin(); it != m_pModelData->m_mapPasses.end(); ++it)
	{
		if (!it->second.bHasAlphaTex)
		{
			m_pMesh->DrawSub(m_uLodID, it->second.nSubID);
		}
	}
}

bool CModelObject::PrepareEdge()const
{
	CRenderSystem& R = GetRenderSystem();
	R.SetDepthBufferFunc(true, false);
	R.SetAlphaTestFunc(false);
	R.SetBlendFunc(true, BLENDOP_ADD, SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA);
	//R.SetBlendFunc(true, BLENDOP_ADD, SBF_DEST_COLOUR, SBF_ZERO);

	R.SetLightingEnabled(false);
	R.SetCullingMode(CULL_CLOCK_WISE);

	R.SetTextureFactor(Color32(176,176,176,176));
	R.SetTextureColorOP(0,TBOP_SOURCE1, TBS_TFACTOR);
	R.SetTextureAlphaOP(0,TBOP_DISABLE);

	//R.SetShader(m_pModelData->m_nEdgeShaderID);
	return true;
}

void CModelObject::FinishEdge()const
{
	GetRenderSystem().SetShader((CShader*)NULL);
}

void CModelObject::DrawModelEdge()const
{
	if (PrepareEdge())
	{
		if (m_pModelData->m_bHasAlphaTex)
		{
			DrawSubsHasNoAlphaTex();
		}
	}
	FinishEdge();
}

void CModelObject::renderMesh(E_MATERIAL_RENDER_TYPE eModelRenderType)const
{
	//GetRenderSystem().SetMaterial(m_vAmbient,m_vDiffuse);
	//GetRenderSystem().GetSharedShader()->setVec3D("g_vAmbient",m_vAmbient);
	//GetRenderSystem().GetSharedShader()->setVec3D("g_vDiffuse",m_vDiffuse);
	if (eModelRenderType!=MATERIAL_RENDER_NOTHING&&Prepare())
	{
		for (std::map<int,ModelRenderPass>::iterator it = m_pModelData->m_mapPasses.begin(); it != m_pModelData->m_mapPasses.end(); ++it)
		{
			if (m_setShowSubset[it->second.nSubID])
			{
				if (it->second.material.getRenderType()&eModelRenderType)
				{
					if (PassBegin(it->second))
					{
						m_pMesh->DrawSub(m_uLodID, it->second.nSubID);
					}
					PassEnd();
					//	GetRenderSystem().GetDevice()->SetStreamSourceFreq(0,1);
					//	GetRenderSystem().GetDevice()->SetStreamSourceFreq(1,1);
				}
			}
		}
	}
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

void CModelObject::draw()const
{
	if (Prepare())
	{
		if (m_pMesh)
		{
			m_pMesh->draw();
		}
	}
}

void CModelObject::DrawBones()const
{
	if (m_pSkeleton)
	{
		m_pSkeleton->Render(m_Bones);
	}
}