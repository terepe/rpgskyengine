#pragma once
#include "ModelObject.h"

class CModelComplex :public CModelObject
{
public:
	CModelComplex();
	~CModelComplex();
public:
	virtual void OnFrameMove(float fElapsedTime);
	virtual void drawMesh(E_MATERIAL_RENDER_TYPE eModelRenderType=MATERIAL_RENDER_NORMAL)const;
	virtual void drawMeshWithTexture(E_MATERIAL_RENDER_TYPE eModelRenderType=MATERIAL_RENDER_NORMAL)const;
	virtual void renderMesh(E_MATERIAL_RENDER_TYPE eModelRenderType=MATERIAL_RENDER_NORMAL)const;
	virtual void renderParticles(E_MATERIAL_RENDER_TYPE eParticleRenderType=MATERIAL_RENDER_NORMAL)const;
	virtual void drawSkeleton()const;
	void loadSkinModel(const char* szName,const char* szFilename);
	void loadChildModel(const char* szBoneName,const char* szFilename);
public:
	std::map<std::string,CModelObject*>	m_mapSkinModel;
	std::map<std::string,CModelObject*>	m_mapChildModel;
};