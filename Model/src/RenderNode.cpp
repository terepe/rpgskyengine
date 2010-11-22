#include "RenderNodel.h"

CRenderNodel::CRenderNodel()
:m_pParent(NULL)
,m_nBindingBoneID(-1)
,m_vPos(0.0f,0.0f,0.0f)
,m_vRotate(0.0f,0.0f,0.0f)
,m_vScale(1.0f,1.0f,1.0f)
{
	m_mWorld.unit();
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

void CRenderNodel::setParent(CRenderNodel* pParent)
{
	m_pParent = pParent;
}

CRenderNodel* CRenderNodel::getParent()
{
	return m_pParent;
}

void CRenderNodel::addChild(const char* szName, CRenderNodel* pChild)
{
	pChild->setParent(this);
	m_mapChildObj[szName] = pChild;
}

CRenderNodel* CRenderNodel::getChild(const char* szName)
{
	std::map<std::string,CRenderNodel*>::iterator it = m_mapChildObj.find(szName);
	// ----
	if (it != m_mapChildObj.end())
	{
		return it->second;
	}
	return NULL;
}

void CRenderNodel::delChild(const char* szName)
{
	std::map<std::string,CRenderNodel*>::iterator it = m_mapChildObj.find(szName);
	// ----
	if (it != m_mapChildObj.end())
	{
		delete it->second;
		m_mapChildObj.erase(it);
	}
}

void CRenderNodel::setBindingBone(const char* szBoneName)
{
	m_strBindingBoneName = szBoneName;
}

void CRenderNodel::setChildBindingBone(const char* szName, const char* szBoneName)
{
	CRenderNodel* pRenderNodel = getChild(szName);
	// ----
	if (pRenderNodel)
	{
		pRenderNodel->setBindingBone(szBoneName);
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
	m_mWorld = mTrans*mRotate*mScale;
}