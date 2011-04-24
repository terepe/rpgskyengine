#pragma once
#include "ModelData.h"
#include "Particle.h"
#include "RenderNode.h"

class CModelObject: public CRenderNode
{
public:
	CModelObject();
	~CModelObject();
public:
	CONST_GET_SET_VARIABLE	(BBox&, m_, BBox);
	virtual int		getType			() {return NODE_MODEL;}
	virtual void	frameMove		(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void	render			(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
public:
	void			Register		(const char* szFilename);
	bool			load			(const char* szFilename);
	bool			isCreated		(){return m_bCreated;}
	void			create			();
	int				getModelDataID	()const;
	CModelData*		getModelData	()const;
protected:
	int								m_nModelID;			// 模型ID
	CModelData*						m_pModelData;		// 模型源数据
protected:	// Attribute.
	bool							m_bCreated;
protected:
	BBox							m_BBox;
	std::string						m_strParentBoneName;
public:
	const std::string& getModelFilename(){return m_strModelFilename;}
protected:
	std::string m_strModelFilename;
};