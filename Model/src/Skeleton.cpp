#include "Skeleton.h"
#include "RenderSystem.h"
#include "Graphics.h"
#include "TextRender.h"

void CBone::CalcMatrix(int time, bool bRotate)
{
	if (m_bCalc)
		return;

	Matrix m;
	Quaternion q;

	bool tr = m_pBoneAnim->rot.isUsed() || m_pBoneAnim->scale.isUsed() || m_pBoneAnim->trans.isUsed() || m_pBoneAnim->billboard;
	if (tr)
	{
		m.translation(m_pBoneAnim->pivot);

		if (m_pBoneAnim->trans.isUsed()) {
			Vec3D tr = m_pBoneAnim->trans.getValue(time);
			m *= Matrix::newTranslation(tr);
		}

		if (m_pBoneAnim->rot.isUsed() && bRotate) {
			q = m_pBoneAnim->rot.getValue(time);
			m *= Matrix::newQuatRotate(q);
		}

		if (m_pBoneAnim->scale.isUsed()) {
			Vec3D sc = m_pBoneAnim->scale.getValue(time);
			m *= Matrix::newScale(sc);
		}

		if (m_pBoneAnim->billboard) {
			Matrix mtrans;
			//十分恼火的六氟化硫//////////////////////glGetFloatv(GL_MODELVIEW_MATRIX, &(mtrans.m[0][0]));
			mtrans.transpose();
			mtrans.Invert();
			Vec3D camera = mtrans * Vec3D(0.0f,0.0f,0.0f);
			Vec3D look = (camera - m_pBoneAnim->pivot).normalize();

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
			m *= mbb;
		}

		m *= Matrix::newTranslation(m_pBoneAnim->pivot*-1.0f);

	}
	else
	{
		m=Matrix::UNIT;
	}

	// 找到父类的转换矩阵
	if (m_pParent)
	{
		m_pParent->CalcMatrix(time, bRotate);
		m_mat = m_pParent->m_mat * m;
	}
	else
	{
		m_mat = m;
	}

	// transform matrix for normal vectors ... ??
	if (m_pBoneAnim->rot.isUsed() && bRotate)
	{
		if (m_pParent)
		{
			m_mRot = m_pParent->m_mRot * Matrix::newQuatRotate(q);
		}
		else
		{
			m_mRot = Matrix::newQuatRotate(q);
		}
	}
	else
	{
		m_mRot=Matrix::UNIT;
	}

	m_vTransPivot = m_mat * m_pBoneAnim->pivot;
	//m_mat*=m_pBoneAnim->mSkin/*mSkin.Invert()*/;
	m_bCalc = true;
}

uint8 CSkeleton::getIDByName(const std::string& strName)
{
	//for (std::vector<BoneAnim>::iterator it=m_BoneAnims.begin();it!=m_BoneAnims.end();it++)
	for (size_t i=0;i<m_BoneAnims.size();++i)
	{
		if (strName==m_BoneAnims[i].strName)
		{
			return i;
		}
	}
	return 0xFF;
}

bool CSkeleton::CreateBones(std::vector<CBone>& bones)
{
	if (m_BoneAnims.size())
	{
		bones.resize(m_BoneAnims.size());
		for (uint32 i = 0; i < bones.size(); i++)
		{
			bones[i].m_pBoneAnim = &m_BoneAnims[i];
			if (bones[i].m_pBoneAnim->parent<m_BoneAnims.size())
			{
				bones[i].m_pParent = &bones[bones[i].m_pBoneAnim->parent];
			}
			else
			{
				bones[i].m_pParent = NULL;
			}
		}
	}
	return true;
}

