#pragma once
#include "MapObj.h"
#include "ModelComplex.h"

class C3DMapObj : public CMapObj, public CModelComplex
{
public:
	C3DMapObj();
	~C3DMapObj();
protected:
	Pos2D m_posCell;
public:
	virtual void GetCellPos( Pos2D& posCell );
	virtual void SetCellPos( Pos2D& posCell );

	virtual void Process(void* pInfo){;}
	virtual int  GetObjType(){return MAP_3DOBJ;}
	virtual bool IsFocus(){return false;}

	virtual Matrix getWorldMatrix()const;
	virtual Matrix getShadowMatrix(const Vec3D& vLight,float fHeight)const;
	virtual BBox getBBox()const;
	virtual void render(int flag)const;
	virtual void renderShadow(const Vec3D& vLight,float fHeight)const;
	virtual void renderFocus()const;
	virtual void renderDebug()const;
	virtual void OnFrameMove(float fElapsedTime);

	virtual bool intersect(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax);
	virtual int getOrder();
	bool isSkinMesh();

	void renderFocus(Color32 color)const;
protected:
};