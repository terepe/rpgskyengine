#include "RenderNodel.h"
#include <algorithm>

CRenderNodel::CRenderNodel()
:m_pParent(NULL)
,m_nBindingBoneID(-1)
,m_vPos(0.0f,0.0f,0.0f)
,m_vRotate(0.0f,0.0f,0.0f)
,m_vScale(1.0f,1.0f,1.0f)
{
	m_mWorldMatrix.unit();
}

CRenderNodel::~CRenderNodel()
{
	clearChild();
}

void CRenderNodel::frameMove(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	for (MAP_RENDER_NODEL::iterator it=m_mapChildObj.begin();it!=m_mapChildObj.end();it++)
	{
		(*it)->frameMove(mWorld, fTime, fElapsedTime);
	}
}

void CRenderNodel::render(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType)const
{
	for (MAP_RENDER_NODEL::const_iterator it=m_mapChildObj.begin();it!=m_mapChildObj.end();it++)
	{
		(*it)->render(mWorld, eRenderType);
	}
}

void CRenderNodel::addChild(CRenderNodel* pChild)
{
	pChild->setParent(this);
	m_mapChildObj.push_back(pChild);
}

CRenderNodel* CRenderNodel::getChild(const char* szName)
{
	for (MAP_RENDER_NODEL::iterator it=m_mapChildObj.begin();it!=m_mapChildObj.end();it++)
	{
		if (m_strName == szName)
		{
			return (*it);
		}
	}
	return NULL;
}

const CRenderNodel* CRenderNodel::getChild(const char* szName)const
{
	for (MAP_RENDER_NODEL::const_iterator it=m_mapChildObj.begin();it!=m_mapChildObj.end();it++)
	{
		if (m_strName == szName)
		{
			return (*it);
		}
	}
	return NULL;
}

bool CRenderNodel::removeChild(CRenderNodel* pChild)
{
	MAP_RENDER_NODEL::iterator it = std::find(m_mapChildObj.begin(),m_mapChildObj.end(),pChild);
	// ----
	if (it != m_mapChildObj.end())
	{
		m_mapChildObj.erase(it);
		return true;
	}
	return false;
}

void CRenderNodel::delChild(CRenderNodel* pChild)
{
	if (removeChild(pChild))
	{
		delete pChild;
	}
}

void CRenderNodel::clearChild()
{
	for (MAP_RENDER_NODEL::const_iterator it=m_mapChildObj.begin();it!=m_mapChildObj.end();it++)
		delete (*it);
	// ----
	m_mapChildObj.clear();
}

void CRenderNodel::setChildBindingBone(const char* szName, const char* szBoneName)
{
	CRenderNodel* pRenderNodel = getChild(szName);
	// ----
	if (pRenderNodel)
	{
		pRenderNodel->setBindingBoneName(szBoneName);
	}
}

void CRenderNodel::updateWorldMatrix()
{
	Matrix mTrans;
	Matrix mRotate;
	Matrix mScale;
	mTrans.translation(getPos());
	mRotate.rotate(getRotate());
	mScale.scale(getScale());
	m_mWorldMatrix = mTrans*mRotate*mScale;
}