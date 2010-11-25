#include "RenderNode.h"
#include <algorithm>

CRenderNode::CRenderNode()
:m_pParent(NULL)
,m_nBindingBoneID(-1)
,m_vPos(0.0f,0.0f,0.0f)
,m_vRotate(0.0f,0.0f,0.0f)
,m_vScale(1.0f,1.0f,1.0f)
{
	m_mWorldMatrix.unit();
}

CRenderNode::~CRenderNode()
{
	clearChild();
}

void CRenderNode::frameMove(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	FOR_IN(LIST_RENDER_NODE,it,m_mapChildObj)
	{
		(*it)->frameMove(mWorld, fTime, fElapsedTime);
	}
}

void CRenderNode::render(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType)const
{
	CONST_FOR_IN(LIST_RENDER_NODE,it,m_mapChildObj)
	{
		(*it)->render(mWorld, eRenderType);
	}
}

void CRenderNode::addChild(CRenderNode* pChild)
{
	pChild->setParent(this);
	m_mapChildObj.push_back(pChild);
}

CRenderNode* CRenderNode::getChild(const char* szName)
{
	FOR_IN(LIST_RENDER_NODE,it,m_mapChildObj)
	{
		if (m_strName == szName)
		{
			return (*it);
		}
	}
	return NULL;
}

const CRenderNode* CRenderNode::getChild(const char* szName)const
{
	CONST_FOR_IN(LIST_RENDER_NODE,it,m_mapChildObj)
	{
		if (m_strName == szName)
		{
			return (*it);
		}
	}
	return NULL;
}

bool CRenderNode::removeChild(CRenderNode* pChild)
{
	LIST_RENDER_NODE::iterator it = std::find(m_mapChildObj.begin(),m_mapChildObj.end(),pChild);
	// ----
	if (it != m_mapChildObj.end())
	{
		m_mapChildObj.erase(it);
		return true;
	}
	return false;
}

bool CRenderNode::delChild(CRenderNode* pChild)
{
	if (removeChild(pChild))
	{
		delete pChild;
		return true;
	}
	return false;
}

void CRenderNode::clearChild()
{
	for (LIST_RENDER_NODE::const_iterator it=m_mapChildObj.begin();it!=m_mapChildObj.end();it++)
		delete (*it);
	// ----
	removeAllChild();
}

void CRenderNode::removeAllChild()
{
	m_mapChildObj.clear();
}

void CRenderNode::setChildBindingBone(const char* szName, const char* szBoneName)
{
	CRenderNode* pRenderNodel = getChild(szName);
	// ----
	if (pRenderNodel)
	{
		pRenderNodel->setBindingBoneName(szBoneName);
	}
}

void CRenderNode::updateWorldMatrix()
{
	Matrix mTrans;
	Matrix mRotate;
	Matrix mScale;
	mTrans.translation(getPos());
	mRotate.rotate(getRotate());
	mScale.scale(getScale());
	m_mWorldMatrix = mTrans*mRotate*mScale;
}