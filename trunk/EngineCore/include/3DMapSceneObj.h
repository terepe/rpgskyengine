#pragma once
#include "3DMapObj.h"

class DLL_EXPORT C3DMapSceneObj : public C3DMapObj
{
public:
	int64 getObjectID(){return m_ObjectID;}
	void setObjectID(int64 id){m_ObjectID = id;}
	virtual int  GetObjType(){return MAP_3DSIMPLE;}
	bool Create(const string& strFilename,const Vec3D& vPos,const Vec3D& vRotate,const Vec3D& vScale);
	static C3DMapSceneObj* CreateNew(const string& strFilename,const Vec3D& vPos,const Vec3D& vRotate,const Vec3D& vScale);
	virtual void renderFocus()const;
protected:
	int64	m_ObjectID;
};