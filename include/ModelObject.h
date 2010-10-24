#pragma once
#include "ModelData.h"
#include "AnimMgr.h"
#include "Particle.h"

class CModelObject
{
public:
	CModelObject();
	~CModelObject();
public:
	void SkinAnim();	// ��������
	virtual void OnFrameMove(float fElapsedTime);
public:
	void SetLOD(unsigned long uLodID);		// ����LodID
	void SetSkin(unsigned long uSkinID);	// ����Ƥ��
	void setAnim(int nID);
	void setAnim(const char* szAnimName);
	void SetLightMap(const std::string& strFilename);	// SetLightMap
public:
	void Register(const std::string& strFilename);
	bool load(const std::string& strFilename);
	bool Prepare()const;

	virtual void renderMesh			(E_MATERIAL_RENDER_TYPE eModelRenderType=MATERIAL_NORMAL)const;
	virtual void renderParticles	(E_MATERIAL_RENDER_TYPE eParticleRenderType=MATERIAL_NORMAL)const;
	virtual void render				(E_MATERIAL_RENDER_TYPE eMeshRenderType=MATERIAL_NORMAL,E_MATERIAL_RENDER_TYPE eParticleRenderType=MATERIAL_NORMAL)const;

	virtual void drawSkeleton(CTextRender* pTextRender)const;

	void updateEmitters(const Matrix& mWorld, float fElapsedTime);
	void SetLodLevel(int nLodLevel);

	void Animate(const std::string& strAnimName);
	void CalcBones(const std::string& strAnim, int time);

	bool isCreated();
	void create();

	void setBBox(const BBox& box);
	const BBox& getBBox()const;
	int getModelDataID()const;
	CModelData* getModelData()const;
protected:
	int m_nModelID;									// ģ��ID
	CModelData*		m_pModelData;					// ģ��Դ����
public:												// All the anim data.
	SingleAnimNode				m_AnimMgr;			// ����������
	CHardwareVertexBuffer*		m_pVB;				// ���㻺��
	std::vector<Matrix>			m_setBonesMatrix;	// ��������
	std::vector<CParticleGroup>	m_setParticleGroup;	// ����ID����
	std::string	m_strAnimName;						// Current Name Of Animate
	int		m_nAnimTime;							// ����ʱ��֡
	float	m_fTrans;								// ����͸����
	float	m_fAlpha;								// ����͸��
protected:											// Attribute.
	float	m_fRad;									// Radius
	unsigned long	m_uLodLevel;							// Current Lod Level
	unsigned long	m_uSkinID;								// Current Skin ID
	unsigned long	m_uLightMapTex;						//
	bool	m_bLightmap;
	bool	m_bCreated;
protected:
	//LightAnim		*lights;
	//RibbonEmitter	*ribbons;
	//void lightsOn(int lbase);
	//void lightsOff(int lbase);
	BBox m_BBox;
public:
	const std::string& getModelFilename(){return m_strModelFilename;}
protected:
	std::string m_strModelFilename;
};