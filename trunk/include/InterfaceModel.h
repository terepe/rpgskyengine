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
	void setMaterial(const char* szMaterial)
	{
		strMaterial = szMaterial;
	}
	std::string			strMaterial;
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

class iSkeletonData
{
public:
	std::vector<BoneInfo>				m_Bones;	// 骨骼
	std::map<std::string,SkeletonAnim>	m_Anims;	// 动画
	
	virtual int getBoneIDByName(const char* szName)=0;
	virtual size_t getAnimationCount()=0;
	//virtual void setAnimation(const std::string& strName, long timeCount)=0;
	virtual bool getAnimation(const std::string& strName, long& timeCount)const=0;
	virtual bool getAnimation(size_t index, std::string& strName, long& timeCount)const=0;
	virtual bool delAnimation(const std::string& strName)=0;
};



struct TexCoordSet
{
	Vec2D tc[4];
};

struct ParticleData
{
public:
	//				初速度，		变化，			伸展，		lat，	重量，		周期，		产生率，	地区L，		地区W，		减速度
	Animated<float> m_Speed,	m_Variation,	m_Spread, m_Lat,	m_Gravity,	m_Lifespan,	m_Rate,	m_Areal,	m_Areaw,	m_Deacceleration;
	Animated<unsigned char> m_Enabled;
	Color32 m_Colors[3];	// 颜色x3
	float m_Sizes[3];		// 大小x3
	// 生命的中点，减速，旋转
	float m_fLifeMid, m_fSlowdown, m_fRotation;
	Vec3D m_vPos;			// 坐标
	int m_nOrder, type;
	// 纹理动画的Tile信息
	int m_nRows, m_nCols;
	std::vector<TexCoordSet> m_Tiles;
	bool m_bBillboard;			// 公告板
	// unknown parameters omitted for now ...
	long flags;

	int	m_nBoneID;

	float tofs;

	std::string m_strMaterialName;
public:
	ParticleData(): m_nBoneID(0), m_fLifeMid(0)
	{
		//		m_nBlend = 0;
		m_nOrder = 0;
		type = 0;

		m_nRows = 0;
		m_nCols = 0;

		m_fSlowdown = 0;
		m_fRotation = 0;
		tofs = 0;

		m_Tiles.resize(1);
		m_Tiles[0].tc[0]=Vec2D(1,0);
		m_Tiles[0].tc[1]=Vec2D(1,1);
		m_Tiles[0].tc[2]=Vec2D(0,1);
		m_Tiles[0].tc[3]=Vec2D(0,0);
	}
	void InitTile(Vec2D *tc, int nID)
	{
	Vec2D otc[4];
	Vec2D a,b;
	int x = nID % m_nCols;
	int y = nID / m_nCols;
	a.x = x * (1.0f / m_nCols);
	b.x = (x+1) * (1.0f / m_nCols);
	a.y = y * (1.0f / m_nRows);
	b.y = (y+1) * (1.0f / m_nRows);

	otc[0] = a;
	otc[1].x = b.x;otc[1].y = a.y;
	otc[2] = b;
	otc[3].x = a.x;otc[3].y = b.y;

	for (int i=0; i<4; i++)
	{// 有必要这样以来吗？m_nOrder？
		tc[(i+4-m_nOrder) & 3] = otc[i];
	}
	}
};

class CRenderNode;
class iRenderNodeMgr
{
public:
	iRenderNodeMgr(){};
	virtual ~iRenderNodeMgr(){};

	virtual CRenderNode*	loadRenderNode(const char* szFilename)=0;
	virtual CRenderNode*	createRenderNode(iSkeletonData* data)=0;
	virtual CRenderNode*	createRenderNode(ParticleData* data)=0;
	virtual CRenderNode*	createRenderNode(iLodMesh* data)=0;
	virtual iSkeletonData*	createSkeletonData(const char* szName)=0;
	virtual ParticleData*	createParticleData(const char* szName)=0;
	virtual iLodMesh*		createLodMesh(const char* szName)=0;
	virtual CMaterial*		createMaterial(const char* szName)=0;
};

//////////////////////////////////////////////////////////////////////////
class CModelPlugBase:public CDataPlugBase
{
public:
	CModelPlugBase(){};
	virtual ~CModelPlugBase(){};
	virtual CRenderNode* importData(iRenderNodeMgr* pRenderNodeMgr, const char* szFilename)=0;
};