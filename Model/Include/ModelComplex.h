#pragma once
#include "ModelObject.h"

class CModelComplex :public CModelObject
{
public:
	CModelComplex();
	~CModelComplex();
public:
	virtual void OnFrameMove(float fElapsedTime);
	virtual void renderMesh(E_MATERIAL_RENDER_TYPE eModelRenderType=MATERIAL_NORMAL)const;
	virtual void renderParticles(E_MATERIAL_RENDER_TYPE eParticleRenderType=MATERIAL_NORMAL)const;
	virtual void drawSkeleton(CTextRender* pTextRender)const;
	void loadSkinModel(const char* szName,const char* szFilename);
	void loadChildModel(const char* szBoneName,const char* szFilename);
	void delChildModel(const char* szBoneName);
	void renderChildMesh(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eModelRenderType)const;
public:
	std::map<std::string,CModelObject*>	m_mapSkinModel;
	std::map<std::string,CModelObject*>	m_mapChildModel;
};