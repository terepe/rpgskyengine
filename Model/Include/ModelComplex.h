#pragma once
#include "ModelObject.h"

class DLL_EXPORT CModelComplex :public CModelObject
{
public:
	CModelComplex();
	~CModelComplex();
public:
	virtual void OnFrameMove(float fElapsedTime);
	virtual void DrawSubsHasNoAlphaTex()const;
	virtual void DrawModelEdge()const;
	virtual void render(E_MODEL_RENDER_TYPE eModelRenderType=MODEL_RENDER_NORMAL)const;
	virtual void draw()const;
	virtual void DrawBones()const;
	void loadSkinModel(const std::string& strName,const std::string& strFilename);
public:
	std::map<std::string,CModelObject*>	m_mapSkinModel;
	std::map<std::string,CModelObject*>	m_mapChildModel;
};