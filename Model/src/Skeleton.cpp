#include "Skeleton.h"
#include "RenderSystem.h"
#include "Graphics.h"
#include "TextRender.h"

void CBone::CalcMatrix1(const BoneAnim& boneAnim,int time)
{
	m_bCalc = false;	// 重置所有骨骼为'false',说明骨骼的动画还没计算过！

	//bool tr = m_pBoneAnim->rot.isUsed() || m_pBoneAnim->scale.isUsed() || m_pBoneAnim->trans.isUsed() || m_pBoneAnim->billboard;
	//if (tr)
	//m_mat.translation(boneInfo.pivot);
	m_mat.unit();
	boneAnim.transform(m_mat,time);
}
void CBone::CalcMatrix2()
{
	if (m_bCalc)
	{
		return;
	}
	m_bCalc = true;

	// 找到父类的转换矩阵
	if (m_pParent)
	{
		m_pParent->CalcMatrix2();
		m_mat = m_pParent->m_mat * m_mat;
		//m_mRot = m_pParent->m_mRot * m_mRot;
	}
}

void CBone::CalcMatrix3(const BoneInfo& boneInfo)
{
	if (boneInfo.billboard)
	{
		Matrix mtrans;
		GetRenderSystem().getViewMatrix(mtrans);
		mtrans.transpose();
		mtrans.Invert();
		Vec3D camera = mtrans * Vec3D(0.0f,0.0f,0.0f);
		Vec3D look = (camera - boneInfo.pivot).normalize();

		Vec3D up = ((mtrans * Vec3D(0.0f,1.0f,0.0f)) - camera).normalize();
		// these should be normalized by default but fp inaccuracy kicks in when looking down :(
		Vec3D right = (up % look).normalize();
		up = (look % right).normalize();

		// calculate a billboard matrix
		Matrix mbb=Matrix::UNIT;
		//mbb.m[0][2] = right.x;
		//mbb.m[1][2] = right.y;
		//mbb.m[2][2] = right.z;
		mbb.m[3][2] = 0.0f;
		mbb.m[0][1] = up.x;
		mbb.m[1][1] = up.y;
		mbb.m[2][1] = up.z;
		mbb.m[3][1] = 0.0f;
		//mbb.m[0][0] = look.x;
		//mbb.m[1][0] = look.y;
		//mbb.m[2][0] = look.z;
		mbb.m[3][0] = 0.0f;
		/*
		mbb.m[0][2] = right.x;
		mbb.m[1][2] = right.y;
		mbb.m[2][2] = right.z;
		mbb.m[0][1] = up.x;
		mbb.m[1][1] = up.y;
		mbb.m[2][1] = up.z;
		mbb.m[0][0] = look.x;
		mbb.m[1][0] = look.y;
		mbb.m[2][0] = look.z;
		*/
		m_mat *= mbb;
	}

	m_vTransPivot = m_mat * boneInfo.pivot;

	//m_mat*=Matrix::newTranslation(boneInfo.pivot*-1.0f);
	m_mat*=boneInfo.mInvLocal;
	//m_mRot = Matrix::newQuatRotate(q);


	m_mRot=m_mat;
	m_mRot._14=0;
	m_mRot._24=0;
	m_mRot._34=0;
}

uint8 CSkeleton::getIDByName(const std::string& strName)
{
	for (size_t i=0;i<m_Bones.size();++i)
	{
		if (strName==m_Bones[i].strName)
		{
			return i;
		}
	}
	return 0xFF;
}

bool CSkeleton::CreateBones(std::vector<CBone>& bones)
{
	if (m_Bones.size())
	{
		bones.resize(m_Bones.size());
		for (uint32 i = 0; i < bones.size(); i++)
		{
			//bones[i].m_pBoneAnim = &m_BoneAnims[i];
			if (m_Bones[i].parent<m_Bones.size())
			{
				bones[i].m_pParent = &bones[m_Bones[i].parent];
			}
			else
			{
				bones[i].m_pParent = NULL;
			}
		}
	}
	return true;
}

void CSkeleton::CalcBonesMatrix(const std::string& strAnim, int time, std::vector<CBone>& bones)
{
	// 默认的骨骼动画运算
	std::map<std::string,SkeletonAnim>::const_iterator it = m_Anims.find(strAnim);
	if (it==m_Anims.end())
	{
		return;
	}
	const std::vector<BoneAnim>& bonesAnim = it->second.setBonesAnim;
	for (uint32 i = 0; i < bones.size(); i++)
	{
		bones[i].CalcMatrix1(bonesAnim[i],time);
	}
	for (uint32 i = 0; i < bones.size(); i++)
	{
		bones[i].CalcMatrix2();
	}
	for (uint32 i = 0; i < bones.size(); i++)
	{
		bones[i].CalcMatrix3(m_Bones[i]);
	}
}

