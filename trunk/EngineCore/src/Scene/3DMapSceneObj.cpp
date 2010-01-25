#include "3DMapSceneObj.h"

bool C3DMapSceneObj::Create(const string& strFilename,Vec3D vPos,Vec3D vRotate,float fScale)
{
	//if (!IOReadFile::Exists(strFilename))
	//{
	//	MessageBoxA(NULL,strFilename.c_str(),"Error!Cannot find!",0);
	//	return false;
	//}
	m_pModelObject = new CModelObject;
	m_pModelObject->Register(strFilename);
	setPos(vPos);
	setRotate(vRotate);
	setScale(fScale);
	//Vec4D vColor = m_pTerrain->GetData().GetColor(Vec2D(vPos.x,vPos.z));
	//vColor.w=1.0f;
	//pObject->SetMaterial(vColor*0.5f,vColor+0.3f);
	return true;
}

C3DMapSceneObj* C3DMapSceneObj::CreateNew(const string& strFilename,Vec3D vPos,Vec3D vRotate,float fScale)
{
	C3DMapSceneObj* pObject = new C3DMapSceneObj;
	if(!pObject->Create(strFilename,vPos,vRotate,fScale))
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