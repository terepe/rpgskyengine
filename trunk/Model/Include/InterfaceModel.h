#pragma once
#include "InterfaceDataPlugsBase.h"
#include "Animated.h"
#include "Frustum.h"
#include "Material.h"

//////////////////////////////////////////////////////////////////////////
struct VertexIndex
{
	unsigned short p;
	unsigned short n;
	unsigned short c;
	unsigned short uv1;
	unsigned short uv2;
	unsigned short w;
	unsigned short b;

	VertexIndex()
	{
		memset(this,0,sizeof(*this));
	}

	bool operator< (const VertexIndex& v) const
	{
		const unsigned char* p1=(const unsigned char*)this;
		const unsigned char* p2=(const unsigned char*)&v;
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
	void addBone(unsigned long uBone)
	{bone.push_back(uBone);}
	void addWeight(unsigned long uWeight)
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
	void setBone(size_t n, unsigned long uBone)
	{
		setVectorValue(bone,n,uBone);
	}
	void setWeight(size_t n, unsigned long uWeight)
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
	void getBone(size_t n, unsigned long& uBone)
	{
		getVectorValue(bone,n,uBone);
	}
	void getWeight(size_t n, unsigned long& uWeight)
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
	std::vector<unsigned long>	weight;
	std::vector<unsigned long>	bone;
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
struct BoneInfo
{
	BoneInfo():
	parent(0xFF),
	billboard(false)
	{
		mInvLocal.unit();
	}
	std::string				strName;
	Matrix					mInvLocal;
	Vec3D pivot;
	unsigned char parent;
	bool billboard;
};

struct BoneAnim
{
	Animated<Vec3D>			trans;
	Animated<Quaternion>	rot;
	Animated<Vec3D>			scale;
	void transform(Matrix& m, unsigned int time)const
	{
		if (trans.isUsed())
		{
			Vec3D tr = trans.getValue(time);
			m *= Matrix::newTranslation(tr);
		}
		if (rot.isUsed())
		{
			Quaternion q = rot.getValue(time);
			m *= Matrix::newQuatRotate(q);
		}
		if (scale.isUsed())
		{
			Vec3D sc = scale.getValue(time);
			m *= Matrix::newScale(sc);
		}
	}
};

struct SkeletonAnim
{
	std::vector<BoneAnim> setBonesAnim;
	float fSpeed;
	unsigned int uTotalFrames;
};

class iSkeleton
{
public:
	std::vector<BoneInfo>				m_Bones;	// ¹Ç÷À
	std::map<std::string,SkeletonAnim>	m_Anims;	// ¶¯»­
	
	virtual size_t getAnimationCount()=0;
	//virtual void setAnimation(const std::string& strName, long timeCount)=0;
	virtual bool getAnimation(const std::string& strName, long& timeCount)const=0;
	virtual bool getAnimation(size_t index, std::string& strName, long& timeCount)const=0;
	virtual bool delAnimation(const std::string& strName)=0;
};

class iModelData
{
public:
	virtual const std::string& getItemName()=0;

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