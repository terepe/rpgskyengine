#pragma once
#include "IORead.h"
#include "Vec2D.h"
#include "Vec3D.h"
#include "Vec4D.h"
#include "Matrix.h"

//左右手转换就是 z轴负一下
//Vec3D(v.x, v.y, -v.z)
//q.rotate(Vec3D(v.x,v.y,-v.z));
//
//然后剩下的就是 向上的问题
//mu是z向上， 换到我那里 就得y向上
//Vec3D(v.x, -v.z, v.y)
//Quaternion(q.x, -q.z, q.y, q.w)
//
//俩一合 就是
//Vec3D(v.x, v.z, v.y)
//
//Quaternion q;
//q.rotate(Vec3D(v.x,v.y,-v.z));
//return Quaternion(q.x, -q.z, q.y, q.w);

inline Vec3D fixCoordSystemPos(Vec3D v)
{
	return Vec3D(v.x, v.z, v.y)*0.01f;
}
inline Vec3D fixCoordSystemNormal(Vec3D v)
{
	return Vec3D(v.x, v.z, v.y);
}

inline Quaternion fixCoordSystemRotate(Vec3D v)
{
	Quaternion q;
	q.rotate(Vec3D(v.x,v.y,-v.z));
	return Quaternion(q.x, -q.z, q.y, q.w);
}
#define POINT_MEMCPY(p,pDest,size) memcpy(pDest,p,size);p+=size;
#define STRUCT_MEMCPY(p,dest) memcpy(&dest,p,sizeof(dest));p+=sizeof(dest);
#define VECTOR_MEMCPY(p,dest,_size) dest.resize(_size);memcpy(&dest[0],p,dest.size()*sizeof(dest[0]));p+=dest.size()*sizeof(dest[0]);

class CMUBmd
{
public:
	struct BmdHead
	{
		char strFile[32];
		uint16 uSubCount;
		uint16 uBoneCount;
		uint16 uAnimCount;
	};

	struct BmdSkeleton
	{
		struct BmdAnim
		{
			uint16 uFrameCount;
			bool bOffset;
			std::vector<Vec3D> vOffset;
			void load(char*& p);
		};
		struct BmdBone
		{
			bool bEmpty;
			char szName[32];
			int16 nParent;
			std::vector<Vec3D> setTrans;
			std::vector<Vec3D> setRotate;

			Matrix	mLocal;

			void load(char*& p, const std::vector<BmdAnim>& setBmdAnim);
		};
		std::vector<BmdAnim> setBmdAnim;
		std::vector<BmdBone> setBmdBone;

		Matrix getLocalMatrix(uint8 uBoneID);
		Matrix getRotateMatrix(uint8 uBoneID);
		void calcLocalMatrix(uint8 uBoneID);

		void load(char*& p, uint16 uBoneCount, uint16 uAnimCount);
	};

	struct BmdSub
	{
		struct BmdSubHead
		{
			uint16 uVertexCount;
			uint16 uNormal;
			uint16 uUVCount;
			uint16 uTriangleCount;
			uint16 uID;//?
		};
		struct BmdPos
		{
			uint32 uBones;
			Vec3D vPos;
		};
		struct BmdNormal
		{
			uint32 uBones;//?
			Vec3D vNormal;
			uint32 uUnknown2;
		};
		struct BmdTriangle
		{
			uint16 uUnknown1;//03
			uint16 indexVertex[3];
			uint16 uUnknown2;//CD
			uint16 indexNormal[3];
			uint16 uUnknown3;//CD
			uint16 indexUV[3];
			uint16 uUnknown[20];//CDCDCDCD
		};

		BmdSubHead head;
		std::vector<BmdPos> setVertex;
		std::vector<BmdNormal> setNormal;
		std::vector<Vec2D> setUV;
		std::vector<BmdTriangle> setTriangle;

		char szTexture[32];// 纹理
		void load(char*& p);
	};

	bool LoadFile(const std::string& strFilename);

	BmdHead head;
	std::vector<BmdSub> setBmdSub;
	BmdSkeleton bmdSkeleton;
	int nFrameCount;

};

#define MU_BMD_ANIM_FRAME_TIME 300

