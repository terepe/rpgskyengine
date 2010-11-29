#pragma once
#include "3DMapObj.h"

class C3DMapSceneObj : public C3DMapObj
{
protected:
	__int64			m_ObjectID;
public:
	__int64			getObjectID	()				{return m_ObjectID;}
	void			setObjectID	(__int64 id)	{m_ObjectID = id;}
	virtual int		GetObjType	()				{return MAP_3DSIMPLE;}
	virtual void	renderFocus	()const;
};