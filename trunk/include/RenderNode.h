#pragma once
#include "Material.h"
#include "Matrix.h"
#include "frustum.h"
#include <string>
#include <list>
#include "common.h"
#include "InterfaceModel.h"

class CRenderNode;
typedef std::list<CRenderNode*>		LIST_RENDER_NODE;
//----

template<class T>
class CNode
{

};

class CObjectPosition
{
public:
	CObjectPosition()
		:m_vPos(0.0f,0.0f,0.0f)
		,m_vRotate(0.0f,0.0f,0.0f)
		,m_vScale(1.0f,1.0f,1.0f)
	{
		m_mWorldMatrix.unit();
	}
	//----
	CONST_GET_SET_VARIABLE	(Vec3D&,		m_v,Pos);
	CONST_GET_SET_VARIABLE	(Vec3D&,		m_v,Rotate);
	CONST_GET_SET_VARIABLE	(Vec3D&,		m_v,Scale);
	CONST_GET_SET_VARIABLE	(BBox&,			m_,LocalBBox);
	CONST_GET_SET_VARIABLE	(BBox&,			m_,WorldBBox);
	CONST_GET_SET_VARIABLE	(Matrix&,		m_m,WorldMatrix);
	//----
	void					updateWorldBBox		();
	void					updateWorldMatrix	();
protected:
	Vec3D					m_vPos;
	Vec3D					m_vRotate;
	Vec3D					m_vScale;
	BBox					m_LocalBBox;
	BBox					m_WorldBBox;
	Matrix					m_mWorldMatrix;
};

class CRenderNode:public CObjectPosition,public iRenderNode
{
public:
	enum{NODE_BASE,NODE_PARTICLE,NODE_SKELETON,NODE_MODEL,NODE_SKINE};
public:
	CRenderNode();
	~CRenderNode();
public:
	GET_SET_VARIABLE		(CRenderNode*,	m_p,Parent);
	//----
	GET_VARIABLE			(LIST_RENDER_NODE&,ChildObj,m_mapChildObj);
	CONST_GET_VARIABLE		(LIST_RENDER_NODE&,ChildObj,m_mapChildObj);
	CONST_GET_SET_VARIABLE	(BBox&,m_,BBox);
	//----
	GET_SET_VARIABLE_STRING	(Name);
	GET_SET_VARIABLE_STRING	(BindingBoneName);
	//----
	virtual	int				getType				() = 0;
	virtual void			frameMove			(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void			render				(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
	iRenderNode*			getChild			(const char* szName);
	const CRenderNode*		getChild			(const char* szName)const;
	virtual void			addChild			(iRenderNode* pChild);
	virtual bool			removeChild			(CRenderNode* pChild);
	virtual bool			delChild			(CRenderNode* pChild);
	virtual bool			contain				(const CRenderNode* pChild)const;
	virtual void			clearChildren		();
	virtual void			removeChildren		();
	virtual bool			intersectSelf		(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax)const{return false;}
	virtual CRenderNode*	intersect			(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax);
	
	//----
	void					setChildBindingBone	(const char* szName, const char* szBoneName);
protected:
	CRenderNode*			m_pParent;
	LIST_RENDER_NODE		m_mapChildObj;
	//----
	std::string				m_strName;
	std::string				m_strBindingBoneName;
	int						m_nBindingBoneID;
	BBox					m_BBox;
};
