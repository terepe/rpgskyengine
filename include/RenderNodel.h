#pragma once
#include "Material.h"
#include "Matrix.h"
#include <string>
#include <list>

//----
// # Helper macros to build member functions that access member variables
//----
#define SET_VARIABLE(x,y,z)				inline void set##y(x t)	{ z = t; };
#define GET_VARIABLE(x,y,z)				inline x get##y()		{ return z; };
#define GET_SET_VARIABLE(x,y,z)			SET_VARIABLE(x,z,y##z) GET_VARIABLE(x,z,y##z)
//----
#define CONST_SET_VARIABLE(x,y,z)		inline void set##y(const x t)	{ z = t; };
#define CONST_GET_VARIABLE(x,y,z)		inline const x get##y()const	{ return z; };
#define CONST_GET_SET_VARIABLE(x,y,z)	CONST_SET_VARIABLE(x,z,y##z) CONST_GET_VARIABLE(x,z,y##z)
//----
#define GET_SET_VARIABLE_STRING(y)		CONST_SET_VARIABLE(char*,y,m_str##y) CONST_GET_VARIABLE(char*,y,m_str##y.c_str())
//----

class CRenderNodel
{
public:
	enum{NODEL_BASE,NODEL_PARTICLE,NODEL_MODEL,NODEL_SKINE};
	typedef std::list<CRenderNodel*>		MAP_RENDER_NODEL;
public:
	CRenderNodel();
	~CRenderNodel();
public:
	GET_SET_VARIABLE		(CRenderNodel*,	m_p,Parent);
	CONST_GET_SET_VARIABLE	(Vec3D&,		m_v,Pos);
	CONST_GET_SET_VARIABLE	(Vec3D&,		m_v,Rotate);
	CONST_GET_SET_VARIABLE	(Vec3D&,		m_v,Scale);
	CONST_GET_SET_VARIABLE	(Matrix&,		m_m,WorldMatrix);
	GET_SET_VARIABLE_STRING	(Name);
	GET_SET_VARIABLE_STRING	(BindingBoneName);
	//----
	virtual	int				getType				() = 0;
	virtual void			frameMove			(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void			render				(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
	void					addChild			(CRenderNodel* pChild);
	CRenderNodel*			getChild			(const char* szName);
	const CRenderNodel*		getChild			(const char* szName)const;
	bool					removeChild			(CRenderNodel* pChild);
	void					delChild			(CRenderNodel* pChild);
	void					clearChild			();
	void					setChildBindingBone	(const char* szName, const char* szBoneName);
	//----
	void					updateWorldMatrix	();
protected:
	CRenderNodel*			m_pParent;
	MAP_RENDER_NODEL		m_mapChildObj;
	std::string				m_strName;
	std::string				m_strBindingBoneName;
	int						m_nBindingBoneID;
	Matrix					m_mWorldMatrix;
	Vec3D					m_vPos;
	Vec3D					m_vRotate;
	Vec3D					m_vScale;
};
