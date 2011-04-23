#include "Skeleton.h"
#include "RenderSystem.h"
#include "Graphics.h"
#include "TextRender.h"

void CSkeleton::calcBonesTree(int nBoneID,std::vector<Matrix>& setBonesMatrix,std::vector<bool>& setCalc)const
{
	if (setCalc[nBoneID])
	{
		return;
	}
	setCalc[nBoneID] = true;

	// 找到父类的转换矩阵
	int nParent = m_Bones[nBoneID].parent;
	if (nParent!=255)
	{
		calcBonesTree(nParent,setBonesMatrix,setCalc);
		setBonesMatrix[nBoneID] = setBonesMatrix[nParent] * setBonesMatrix[nBoneID];
	}
}

void CSkeleton::CalcBonesMatrix(const std::string& strAnim, int time, std::vector<Matrix>& setBonesMatrix)
{
	// 默认的骨骼动画运算
	std::map<std::string,SkeletonAnim>::const_iterator it = m_Anims.find(strAnim);
	if (it==m_Anims.end())
	{
		//return;
		for (size_t i=0;i<m_Bones.size();++i)
		{
			setBonesMatrix[i].unit();
		}
	}
	else
	{
		const std::vector<BoneAnim>& bonesAnim = it->second.setBonesAnim;
		for (size_t i=0;i<m_Bones.size();++i)
		{
			//bool tr = m_pBoneAnim->rot.isUsed() || m_pBoneAnim->scale.isUsed() || m_pBoneAnim->trans.isUsed() || m_pBoneAnim->billboard;
			//if (tr)
			//m_mat.translation(boneInfo.pivot);
			setBonesMatrix[i].unit();
			bonesAnim[i].transform(setBonesMatrix[i],time);
		}
	}

	std::vector<bool> setCalc(m_Bones.size(),false);// 重置所有骨骼为'false',说明骨骼的动画还没计算过！
	
	for (size_t i=0;i<m_Bones.size();++i)
	{
		calcBonesTree(i,setBonesMatrix,setCalc);
	}
	for (size_t i=0;i<m_Bones.size();++i)
	{
		if (m_Bones[i].billboard)
		{
			Matrix mtrans;
			GetRenderSystem().getViewMatrix(mtrans);
			mtrans.transpose();
			mtrans.Invert();
			Vec3D camera = mtrans * Vec3D(0.0f,0.0f,0.0f);
			Vec3D look = (camera - m_Bones[i].pivot).normalize();

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
			setBonesMatrix[i] *= mbb;
		}

		//m_mat*=Matrix::newTranslation(m_Bones[i].pivot*-1.0f);
		setBonesMatrix[i]*=m_Bones[i].mInvLocal;
		//m_mRot = Matrix::newQuatRotate(q);

		/*setBonesMatrix[i].m_mRot=setBonesMatrix[i];
		setBonesMatrix[i].m_mRot._14=0;
		setBonesMatrix[i].m_mRot._24=0;
		setBonesMatrix[i].m_mRot._34=0;*/
	}
}

void CSkeleton::calcBonesPoint(const std::vector<Matrix>& setBonesMatrix, std::vector<Vec3D>& setBonesPoint)const
{
	setBonesPoint.resize(setBonesMatrix.size());
	for (size_t i=0;i<m_Bones.size();++i)
	{
		Matrix	mInvLocal = m_Bones[i].mInvLocal;
		mInvLocal.Invert();
		setBonesPoint[i]=setBonesMatrix[i]*mInvLocal*Vec3D(0,0,0);
	}
}

void CSkeleton::render(const std::vector<Matrix>& setBonesMatrix, CTextRender* pTextRender)const
{
	CRenderSystem& R = GetRenderSystem();
	CGraphics& G = GetGraphics();

	std::vector<Vec3D> setBonesPoint;
	calcBonesPoint(setBonesMatrix, setBonesPoint);

	if (R.prepareMaterial("Skeleton"))
	{
		R.SetDepthBufferFunc(false,false);
		for(size_t i=0;i<m_Bones.size();++i)
		{
			if (m_Bones[i].parent!=255)
			{
				G.DrawLine3D(setBonesPoint[m_Bones[i].parent],setBonesPoint[i],0xFFFFFFFF);
			}
		}
		R.finishMaterial();
	}

	R.SetBlendFunc(true);
	R.SetTextureColorOP(0,TBOP_MODULATE);
	R.SetTextureAlphaOP(0,TBOP_MODULATE);
	if (pTextRender)
	{
		for(size_t i=0;i<m_Bones.size();++i)
		{
			if (m_Bones[i].parent!=255)
			{
				Pos2D posScreen;
				R.world2Screen(setBonesPoint[i],posScreen);
				pTextRender->drawText(s2ws(m_Bones[i].strName).c_str(),posScreen.x,posScreen.y);
			}
		}
	}
}

int CSkeleton::getBoneIDByName(const char* szName)
{
	size_t uBoneCount = m_Bones.size();
	// ----
	for(size_t i = 0 ; i < uBoneCount; i++)
	{
		if(strcmp(m_Bones[i].strName.c_str(),szName)==0)
		{
			return i;
		}
	}
	// ----
	return -1;
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