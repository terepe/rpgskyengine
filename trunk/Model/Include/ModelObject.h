#pragma once
#include "ModelData.h"
#include "AnimMgr.h"

class DLL_EXPORT CModelObject
{
public:
	CModelObject();
	~CModelObject();
public:
	void SkinAnim();	// 动画更新
	virtual void OnFrameMove(float fElapsedTime);
public:
	void SetLOD(uint32 uLodID);		// 设置LodID
	void SetSkin(uint32 uSkinID);	// 设置皮肤
	void SetAnim(uint32 uAnimID);	// 设置动画ID
	void SetLightMap(const std::string& strFilename);	// SetLightMap
	void SetMaterial(const Vec4D& vAmbient, const Vec4D& vDiffuse);
public:
	void Register(const std::string& strFilename);
	bool load(const std::string& strFilename);
	bool Prepare()const;
	bool PassBegin(ModelRenderPass& pass)const;	// 渲染步骤
	void PassEnd()const;

	virtual void DrawSubsHasNoAlphaTex()const;
	bool PrepareEdge()const;
	void FinishEdge()const;
	virtual void DrawModelEdge()const;
	virtual void renderMesh(E_MATERIAL_RENDER_TYPE eModelRenderType=MATERIAL_RENDER_NORMAL)const;
	virtual void renderParticles(E_MATERIAL_RENDER_TYPE eParticleRenderType=MATERIAL_RENDER_NORMAL)const;
	virtual void render(E_MATERIAL_RENDER_TYPE eModelRenderType=MATERIAL_RENDER_NORMAL,E_MATERIAL_RENDER_TYPE eParticleRenderType=MATERIAL_RENDER_NORMAL)const;

	virtual void draw()const;

	virtual void DrawBones()const;		// 画骨骼线

	void updateEmitters(const Matrix& mWorld, float fElapsedTime);	// 发射器更新
	void SetLodLevel(int nLodLevel);	// 设置LOD等级

	//void setupAtt(int id);
	//void setupAtt2(int id);
	//void AnimReset() { 
	//	m_bAnimCalc = false; 
	//}
	void Animate(int anim);
	void CalcBones(int time);

	bool isCreated();
	void create();
public: // 模型数据源
	int m_nModelID;		// 模型ID
	CModelData*		m_pModelData;				// 模型源数据
	CSkeleton*		m_pSkeleton;			// 骨架
	CLodMesh*		m_pMesh;
public: // 模型动画计算数据
	AnimManager*			m_AnimMgr;	// 动作管理器
	CHardwareVertexBuffer*	m_pVB;	// 顶点缓冲
	std::vector<CBone>		m_Bones;	// 骨骼
	std::vector<CParticleGroup>	m_setParticleGroup;	// 粒子ID集合
	int m_nCurrentAnimID;	// 当前动作ID
	int m_nAnim;			// 当前动作ID
	int m_nAnimTime;		// 动作时间帧
	float m_fTrans;			// 自身透明度
	float m_fAlpha;			// 过度透明
public:// 属性
	// 设置显示部件
	void ShowGeoset(uint32 uID, bool bShow = true){ if (uID<m_setShowSubset.size()) m_setShowSubset[uID] = bShow;}
	// 设置显示粒子
	void ShowParticle(uint32 uID, bool bShow = true){ if (uID<m_setShowParticle.size()) m_setShowParticle[uID] = bShow;}
protected: 
	ModelType m_ModelType;		// 模型类别
	float	m_fRad;				// 半径
	uint32	m_uLodID;			// 当前LodID
	int		m_nLodLevel;		// LOD 等级
	uint32	m_uSkinID;			// 皮肤I
	OBJID	m_idLightMapTex;	//
	bool	m_bLightmap;
	std::vector<bool> m_setShowParticle;			// 显示粒子

	bool	m_bCreated;
protected:
	Vec4D m_vAmbient;
	Vec4D m_vDiffuse;
	bool m_bCartoonRender;	// 描边
	bool m_bRenderEdge;		// 渲染卡通边缘
	bool m_bRenderCartoon;	// 渲染卡通边缘

	//LightAnim		*lights;

	//RibbonEmitter	*ribbons;

	//void lightsOn(int lbase);
	//void lightsOff(int lbase);
public:
	const std::string& getModelFilename(){return m_strModelFilename;}
protected:
	std::string m_strModelFilename;
	std::vector<bool> m_setShowSubset;	// 显示部件
};