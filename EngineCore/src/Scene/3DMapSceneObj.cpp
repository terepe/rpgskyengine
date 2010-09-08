#include "3DMapSceneObj.h"

bool C3DMapSceneObj::Create(const char* szFilename,const Vec3D& vPos,const Vec3D& vRotate,const Vec3D& vScale)
{
	CModelObject::Register(szFilename);
	setPos(vPos);
	setRotate(vRotate);
	setScale(vScale);
	//Vec4D vColor = m_pTerrain->GetData().GetColor(Vec2D(vPos.x,vPos.z));
	//vColor.w=1.0f;
	//pObject->SetMaterial(vColor*0.5f,vColor+0.3f);
	return true;
}

C3DMapSceneObj* C3DMapSceneObj::CreateNew(const char* szFilename,const Vec3D& vPos,const Vec3D& vRotate,const Vec3D& vScale)
{
	C3DMapSceneObj* pObject = new C3DMapSceneObj;
	if(!pObject->Create(szFilename,vPos,vRotate,vScale))
	{
		S_DEL(pObject);
		return NULL;
	}
	return pObject;
}

void C3DMapSceneObj::renderFocus()const
{
	C3DMapObj::renderFocus(0xFFFF0000);
}