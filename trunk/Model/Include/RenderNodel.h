#pragma once
#include "Material.h"
#include "Matrix.h"
#include <string>

class CRenderNodel
{
public:
	enum{NODEL_BASE,NODEL_PARTICLE,NODEL_MODEL,NODEL_SKINE};
	typedef std::map<std::string,CRenderNodel*>		MAP_RENDER_NODEL;
public:
	CRenderNodel();
	~CRenderNodel();
public:
	virtual	int				getType				() = 0;
	virtual void			frameMove			(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void			render				(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
	void					setParent			(CRenderNodel* pParent);
	CRenderNodel*			getParent			();
	void					addChild			(const char* szName, CRenderNodel* pChild);
	CRenderNodel*			getChild			(const char* szName);
	void					delChild			(const char* szName);
	void					setBindingBone		(const char* szBoneName);
	void					setChildBindingBone	(const char* szName, const char* szBoneName);

	virtual void			setPos				(const Vec3D& vPos)		{m_vPos=vPos;updateWorldMatrix();}
	virtual void			setRotate			(const Vec3D& vRotate)	{m_vRotate=vRotate;updateWorldMatrix();}
	virtual void			setScale			(const Vec3D& vScale)	{m_vScale=vScale;updateWorldMatrix();}

	virtual const Vec3D&	getPos				()const					{return m_vPos;}
	virtual const Vec3D&	getRotate			()const					{return m_vRotate;}
	virtual const Vec3D&	getScale			()const					{return m_vScale;}

	void					updateWorldMatrix	();
	const Matrix&			getWorldMatrix		()const					{return m_mWorld;}
protected:
	CRenderNodel*		m_pParent;
	MAP_RENDER_NODEL	m_mapChildObj;
	std::string			m_strBindingBoneName;
	int					m_nBindingBoneID;
	Matrix				m_mWorld;
	Vec3D				m_vPos;
	Vec3D				m_vRotate;
	Vec3D				m_vScale;
};
