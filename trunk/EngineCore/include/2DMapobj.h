#pragma once
#include "MapObj.h"

class C2DMapObj : public CMapObj
{
public:
	C2DMapObj();
	~C2DMapObj();
protected:
	Pos2D m_posCell;
// for mapobj
public:
	void GetWorldPos( Vec3D& vWorldPos );

	void GetCellPos( Pos2D& posCell );
	void SetCellPos( Pos2D& posCell );

	virtual void Show(void* pInfo){}
	virtual void Process(void* pInfo){}

	virtual int GetObjType   (){return MAP_NONE;}
	virtual BOOL IsFocus(){return false;}
};