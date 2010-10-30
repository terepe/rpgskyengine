#include "RenderNodel.h"

CRenderNodel::CRenderNodel()
{
}

CRenderNodel::~CRenderNodel()
{
}

void CRenderNodel::frameMove(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	for (std::map<std::string,CRenderNodel*>::const_iterator it=m_mapChildObj.begin();it!=m_mapChildObj.end();it++)
	{
		it->second->frameMove(mWorld, fTime, fElapsedTime);
	}
}

void CRenderNodel::render(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType)const
{
	for (std::map<std::string,CRenderNodel*>::const_iterator it=m_mapChildObj.begin();it!=m_mapChildObj.end();it++)
	{
		it->second->render(mWorld, eRenderType);
	}
}

CRenderNodel* CRenderNodel::getChild(const char* sName)
{
	std::map<std::string,CRenderNodel*>::iterator it = m_mapChildObj.find(sName);
	if (it != m_mapChildObj.end())
	{
		return it->second;
	}
	return NULL;
}

void CRenderNodel::delChild(const char* sName)
{
	std::map<std::string,CRenderNodel*>::iterator it = m_mapChildObj.find(sName);
	if (it != m_mapChildObj.end())
	{
		delete it->second;
		m_mapChildObj.erase(it);
	}
}

//void CRenderNodel::setChildPosition(const char* sName, const char* szBoneName)
//{
//	std::map<std::string,CRenderNodel*>::iterator it = m_mapChildObj.find(sName);
//	if (it != m_mapChildObj.end())
//	{
//		it->second.strBoneName = szBoneName;
//	}
//}