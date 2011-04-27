#pragma once
#include "ModelData.h"
#include "SkeletonNode.h"

class CModelObject: public CSkeletonNode
{
public:
	CModelObject();
	~CModelObject();
public:
	CONST_GET_SET_VARIABLE	(BBox&, m_, BBox);
	CONST_GET_SET_VARIABLE	(std::string&, m_str, ModelFilename);
	CONST_GET_SET_VARIABLE	(int, m_n, ModelDataID);
	//CONST_GET_SET_VARIABLE	(CModelData*, m_p, ModelData);
	virtual void	frameMove		(const Matrix& mWorld, double fTime, float fElapsedTime);
public:
	void			Register		(const char* szFilename);
	bool			load			(const char* szFilename);
	bool			isCreated		(){return m_bCreated;}
	void			create			();
protected:
	int				m_nModelDataID;		// 模型ID
	CModelData*		m_pModelData;		// 模型源数据
	bool			m_bCreated;
	BBox			m_BBox;
	std::string		m_strModelFilename;
};