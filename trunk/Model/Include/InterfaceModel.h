#pragma once
#include "Animated.h"
#include "Frustum.h"
#include "Vec2D.h"

//////////////////////////////////////////////////////////////////////////
struct FaceIndex
{
	FaceIndex()
	{
		memset(this,0,sizeof(*this));
	}
	uint16 v[3];
	uint16 n[3];
	uint16 c[3];
	uint16 uv1[3];
	uint16 uv2[3];
	uint16 w[3];
	uint16 b[3];
};

class iLodMesh
{
public:
	virtual void addFaceIndex(int nSubID, const FaceIndex& faceIndex)=0;
	virtual int getSubCount()=0;

	virtual const BBox& getBBox()=0;
	virtual size_t getPosCount()=0;
	virtual size_t getBoneCount()=0;
	virtual size_t getWeightCount()=0;
	virtual size_t getNormalCount()=0;
	virtual size_t getTexcoordCount()=0;

	virtual void addPos(const Vec3D& vPos)=0;
	virtual void addBone(uint32 uBone)=0;
	virtual void addWeight(uint32 uWeight)=0;
	virtual void addNormal(const Vec3D& vNormal)=0;
	virtual void addTexcoord(const Vec2D& vUV)=0;

	virtual void update()=0;
};

//////////////////////////////////////////////////////////////////////////
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

class iSkeleton
{
public:
	std::vector<BoneAnim>			m_BoneAnims;	// 骨骼动画源
	//std::vector<ModelAnimation>		m_AnimList;		// 动画配表源
};

class iModelData
{
public:
	virtual const std::string& getItemName()=0;
	virtual void addAnimation(long timeStart, long timeEnd)=0;
	virtual void setRenderPass(int nID, int nSubID, const std::string& strMaterialName)=0;
	virtual	bool loadMaterial(const std::string& strFilename)=0;
	virtual	bool loadParticleEmitters(const std::string& strFilename)=0;

	virtual	iLodMesh& getMesh()=0;
	virtual iSkeleton& getSkeleton()=0;
};

//////////////////////////////////////////////////////////////////////////
typedef BOOL (WINAPI * PFN_Model_Plug_CreateObject)(void ** pobj);
class CModelPlugBase
{
public:
	CModelPlugBase(){};
	virtual ~CModelPlugBase(){};

	virtual const char * GetTitle()		= 0;
	virtual const char * GetFormat()	= 0;
	virtual int Execute(iModelData * pModelData, bool bShowDlg, bool bSpecifyFileName) = 0;
	virtual int importData(iModelData * pModelData, const std::string& strFilename)=0;
	virtual int exportData(iModelData * pModelData, const std::string& strFilename)=0;

	virtual DWORD GetExportDataType() = 0;
	virtual DWORD GetImportDataType() = 0;

	virtual void Release() = 0;
};