#include "SkeletonNode.h"
#include "SkeletonData.h"
#include "TextRender.h"
#include "Graphics.h"

CSkeletonNode::CSkeletonNode()
	:m_nAnimTime(0)
	,m_pSkeletonData(NULL)
{
}

CSkeletonNode::~CSkeletonNode()
{
}

void CSkeletonNode::frameMove(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	m_AnimMgr.Tick(int(fElapsedTime*1000));
	animate(m_strAnimName.c_str());
	// ----
	Matrix mNewWorld = mWorld*m_mWorldMatrix;
	// ----
	if (m_pParent&&m_pParent->getType()==NODE_SKELETON)
	{
		CSkeletonNode* pModel = (CSkeletonNode*)m_pParent;
		// ----
		if (m_nBindingBoneID==-1)
		{
			m_nBindingBoneID = pModel->getSkeletonData()->getBoneIDByName(m_strBindingBoneName.c_str());
		}
		// ----
		if (m_nBindingBoneID!=-1)
		{
			Matrix mBoneLocal = pModel->getSkeletonData()->m_Bones[m_nBindingBoneID].m_mInvLocal;
			mBoneLocal.Invert();
			Matrix mBone = pModel->m_setBonesMatrix[m_nBindingBoneID]*mBoneLocal;
			mNewWorld *= mBone;
		}
	}
	// ----
	CRenderNode::frameMove(mNewWorld,fTime,fElapsedTime);
}

void CSkeletonNode::setSkeletonData(CSkeletonData* pSkeletonData)
{
	m_pSkeletonData = pSkeletonData;
	// ----
	if (!m_pSkeletonData)
	{
		return;
	}
	// ----
	if (m_pSkeletonData->m_Bones.size()>0)
	{
		m_setBonesMatrix.resize(m_pSkeletonData->m_Bones.size());
		pSkeletonData->CalcBonesMatrix("0",0,m_setBonesMatrix);
	}
	// ----
	iSkeletonAnim* pSkeletonAnim = pSkeletonData->getAnimation(0);
	if (pSkeletonAnim)
	{
		setAnim(pSkeletonAnim->getName());
	}
}

void CSkeletonNode::setAnim(int nID)
{
	char szAnimName[256];
	sprintf_s(szAnimName,"%d",nID);
	setAnim(szAnimName);
}

void CSkeletonNode::setAnim(const char* szAnimName)
{
	if (!m_pSkeletonData)
	{
		return;
	}
	m_strAnimName = szAnimName;
	iSkeletonAnim* pSkeletonAnim = m_pSkeletonData->getAnimation(szAnimName);
	if (pSkeletonAnim)
	{
		m_AnimMgr.uFrame=0;
		m_AnimMgr.uTotalFrames = pSkeletonAnim->getTotalFrames();
		m_AnimMgr.CurLoop=0;
	}
}

void CSkeletonNode::drawSkeleton(CTextRender* pTextRender)const
{
	if (!m_pSkeletonData)
	{
		return;
	}

	CRenderSystem& R = GetRenderSystem();
	CGraphics& G = GetGraphics();

	std::vector<Vec3D> setBonesPoint;
	setBonesPoint.resize(m_setBonesMatrix.size());
	for (size_t i=0;i<m_pSkeletonData->m_Bones.size();++i)
	{
		Matrix	mInvLocal = m_pSkeletonData->m_Bones[i].m_mInvLocal;
		mInvLocal.Invert();
		setBonesPoint[i]=m_setBonesMatrix[i]*mInvLocal*Vec3D(0,0,0);
	}

	if (R.prepareMaterial("Skeleton"))
	{
		R.SetDepthBufferFunc(false,false);
		for(size_t i=0;i<m_pSkeletonData->m_Bones.size();++i)
		{
			if (m_pSkeletonData->m_Bones[i].m_uParent!=255)
			{
				G.DrawLine3D(setBonesPoint[m_pSkeletonData->m_Bones[i].m_uParent],setBonesPoint[i],0xFFFFFFFF);
			}
		}
		R.finishMaterial();
	}

	R.SetBlendFunc(true);
	R.SetTextureColorOP(0,TBOP_MODULATE);
	R.SetTextureAlphaOP(0,TBOP_MODULATE);
	if (pTextRender)
	{
		for(size_t i=0;i<m_pSkeletonData->m_Bones.size();++i)
		{
			if (m_pSkeletonData->m_Bones[i].m_uParent!=255)
			{
				Pos2D posScreen;
				R.world2Screen(setBonesPoint[i],posScreen);
				pTextRender->drawText(s2ws(m_pSkeletonData->m_Bones[i].m_strName).c_str(),posScreen.x,posScreen.y);
			}
		}
	}

	// ----
	for (LIST_RENDER_NODE::const_iterator it=m_mapChildObj.begin();it!=m_mapChildObj.end();it++)
	{
		//it->second.pChildObj->drawSkeleton(pTextRender);
	}
}

void CSkeletonNode::CalcBones(const char* szAnim, int time)
{
	if (m_pSkeletonData)
	{
		m_pSkeletonData->CalcBonesMatrix(szAnim,time,m_setBonesMatrix);
	}
}

void CSkeletonNode::animate(const char* szAnimName)
{
	if (!m_pSkeletonData)
	{
		return;
	}
	int t =	m_AnimMgr.uFrame;

	// ��������
	if ((m_setBonesMatrix.size()>0)  && (m_nAnimTime != t || m_strAnimName != szAnimName))
	{
		CalcBones(szAnimName, t);
	}
	m_nAnimTime		= t;
	m_strAnimName	= szAnimName;
}
