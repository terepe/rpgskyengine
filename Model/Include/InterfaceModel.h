#pragma once
#include "InterfaceDataPlugsBase.h"
#include "Animated.h"
#include "Frustum.h"
#include "Material.h"

//////////////////////////////////////////////////////////////////////////
struct VertexIndex
{
	uint16 p;
	uint16 n;
	uint16 c;
	uint16 uv1;
	uint16 uv2;
	uint16 w;
	uint16 b;

	VertexIndex()
	{
		memset(this,0,sizeof(*this));
	}

	bool operator< (const VertexIndex& v) const
	{
		const uint8* p1=(const uint8*)this;
		const uint8* p2=(const uint8*)&v;
		for (size_t i=0;i<sizeof(VertexIndex);++i)
		{
			if (*p1!=*p2)
			{
				return *p1<*p2;
			}
			p1++;p2++;
		}
		return false;
	}
	bool operator== (const VertexIndex& v) const
	{
		return p==v.p&&n==v.n&&c==v.c&&uv1==v.uv1&&uv2==v.uv2&&w==v.w&&b==v.b;
	}
};

class CSubMesh
{
public:
	void addPos(const Vec3D& vPos)
	{pos.push_back(vPos);}
	void addBone(uint32 uBone)
	{bone.push_back(uBone);}
	void addWeight(uint32 uWeight)
	{weight.push_back(uWeight);}
	void addNormal(const Vec3D& vNormal)
	{normal.push_back(vNormal);}
	void addColor(const Color32& clr)
	{color.push_back(clr);}
	void addTexcoord(const Vec2D& vUV)
	{texcoord.push_back(vUV);}

	template <class _T>
	void  setVectorValue(std::vector<_T>& vec, size_t pos, const _T& val)
	{
		if (vec.size()<=pos)
		{
			vec.resize(pos+1);
		}
		vec.push_back(val);
	}

	void setPos(size_t n, const Vec3D& vPos)
	{
		setVectorValue(pos,n,vPos);
	}
	void setBone(size_t n, uint32 uBone)
	{
		setVectorValue(bone,n,uBone);
	}
	void setWeight(size_t n, uint32 uWeight)
	{
		setVectorValue(weight,n,uWeight);
	}
	void setNormal(size_t n, const Vec3D& vNormal)
	{
		setVectorValue(normal,n,vNormal);
	}
	void setTexcoord(size_t n, const Vec2D& vUV)
	{
		setVectorValue(texcoord,n,vUV);
	}

	template <class _T>
	void  getVectorValue(const std::vector<_T>& vec, size_t pos, _T& val)
	{
		if (vec.size()>pos)
		{
			val=vec[pos];
		}
	}

	void getVertexIndex(size_t n, VertexIndex& vertexIndex)
	{
		getVectorValue(m_setVertexIndex,n,vertexIndex);
	}

	void getPos(size_t n, Vec3D& vPos)
	{
		getVectorValue(pos,n,vPos);
	}
	void getBone(size_t n, uint32& uBone)
	{
		getVectorValue(bone,n,uBone);
	}
	void getWeight(size_t n, uint32& uWeight)
	{
		getVectorValue(weight,n,uWeight);
	}
	void getNormal(size_t n, Vec3D& vNormal)
	{
		getVectorValue(normal,n,vNormal);
	}
	void getTexcoord(size_t n, Vec2D& vUV)
	{
		getVectorValue(texcoord,n,vUV);
	}

	std::vector<Vec3D>	pos;
	std::vector<uint32>	weight;
	std::vector<uint32>	bone;
	std::vector<Vec3D>	normal;
	std::vector<Color32>color;
	std::vector<Vec2D>	texcoord;
	std::vector<Vec2D>	texcoord2;

	std::vector<VertexIndex> m_setVertexIndex;
};

class iLodMesh
{
public:
	virtual int getSubCount()=0;
	virtual CSubMesh& addSubMesh()=0;
	virtual CSubMesh* getSubMesh(size_t n)=0;
	virtual const BBox& getBBox()=0;
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
	  Animated<Quaternion>		rot;
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

	virtual size_t getAnimationCount()=0;
	virtual void setAnimation(const std::string& strName, long timeStart, long timeEnd)=0;
	virtual bool getAnimation(const std::string& strName, long& timeStart, long& timeEnd)const=0;
	virtual bool getAnimation(size_t index, std::string& strName, long& timeStart, long& timeEnd)const=0;
	virtual bool delAnimation(const std::string& strName)=0;

	virtual size_t getRenderPassCount()=0;
	virtual void setRenderPass(int nID, int nSubID, const std::string& strMaterialName)=0;
	virtual bool getRenderPass(int nID, int& nSubID, std::string& strMaterialName)const=0;
	virtual bool delRenderPass(int nID)=0;

	virtual CMaterial& getMaterial(const std::string& strMaterialName)=0;
	virtual	bool loadParticleEmitters(const std::string& strFilename)=0;

	virtual	iLodMesh& getMesh()=0;
	virtual iSkeleton& getSkeleton()=0;
};

//////////////////////////////////////////////////////////////////////////
class CModelPlugBase:public CDataPlugBase
{
public:
	CModelPlugBase(){};
	virtual ~CModelPlugBase(){};

	virtual const char * GetTitle()		= 0;
	virtual const char * GetFormat()	= 0;
	virtual int Execute(iModelData * pModelData, bool bShowDlg, bool bSpecifyFileName) = 0;
	virtual bool importData(iModelData * pModelData, const std::string& strFilename)=0;
	virtual bool exportData(iModelData * pModelData, const std::string& strFilename)=0;

	virtual DWORD GetExportDataType() = 0;
	virtual DWORD GetImportDataType() = 0;

	virtual void Release() = 0;
};