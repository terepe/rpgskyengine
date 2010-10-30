#pragma once
#include "Material.h"
#include "Matrix.h"
#include <string>

class CRenderNodel
{
public:
	CRenderNodel();
	~CRenderNodel();
public:
	virtual void frameMove(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void render(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
	CRenderNodel* getChild(const char* sName);
	void delChild(const char* sName);
protected:
	std::map<std::string,CRenderNodel*>	m_mapChildObj;
};
