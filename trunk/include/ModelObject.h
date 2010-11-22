#pragma once
#include "ModelData.h"
#include "AnimMgr.h"
#include "Particle.h"
#include "RenderNodel.h"

class CModelObject: public CRenderNodel
{
public:
	CModelObject();
	~CModelObject();
public:
	virtual int		getType			() {return NODEL_MODEL;}
	void			SkinAnim		();	// 动画更新
	virtual void	frameMove		(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void	animate			(float fElapsedTime);
	virtual void	render			(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
public:
	void			SetLOD			(unsigned long uLodID);		// 设置LodID
	void			SetSkin			(unsigned long uSkinID);	// 设置皮肤
	void			setAnim			(int nID);
	void			setAnim			(const char* szAnimName);
	void			SetLightMap		(const char* szFilename);	// SetLightMap
public:
	void			Register		(const char* szFilename);
	bool			load			(const char* szFilename);
	//void			loadSkinModel	(const char* szName,const char* szFilename);
	//void			loadChildModel	(const char* szName, const char* szBoneName, const char* szFilename);
	bool			Prepare			()const;

	virtual void	drawSkeleton	(CTextRender* pTextRender)const;

	void			SetLodLevel		(int nLodLevel);

	void			Animate			(const char* szAnimName);
	void			CalcBones		(const char* szAnim, int time);

	bool			isCreated		();
	void			create			();

	void			setBBox			(const BBox& box);
	const BBox&		getBBox			()const;
	int				getModelDataID	()const;
	CModelData*		getModelData	()const;
protected:
	int								m_nModelID;			// 模型ID
	CModelData*						m_pModelData;		// 模型源数据
public:	// All the anim data.
	SingleAnimNode					m_AnimMgr;			// 动作管理器
	CHardwareVertexBuffer*			m_pVB;				// 顶点缓冲
	std::vector<Matrix>				m_setBonesMatrix;	// 骨骼矩阵
	std::string						m_strAnimName;		// Current Name Of Animate
	int								m_nAnimTime;		// 动作时间帧
	float							m_fTrans;			// 自身透明度
	float							m_fAlpha;			// 过度透明
protected:	// Attribute.
	float							m_fRad;				// Radius
	unsigned long					m_uLodLevel;		// Current Lod Level
	unsigned long					m_uSkinID;			// Current Skin ID
	unsigned long					m_uLightMapTex;		//
	bool							m_bLightmap;
	bool							m_bCreated;
protected:
	//LightAnim		*lights;
	//RibbonEmitter	*ribbons;
	//void lightsOn(int lbase);
	//void lightsOff(int lbase);
	BBox m_BBox;
	std::string						m_strParentBoneName;
public:
	const std::string& getModelFilename(){return m_strModelFilename;}
protected:
	std::string m_strModelFilename;
};