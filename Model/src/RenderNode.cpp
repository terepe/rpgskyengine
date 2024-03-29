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
	clearChildren();
}

void CRenderNode::frameMove(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	BBox bbox;
	FOR_IN(LIST_RENDER_NODE,it,m_mapChildObj)
	{
		(*it)->frameMove(mWorld, fTime, fElapsedTime);
		// ----
		// # Update BBox
		// ----
		bbox += (*it)->getBBox();
	}
	m_BBox = bbox;
}

void CRenderNode::render(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType)const
{
	CONST_FOR_IN(LIST_RENDER_NODE,it,m_mapChildObj)
	{
		(*it)->render(mWorld, eRenderType);
	}
}

void CRenderNode::addChild(iRenderNode* pChild)
{
	((CRenderNode*)pChild)->setParent(this);
	m_mapChildObj.push_back((CRenderNode*)pChild);
}

iRenderNode* CRenderNode::getChild(const char* szName)
{
	FOR_IN(LIST_RENDER_NODE,it,m_mapChildObj)
	{
		if (strcmp( (*it)->getName(), szName ) == 0 )
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
		if (strcmp( (*it)->getName(), szName ) == 0 )
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

bool CRenderNode::contain(const CRenderNode* pChild)const
{
	LIST_RENDER_NODE::const_iterator it = std::find(m_mapChildObj.begin(),m_mapChildObj.end(),pChild);
	if (it != m_mapChildObj.end())
	{
		return true;
	}
	return false;
}

void CRenderNode::clearChildren()
{
	CONST_FOR_IN(LIST_RENDER_NODE,it,m_mapChildObj)
		delete (*it);
	// ----
	removeChildren();
}

void CRenderNode::removeChildren()
{
	m_mapChildObj.clear();
}

CRenderNode* CRenderNode::intersect(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax)
{
	if(!intersectSelf(vRayPos,vRayDir,tmin,tmax))
	{
		return NULL;
	}
	CONST_FOR_IN(LIST_RENDER_NODE,it,m_mapChildObj)
 	{
		CRenderNode* pRenderNode = (*it)->intersect(vRayPos,vRayDir,tmin,tmax);
		if(pRenderNode)
		{
			return pRenderNode;
		}
	}
	return this;
}

void CRenderNode::setChildBindingBone(const char* szName, const char* szBoneName)
{
	CRenderNode* pRenderNodel = (CRenderNode*)getChild(szName);
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

void CRenderNode::updateWorldBBox()
{
	m_WorldBBox = m_LocalBBox;
	Matrix mRotate;
	mRotate.rotate(getRotate());

	Vec3D vHalfExtents	= (m_WorldBBox.vMax-m_WorldBBox.vMin)*0.5f*getScale();
	Vec3D vCenter		= (m_WorldBBox.vMax+m_WorldBBox.vMin)*0.5f*getScale();
	vCenter				= mRotate*vCenter+getPos();

	Vec3D vExtent;
	vExtent.x			= Vec3D(abs(mRotate._11),abs(mRotate._12),abs(mRotate._13)).dot(vHalfExtents);
	vExtent.y			= Vec3D(abs(mRotate._21),abs(mRotate._22),abs(mRotate._23)).dot(vHalfExtents);
	vExtent.z			= Vec3D(abs(mRotate._31),abs(mRotate._32),abs(mRotate._33)).dot(vHalfExtents);

	m_WorldBBox.vMin	= vCenter - vExtent;
	m_WorldBBox.vMax	= vCenter + vExtent;
}