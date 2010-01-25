#pragma once
#include "3DMapObj.h"


class DLL_EXPORT C3DMapSceneObj : public C3DMapObj
{
public:
	size_t getObjectID(){return m_ObjectID;}
	void setObjectID(size_t id){m_ObjectID = id;}
	virtual int  GetObjType(){return MAP_3DSIMPLE;}
	bool Create(const string& strFilename,Vec3D vPos,Vec3D vRotate,float fScale);
	static C3DMapSceneObj* CreateNew(const string& strFilename,Vec3D vPos,Vec3D vRotate,float fScale);
	virtual void renderFocus()const;
protected:
	size_t	m_ObjectID;
};