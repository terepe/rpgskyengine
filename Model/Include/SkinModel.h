#pragma once
#include "RenderNode.h"
#include "Animated.h"

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
struct ModelRenderPass // 这部分的结构需要赚到材质层去 使材质更强大
{
	ModelRenderPass()
		:nTransID(-1)
		,nTexanimID(-1)
		,nColorID(-1)
		,nOrder(0)
		,p(0)
	{
	}
	// RenderFlag;
	int nTransID,nTexanimID,nColorID;
	int nOrder;
	float p;

	// Geoset ID
	int nSubID;
	// colours
	Vec4D ocol, ecol;

	std::string strMaterialName;

	bool operator< (const ModelRenderPass &m) const
	{
		// sort order method
		if (nOrder!=m.nOrder)
			return nOrder<m.nOrder;
		else
			return p<m.p;
	}
};

class CHardwareVertexBuffer;
class CLodMesh;
class CSkinModel: public CRenderNode
{
public:
	CSkinModel();
	~CSkinModel();
	virtual int		getType			() {return NODE_MODEL;}
	virtual void	frameMove		(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void	render			(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
	virtual bool	intersectSelf	(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax)const;
	void			setMesh			(CLodMesh* pMesh);
	bool			Prepare			()const;
	void			SetLOD			(unsigned long uLodID);		// 设置LodID
	void			SetLightMap		(const char* szFilename);	// SetLightMap

protected:
	CHardwareVertexBuffer*			m_pVB;				// 顶点缓冲
	CLodMesh*						m_pMesh;
	unsigned long					m_uLodLevel;		// Current Lod Level
	unsigned long					m_uLightMapTex;		//
	bool							m_bLightmap;

	public:
	virtual CONST_GET_SET_VARIABLE	(int,				m_n,Order);
	virtual size_t		getRenderPassCount();
	virtual void		setRenderPass(int nID, int nSubID, const std::string& strMaterialName);
	virtual bool		getRenderPass(int nID, int& nSubID, std::string& strMaterialName)const;
	virtual bool		delRenderPass(int nID);
	void				Init();
private:
	bool				passBegin(const ModelRenderPass& pass, float fOpacity, int nAnimTime)const;
	void				passEnd()const;
public:
	void				renderMesh(E_MATERIAL_RENDER_TYPE eModelRenderType, size_t uLodLevel, CHardwareVertexBuffer* pSkinVB, float fOpacity, int nAnimTime)const;
private:
	/*std::vector<TexAnim>					m_TexAnims;				// 纹理动画源
	std::vector<ColorAnim>					m_ColorAnims;			// 颜色动画源
	std::vector<TransAnim>					m_TransAnims;			// 透明动画源
	std::vector<LightAnim>					m_LightAnims;			// 灯光动画源*/
	std::map<int,ModelRenderPass>			m_mapPasses;			// 渲染过程集
	int										m_nOrder;
};