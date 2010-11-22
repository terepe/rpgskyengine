#pragma once
#include "3DMapObj.h"

class C3DMapSceneObj : public C3DMapObj
{
public:
	__int64					getObjectID	(){return m_ObjectID;}
	void					setObjectID	(__int64 id){m_ObjectID = id;}
	virtual int				GetObjType	(){return MAP_3DSIMPLE;}
	bool					Create		(const char* szFilename,const Vec3D& vPos,const Vec3D& vRotate,const Vec3D& vScale);
	static C3DMapSceneObj*	CreateNew	(const char* szFilename,const Vec3D& vPos,const Vec3D& vRotate,const Vec3D& vScale);
	virtual void			renderFocus	()const;
protected:
	__int64	m_ObjectID;
};