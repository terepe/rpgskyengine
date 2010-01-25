#pragma once
#include "c3_Common.h"
#include "HardwareIndexBuffer.h"
#include "HardwareVertexBuffer.h"
#include "Common.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "IORead.h"
#include "c3_key.h"
#include "Vec2D.h"
#include "Frustum.h"

/* 柔体物体顶点定义 */
#define PHY_OUT_VERTEX (FVF_XYZ | FVF_DIFFUSE | FVF_TEX1)
struct PhyOutVertex
{
	Vec3D							p;
	//float							nx, ny, nz;
	uint32							color;
	Vec2D							t;
};

#define		_BONE_MAX_				2
#define		_MORPH_MAX_				4

struct PhyVertex
{
	Vec3D							pos[_MORPH_MAX_];
	Vec2D							t;
	//float							nx, ny, nz;
	uint32							color;
	uint32							index[_BONE_MAX_];
	float							weight[_BONE_MAX_];
};

struct C3KeyFrame
{
	uint32							pos;
	Matrix							*matrix;
};

class DLL_EXPORT C3Motion
{
public:
	uint32							dwBoneCount;		// 骨骼数
	uint32							dwFrames;			// 帧数

	uint32							dwKeyFrames;
	C3KeyFrame						*lpKeyFrame;

	Matrix							*matrix;

	uint32							dwMorphCount;
	float							*lpMorph;
	int								nFrame;				// 当前帧
public:
	C3Motion();
	~C3Motion();
	static C3Motion* New(IOReadBase& file);
	bool Load(IOReadBase& file);
	bool Save(char *lpName, bool bNew);
	void GetMatrix(uint32 dwBone, Matrix *lpMatrix);
};

class DLL_EXPORT C3Phy
{
public:
	char							*lpName;			// 名字
	uint32							dwBlendCount;		// 每个顶点受到多少根骨骼影响
	uint32							dwNVecCount;		// 顶点数(普通顶点)
	uint32							dwAVecCount;		// 顶点数(透明顶点)
	PhyVertex						*lpVB;				// 顶点池(普通顶点/透明顶点)
	CHardwareVertexBuffer*			vb;
	uint32							dwNTriCount;		// 多边形数(普通多边形)
	uint32							dwATriCount;		// 多边形数(透明多边形)
	uint16							*lpIB;				// 索引池(普通多边形/透明多边形)
	CHardwareIndexBuffer*			ib;
	std::string						strTexName;			// [ be used by plugin ]
	int								nTex;
	int								nTex2;
	BBox							bbox;
	C3Motion						*lpMotion;
	float							fA, fR, fG, fB;
	C3Key							Key;
	bool							bDraw;
	uint32							dwTexRow;
	Matrix							InitMatrix;
	// 12-20-2002 : "STEP"
	Vec2D						uvstep;
public:
	C3Phy();
	~C3Phy();
	static C3Phy* New(IOReadBase& file, bool bTex = false);
	bool Load(IOReadBase& file, bool bTex = false);
	bool Save(char *lpName, bool bNew);
	static void Prepare();
	bool Calculate();
	bool DrawNormal();
	bool DrawAlpha(bool bZ = false, int nAsb=5, int nAdb=6);
	void NextFrame(int nStep);
	void SetFrame(uint32 dwFrame);
	void Muliply(int nBoneIndex, Matrix *matrix);
	void SetColor(float alpha, float red, float green, float blue);
	void ClearMatrix();
	void ChangeTexture(int nChangeTexID, int nChangeTexID2 = 0);
};