void CSkeleton::Render(const std::vector<CBone>& bones)const
{
	CRenderSystem& R = GetRenderSystem();
	CGraphics& G = GetGraphics();

	R.SetAlphaTestFunc(false);
	R.SetBlendFunc(false);
	R.SetDepthBufferFunc(false, false);
	R.SetLightingEnabled(false);
	R.SetTextureColorOP(0,TBOP_SOURCE2);
	R.SetTextureAlphaOP(0,TBOP_DISABLE);

	for (uint32 i=0; i < bones.size(); i++)
	{
		if (bones[i].m_pParent)
		{
			G.DrawLine3D(bones[i].m_pParent->m_vTransPivot,bones[i].m_vTransPivot,0xFFFFFFFF);
		}
	}
	R.SetBlendFunc(true);
	R.SetTextureColorOP(0,TBOP_MODULATE);
	R.SetTextureAlphaOP(0,TBOP_MODULATE);
	for (uint32 i=0; i < bones.size(); i++)
	{
		if (bones[i].m_pParent)
		{
			Pos2D posScreen;
			R.world2Screen(bones[i].m_vTransPivot,posScreen);
			GetTextRender().drawText(s2ws(m_Bones[i].strName),posScreen.x,posScreen.y);
			G.DrawLine3D(bones[i].m_pParent->m_vTransPivot,bones[i].m_vTransPivot,0xFFFFFFFF);
		}
	}
}

CNodeData* CSkeleton::Save(CNodeData& lump, const char* name)
{
	if (0==m_Bones.size())
	{
		return false;
	}
	CNodeData* pNode = lump.SetInt(name,m_Bones.size());
	if (pNode)
	{
		for (uint32 i=0; i<m_Bones.size(); ++i)
		{
			CNodeData* pChild = pNode->AddNode(i);
			if (pChild)
			{
				//m_BoneAnims[i].trans.Save(*pChild,"trans");
				//m_BoneAnims[i].rot.Save(*pChild,"rot");
				//m_BoneAnims[i].scale.Save(*pChild,"scale");

// 				pChild->SetString("name", m_BoneAnims[i].strName.c_str());
// 				pChild->SetVal("pivot", m_BoneAnims[i].pivot);
// 				pChild->SetVal("parent", m_BoneAnims[i].parent);
// 				pChild->SetVal("billboard",m_BoneAnims[i].billboard);
// 				pChild->SetVal("mat",m_BoneAnims[i].mSkin);
			}
		}
	}
	return pNode;
}

CNodeData* CSkeleton::Load(CNodeData& lump, const char* name)
{
	int nCount = 0;
	CNodeData* pNode = lump.GetInt(name, nCount);
	if (pNode)
	{
		m_Bones.resize(nCount);
		for (uint32 i=0; i<m_Bones.size(); ++i)
		{
			CNodeData* pChild = pNode->firstChild(i);
			if (pChild)
			{
// 				m_BoneAnims[i].trans.Load(*pChild,"trans");
// 				m_BoneAnims[i].rot.Load(*pChild,"rot");
// 				m_BoneAnims[i].scale.Load(*pChild,"scale");
// 
// 				pChild->GetString("name", m_BoneAnims[i].strName);
// 				pChild->GetVal("pivot", m_BoneAnims[i].pivot);
// 				pChild->GetVal("parent", m_BoneAnims[i].parent);
// 				pChild->GetVal("billboard",m_BoneAnims[i].billboard);
// 				pChild->GetVal("mat",m_BoneAnims[i].mSkin);
			}
		}
	}
	return pNode;
}

size_t CSkeleton::getAnimationCount()
{
	return m_Anims.size();
}

bool CSkeleton::getAnimation(const std::string& strName, long& timeCount)const
{
	std::map<std::string,SkeletonAnim>::const_iterator it = m_Anims.find(strName);
	if (it==m_Anims.end())
	{
		return false;
	}
	timeCount = it->second.uTotalFrames;
	return true;
}

bool CSkeleton::getAnimation(size_t index, std::string& strName, long& timeCount)const
{
	if (m_Anims.size()<=index)
	{
		return false;
	}
	std::map<std::string,SkeletonAnim>::const_iterator it = m_Anims.begin();
	advance(it,index);
	strName = it->first;
	timeCount = it->second.uTotalFrames;
	return true;
}

bool CSkeleton::delAnimation(const std::string& strName)
{
	std::map<std::string,SkeletonAnim>::iterator it=m_Anims.find(strName);
	if(it!=m_Anims.end())
	{
		m_Anims.erase(it);
		return true;
	}
	return false;
}