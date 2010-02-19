#pragma once
#include "Manager.h"
#include "Modelheaders.h"
#include "Animated.h"
//#include "enums.h"
#include "ParticleEmitter.h"

#include "Vec4D.h"
#include "Matrix.h"

#include "Skeleton.h"
#include "LodMesh.h"

class CModelData;
class CBone;

struct TexAnim
{
	Animated<Vec3D> trans, rot, scale;
	// 计算
	void Calc(int nTime, Matrix& matrix);
};

struct ColorAnim
{
	Animated<Vec3D> color;
	Animated<Vec3D> specular;
	Animated<short> opacity;

	Vec4D GetColor(uint32 uTime)
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



struct ModelAttachment
{
	int id;
	Vec3D pos;
	int bone;
	CModelData *model;

	void setup();
	void setupParticle();
};


class DLL_EXPORT CModelData:public iModelData
{
public:
	CModelData();
	~CModelData();
public:
	virtual void addAnimation(long timeStart, long timeEnd);
	virtual void setRenderPass(int nID, const std::string& strName,
		const std::string& strDiffuse, const std::string& strEmissive,
		const std::string& strSpecular, const std::string& strNormal,
		const std::string& strEnvironment, const std::string& strShader,
		int nChannel, bool bBlend, bool bAlphaTest, float fTexScaleU, float fTexScaleV);
	virtual	iLodMesh& getMesh(){return m_Mesh;}
	virtual iSkeleton& getSkeleton(){return m_Skeleton;}

	virtual bool LoadFile(const std::string& strFilename);
	virtual	bool loadMaterial(const std::string& strFilename,const std::string& strPath);
	virtual bool saveMaterial(const std::string& strFilename);
	virtual bool loadParticleMaterial(const std::string& strFilename,const std::string& strPath);
	virtual	bool loadParticleEmitters(const std::string& strFilename,const std::string& strPath);
	bool SaveFile(const std::string& strFilename);
	void InitSkins();
	void Init();
	int	GetOrder();
	bool isLoaded();
public:
	CLodMesh				m_Mesh;
 	CBoundMesh				m_BoundMesh;	// 包围盒
public: // 动画源
	CSkeleton		m_Skeleton;						// 骨架
	ModelCamera		m_Camera;
	int				*globalSequences;
	
	std::vector<ModelAnimation>	m_AnimList;				// 动画配表源
	std::vector<TexAnim>		m_TexAnims;				// 纹理动画源
	std::vector<ColorAnim>		m_ColorAnims;			// 颜色动画源
	std::vector<TransAnim>		m_TransAnims;			// 透明动画源
	std::vector<LightAnim>		m_LightAnims;			// 灯光动画源

	std::map<int,ModelRenderPass>m_mapPasses;				// 渲染过程集
	std::vector<CParticleEmitter>	m_setParticleEmitter;	// Particle Emitters
	//std::vector<CRibbonEmitter>	ribbons;			// 条带源
public:
	bool m_bHasAlphaTex;	// 是否有ALPHA透明纹理
public:
//	ModelType	m_ModelType;	// 模型类别
//public:
	std::vector<ModelAttachment> atts;
	int attLookup[40];
	int boneLookup[27];
public:
	int m_nOrder;
	bool bLoaded;
};

typedef struct{
	CModelPlugBase * pObj;
	HINSTANCE hIns;
}MODEL_PLUG_ST, * LPMODEL_PLUG_ST;

class CModelMgr: public CManager<CModelData>
{
public:
	CModelMgr();
	uint32 RegisterModel(const std::string& strFilename);
	CModelData* GetModel(uint32 uModelID);
	bool loadModel(CModelData& modelData,const std::string& strFilename);
private:
	bool loadPlugFromPath(const std::string& strPath);
	bool createPlug(const std::string& strFilename);

	std::vector<MODEL_PLUG_ST> m_arrPlugObj;
};

CModelMgr& GetModelMgr();
