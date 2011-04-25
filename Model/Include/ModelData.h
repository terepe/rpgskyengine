#pragma once
#include "Manager.h"
#include "Animated.h"
#include "Vec4D.h"
#include "Matrix.h"
#include "SkeletonData.h"
#include "LodMesh.h"
#include "Material.h"

class CModelData;
class CBone;

struct TexAnim
{
	Animated<Vec3D> trans, rot, scale;
	// 计算
	void Calc(int nTime, Matrix& matrix)const;
};

struct ColorAnim
{
	Animated<Vec3D> color;
	Animated<Vec3D> specular;
	Animated<short> opacity;

	Vec4D GetColor(unsigned long uTime)const
	{
		return Vec4D(color.getValue(uTime), opacity.getValue(uTime)/32767.0f);
	}
};

struct TransAnim // Transparency
{
	Animated<short> trans;
};

struct ModelCamera
{
	bool ok;

	Vec3D pos, target;
	float nearclip, farclip, fov;
	Animated<Vec3D> tPos, tTarget;
	Animated<float> rot;

	void setup(int time=0);

	ModelCamera():ok(false) {}
};

struct LightAnim
{
	int type, parent;
	Vec3D pos, tpos, dir, tdir;
	Animated<Vec3D> diffColor, ambColor;
	Animated<float> diffIntensity, ambIntensity;
	void setup(int time, int l);
};


//////////////////////////////////////////////////////////////////////////
#pragma pack(push) // 将当前pack设置压栈保存 
#pragma pack(1) // 必须在结构体定义之前使用 
struct ModelRenderPass
{
	ModelRenderPass()
		:nRenderFlag(0)
		,nBlendMode(0)
		,nTransID(-1)
		,nTexanimID(-1)
		,nColorID(-1)
		,nOrder(0)
		,p(0)
	{
	}
	// RenderFlag;
	unsigned short nRenderFlag;	//
	unsigned short nBlendMode;	//
	int nTransID,nTexanimID,nColorID;
	int nOrder;
	float p;

	// Geoset ID
	int nSubID;
	//
	bool bUseTex2, bTrans, bUnlit, bNoZWrite;

	// colours
	Vec4D ocol, ecol;

	std::string strMaterialName;

	bool operator< (const ModelRenderPass &m) const
	{
		// sort order method
		if (nOrder!=m.nOrder)
			return nOrder<m.nOrder;
		else
			return nBlendMode == m.nBlendMode ? (p<m.p) : (nBlendMode<m.nBlendMode);
	}
};
#pragma pack(pop) // 恢复先前的pack设置 

//////////////////////////////////////////////////////////////////////////

class CModelData:public iModelData
{
public:
	CModelData();
	~CModelData();
public:
	virtual CONST_GET_SET_VARIABLE	(std::string&,		m_str,ItemName);
	virtual CONST_GET_SET_VARIABLE	(bool,				m_b,Loaded);
	virtual CONST_GET_SET_VARIABLE	(int,				m_n,Order);
	
	virtual size_t		getRenderPassCount();
	virtual void		setRenderPass(int nID, int nSubID, const std::string& strMaterialName);
	virtual bool		getRenderPass(int nID, int& nSubID, std::string& strMaterialName)const;
	virtual bool		delRenderPass(int nID);

	virtual void		loadMaterial(const char* szFilename, const char* szParentDir);

	virtual CMaterial&	getMaterial(const char* szName);

	virtual	iLodMesh&	getMesh(){return m_Mesh;}
	virtual iSkeletonData&	getSkeleton(){return m_Skeleton;}

	virtual bool		saveMaterial(const std::string& strFilename);
	virtual	bool		loadParticleDatas(const char* szFilename);
	void				Init();
private:
	bool				passBegin(const ModelRenderPass& pass, float fOpacity, int nAnimTime)const;
	void				passEnd()const;
public:
	void				renderMesh(E_MATERIAL_RENDER_TYPE eModelRenderType, size_t uLodLevel, CHardwareVertexBuffer* pSkinVB, float fOpacity, int nAnimTime)const;
public:
	CLodMesh								m_Mesh;
 	CBoundMesh								m_BoundMesh;			// 包围盒
public: // 动画源
	CSkeletonData							m_Skeleton;				// 骨架

	std::vector<TexAnim>					m_TexAnims;				// 纹理动画源
	std::vector<ColorAnim>					m_ColorAnims;			// 颜色动画源
	std::vector<TransAnim>					m_TransAnims;			// 透明动画源
	std::vector<LightAnim>					m_LightAnims;			// 灯光动画源
	std::vector<std::string>				m_setParticle;			// 粒子动画源
private:
	std::map<int,ModelRenderPass>			m_mapPasses;			// 渲染过程集
private:
	std::string								m_strItemName;
	bool									m_bLoaded;
	int										m_nOrder;
};