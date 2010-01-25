#pragma once
#include "modelheaders.h"
#include "Vec4D.h"
#include "Matrix.h"
#include "Animated.h"

struct BoneAnim
{
public:
	BoneAnim():
	parent(0xFF),
	billboard(false)
	{
		mSkin.unit();
	}
	std::string				strName;
	Animated<Vec3D>			trans;
	Animated<Quaternion>	rot;
	Animated<Vec3D>			scale;
	Matrix					mSkin;
	Vec3D pivot;
	uint8 parent;
	bool billboard;
};

// 骨骼类：骨骼动画的最终信息的运算及保存
class CBone
{
public:
	CBone* m_pParent;
	// 骨骼动画信息指针
	BoneAnim* m_pBoneAnim;
	// 最终转换矩阵
	Matrix m_mat;
	// 旋转变化矩阵--用于法线运算
	Matrix m_mRot;
	// 转化后的中心点
	Vec3D m_vTransPivot;
	// 是否计算过了
	bool m_bCalc;
	// 计算动画帧矩阵
	void CalcMatrix(int time, bool bRotate=true);
};

class CSkeleton
{
public:
	std::vector<BoneAnim>			m_BoneAnims;	// 骨骼动画源
	std::vector<ModelAnimation>		m_AnimList;		// 动画配表源
	uint8 getIDByName(const std::string& strName);
	bool CreateBones(std::vector<CBone>& bones);
	//void GetBoneMatrix(int nBoneID, int time, bool bRotate=true);
	void CalcBonesMatrix(int time, std::vector<CBone>& bones);
	void Render(const std::vector<CBone>& bones)const;
	CLumpNode* Save(CLumpNode& lump, const char* name);
	CLumpNode* Load(CLumpNode& lump, const char* name);
};