void CSkeleton::CalcBonesMatrix(int time, std::vector<CBone>& bones)
{
	// 重置所有骨骼为'false',说明骨骼的动画还没计算过！
	for (uint32 i = 0; i < bones.size(); i++)
	{
		bones[i].m_bCalc = false;
	}
	// 默认的骨骼动画运算
	for (uint32 i = 0; i < bones.size(); i++)
	{
		bones[i].CalcMatrix(time);
	}
	for (uint32 i = 0; i < bones.size(); i++)
	{
		bones[i].m_mat*=bones[i].m_pBoneAnim->mSkin;
		bones[i].m_mRot=bones[i].m_mat;
		bones[i].m_mRot._14=0;
		bones[i].m_mRot._24=0;
		bones[i].m_mRot._34=0;
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
			GetTextRender().drawText(s2ws(bones[i].m_pBoneAnim->strName),posScreen.x,posScreen.y);
			G.DrawLine3D(bones[i].m_pParent->m_vTransPivot,bones[i].m_vTransPivot,0xFFFFFFFF);
		}
	}

	//R.SetRenderState(D3DRS_ALPHATESTENABLE, false);
	//R.SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	//R.SetRenderState(D3DRS_SPECULARENABLE, false);
	//R.SetRenderState(D3DRS_ZENABLE, false);
	//R.SetLightingEnabled(false);
	//R.SetTextureStageState(0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1);
	//R.SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_DIFFUSE);
	//R.SetTextureStageState(0, D3DTSS_ALPHAOP,		D3DTOP_DISABLE);
	//G.Begin(BGMODE_LINE,m_BoneAnims.size()*2);
	//G.Color1dw(0xFFFFFFFF);

	//for (int i=0; i < bones.size(); i++)
	//{
	//	if (bones[i].m_pParent)
	//	{
	//		if (bones[i].m_pParent->m_pParent)
	//		{
	//			G.Color1dw(0xFF00FF00);	
	//		}
	//		else
	//		{
	//			G.Color1dw(0xFFFF0000);
	//		}
	//		G.Vertex3fv(bones[i].m_pParent->m_vTransPivot);
	//		G.Color1dw(0xFFFFFFFF);
	//		G.Vertex3fv(bones[i].m_vTransPivot);


	//		//G.Vertex3fv(bones[i].m_vTransPivot);
	//		//G.Vertex3fv(bones[i].m_pParent->m_vTransPivot + Vec3D(0.1f, 0, 0));
	//	}
	//}
	//G.End();
}

CNodeData* CSkeleton::Save(CNodeData& lump, const char* name)
{
	if (0==m_BoneAnims.size())
	{
		return false;
	}
	CNodeData* pNode = lump.SetInt(name,m_BoneAnims.size());
	if (pNode)
	{
		for (uint32 i=0; i<m_BoneAnims.size(); ++i)
		{
			CNodeData* pChild = pNode->AddNode(i);
			if (pChild)
			{
				m_BoneAnims[i].trans.Save(*pChild,"trans");
				m_BoneAnims[i].rot.Save(*pChild,"rot");
				m_BoneAnims[i].scale.Save(*pChild,"scale");

				pChild->SetString("name", m_BoneAnims[i].strName.c_str());
				pChild->SetVal("pivot", m_BoneAnims[i].pivot);
				pChild->SetVal("parent", m_BoneAnims[i].parent);
				pChild->SetVal("billboard",m_BoneAnims[i].billboard);
				pChild->SetVal("mat",m_BoneAnims[i].mSkin);
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
		m_BoneAnims.resize(nCount);
		for (uint32 i=0; i<m_BoneAnims.size(); ++i)
		{
			CNodeData* pChild = pNode->firstChild(i);
			if (pChild)
			{
				m_BoneAnims[i].trans.Load(*pChild,"trans");
				m_BoneAnims[i].rot.Load(*pChild,"rot");
				m_BoneAnims[i].scale.Load(*pChild,"scale");

				pChild->GetString("name", m_BoneAnims[i].strName);
				pChild->GetVal("pivot", m_BoneAnims[i].pivot);
				pChild->GetVal("parent", m_BoneAnims[i].parent);
				pChild->GetVal("billboard",m_BoneAnims[i].billboard);
				pChild->GetVal("mat",m_BoneAnims[i].mSkin);
			}
		}
	}
	return pNode;
}