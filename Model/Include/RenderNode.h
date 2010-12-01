#pragma once
#include "Material.h"
#include "Matrix.h"
#include "frustum.h"
#include <string>
#include <list>

//----
// # Helper macros to build member functions that access member variables
//----
#define SET_VARIABLE(x,y,z)				void set##y(x t)	{ z = t; };
#define GET_VARIABLE(x,y,z)				x get##y()		{ return z; };
#define GET_SET_VARIABLE(x,y,z)			SET_VARIABLE(x,z,y##z) GET_VARIABLE(x,z,y##z)
//----
#define CONST_SET_VARIABLE(x,y,z)		void set##y(const x t)	{ z = t; };
#define CONST_GET_VARIABLE(x,y,z)		const x get##y()const	{ return z; };
#define CONST_GET_SET_VARIABLE(x,y,z)	CONST_SET_VARIABLE(x,z,y##z) CONST_GET_VARIABLE(x,z,y##z)
//----
#define GET_SET_VARIABLE_STRING(y)		CONST_SET_VARIABLE(char*,y,m_str##y) CONST_GET_VARIABLE(char*,y,m_str##y.c_str())
//----
#define FOR_IN(t,i,c)					for (t::iterator i = c.begin();i != c.end(); ++i)
#define CONST_FOR_IN(t,i,c)				for (t::const_iterator i = c.begin();i != c.end(); ++i)
//----

class CRenderNode;
typedef std::list<CRenderNode*>		LIST_RENDER_NODE;
//----

class CRenderNode
{
public:
	enum{NODE_BASE,NODE_PARTICLE,NODE_MODEL,NODE_SKINE};
public:
	CRenderNode();
	~CRenderNode();
public:
	GET_SET_VARIABLE		(CRenderNode*,	m_p,Parent);
	CONST_GET_SET_VARIABLE	(Vec3D&,		m_v,Pos);
	CONST_GET_SET_VARIABLE	(Vec3D&,		m_v,Rotate);
	CONST_GET_SET_VARIABLE	(Vec3D&,		m_v,Scale);
	CONST_GET_SET_VARIABLE	(Matrix&,		m_m,WorldMatrix);
	CONST_GET_SET_VARIABLE	(BBox&,			m_,WorldBBox);
	GET_SET_VARIABLE_STRING	(Name);
	GET_SET_VARIABLE_STRING	(BindingBoneName);
	//----
	GET_VARIABLE			(LIST_RENDER_NODE&,ChildObj,m_mapChildObj);
	CONST_GET_VARIABLE		(LIST_RENDER_NODE&,ChildObj,m_mapChildObj);
	virtual	int				getType				() = 0;
	virtual void			frameMove			(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void			render				(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
	CRenderNode*			getChild			(const char* szName);
	const CRenderNode*		getChild			(const char* szName)const;
	virtual void			addChild			(CRenderNode* pChild);
	virtual bool			removeChild			(CRenderNode* pChild);
	virtual bool			delChild			(CRenderNode* pChild);
	virtual void			clearChild			();
	virtual void			removeAllChild		();
	//----
	void					setChildBindingBone	(const char* szName, const char* szBoneName);
	//----
	void					updateWorldMatrix	();
	void					updateWorldBBox		();
	//----
protected:
	CRenderNode*			m_pParent;
	LIST_RENDER_NODE		m_mapChildObj;
	std::string				m_strName;
	std::string				m_strBindingBoneName;
	int						m_nBindingBoneID;
	Matrix					m_mWorldMatrix;
	Vec3D					m_vPos;
	Vec3D					m_vRotate;
	Vec3D					m_vScale;
	BBox					m_WorldBBox;
};
