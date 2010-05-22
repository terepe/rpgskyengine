#include "Scene.h"
#include "CsvFile.h"
#include "IORead.h"
#include <algorithm>

CScene::CScene():
m_bShowStaticObject(true),
m_bShowAnimObject(true),
m_bShowObjectBBox(false),
m_Fog(32.0f,48.0f,0.01f,0xFF223344),
m_Light(Vec4D(1.0f,1.0f,1.0f,1.0f),Vec4D(1.0f,1.0f,1.0f,1.0f),Vec4D(1.0f,1.0f,1.0f,1.0f),Vec3D(-1.0f,-1.0f,-1.0f))
{
	m_bNeedUpdate = true;
	m_pTerrain = NULL;
}

CScene::~CScene()
{
	removeAllObjects();
}
static Vec3D vEyeForObjectSort;
bool sortObject(CMapObj* p1, CMapObj* p2)
{
	if (p1->getOrder()!=p2->getOrder())
	{
		return p1->getOrder()>p2->getOrder();
	}
	return p1>p2;
//	//float fLength = (vEyeForObjectSort-p1->getPos()).lengthSquared()-(vEyeForObjectSort-p2->getPos()).lengthSquared();
//	//if (fLength!=0)
//	//{
//	//	return fLength>0;
//	//}
//	return p1->getModelFilename()>p2->getModelFilename();
}

void CScene::GetRenderObject(const CFrustum& frustum, DEQUE_MAPOBJ& ObjectList)
{
	m_ObjectTree.getObjectsByFrustum(frustum,ObjectList);
	static bool bTest = true;
	if (bTest)
	{
		vEyeForObjectSort= frustum.getEyePoint();
		std::sort(ObjectList.begin(),ObjectList.end(), sortObject);

		//f
		//if ((*it)->GetObjType()==MAP_3DOBJ)
		//{
		//	C3DMapSceneObj* p3DMapSceneObj = (C3DMapSceneObj*)(*it);
		//	if (!m_bShowAnimObject&&p3DMapSceneObj->isSkinMesh())
		//	{
		//		continue;
		//	}
		//	if (!m_bShowStaticObject&&!p3DMapSceneObj->isSkinMesh())
		//	{
		//		continue;
		//	}
		//}
	}
}

bool CScene::updateMapObj(CMapObj* pMapObj)
{
	return m_ObjectTree.updateObject(pMapObj);
}

void CScene::OnFrameMove(double fTime, float fElapsedTime)
{
	m_ObjectTree.process();
	for (DEQUE_MAPOBJ::iterator it = m_setRenderSceneObj.begin(); it != m_setRenderSceneObj.end(); ++it)
	{
		//if (!(*it)->isCreated())
		//{
		//	(*it)->create();
		//	m_ObjectTree.delObject((*it));
		//	m_ObjectTree.addObject((*it));
		//}
		(*it)->OnFrameMove(fElapsedTime);
	}
}

void CScene::UpdateRender(const CFrustum& frustum)
{
	if (!m_bNeedUpdate)
	{
		static CFrustum s_frustum;
		if (s_frustum==frustum)
		{
			return;
		}
		s_frustum=frustum;
	}
	m_bNeedUpdate = false;
	if (m_pTerrain)
	{
		m_pTerrain->UpdateRender(frustum);
	}
	//
	m_setRenderSceneObj.clear();
	if (m_bShowStaticObject||m_bShowAnimObject)
	{
		GetRenderObject(frustum, m_setRenderSceneObj);
	}
}
#include "Graphics.h"
void CScene::OnFrameRender(double fTime, float fElapsedTime)
{
	//GetRenderSystem().setFogEnable(true);
	GetRenderSystem().ClearBuffer(true, true, m_Fog.color);
	// 
	CRenderSystem& R = GetRenderSystem();
	if (m_bShowObjectBBox)
	{
		R.SetDepthBufferFunc(true,true);
		for (DEQUE_MAPOBJ::iterator it = m_setRenderSceneObj.begin();
			it != m_setRenderSceneObj.end(); ++it)
		{
			(*it)->renderDebug();
		}
	}
	if (m_setFocusObjects.size()>0)
	{
		for(size_t i=0;i<m_setFocusObjects.size();++i)
		{
			m_setFocusObjects[i]->renderDebug();
		}
		// The octree boxs of focus objects.
		for(size_t i=0;i<m_setFocusObjects.size();++i)
		{
			ObjectTree* pParentObjectTree = m_ObjectTree.find(m_setFocusObjects[i]);
			if (pParentObjectTree)
			{
				GetGraphics().drawBBox(pParentObjectTree->getBBox(),0xFF00FF44);
			}
		}
	}
	//
	GetRenderSystem().setFog(m_Fog);
	GetRenderSystem().setFogEnable(m_Fog.fEnd>0.0f);
	GetRenderSystem().SetDirectionalLight(0, m_Light);
	if (m_pTerrain)
	{
		m_pTerrain->Render();
	}
	{
		//
		for (DEQUE_MAPOBJ::iterator it = m_setRenderSceneObj.begin();
			it != m_setRenderSceneObj.end(); ++it)
		{
			try {
				CMapObj* pObj = (*it);
				if(pObj)
				{
					if(pObj->GetObjType() != MAP_ROLE &&
						pObj->GetObjType() != MAP_HERO &&
						pObj->GetObjType() != MAP_PLAYER &&
						pObj->GetObjType() != MAP_3DEFFECT &&
						pObj->GetObjType() != MAP_3DEFFECTNEW)
					{
						Vec4D vColor = m_pTerrain->GetData().GetColor(Vec2D((*it)->getPos().x,(*it)->getPos().z));
						vColor.w=1.0f;

					
						DirectionalLight light(vColor*0.5f,vColor+0.3f,Vec4D(0.6f,0.6f,0.6f,0.6f),Vec3D(-1.0f,-1.0f,-1.0f));
							GetRenderSystem().SetDirectionalLight(0,light);
					}
					else
					{
						DirectionalLight light(Vec4D(0.3f,0.3f,0.3f,0.3f),Vec4D(0.6f,0.6f,0.6f,0.6f),Vec4D(0.6f,0.6f,0.6f,0.6f),Vec3D(-1.0f,-1.0f,-1.0f));
						GetRenderSystem().SetDirectionalLight(0,light);
					}
					(*it)->render(MATERIAL_RENDER_GEOMETRY);
				}
			}catch(...)
			{
				return;
			}
		}
		DirectionalLight light(Vec4D(0.3f,0.3f,0.3f,0.3f),Vec4D(0.6f,0.6f,0.6f,0.6f),Vec4D(0.6f,0.6f,0.6f,0.6f),Vec3D(-1.0f,-1.0f,-1.0f));
		GetRenderSystem().SetDirectionalLight(0,light);

		for(size_t i=0;i<m_setFocusObjects.size();++i)
		{
			m_setFocusObjects[i]->renderFocus();
		}
		Fog fogForGlow;
		fogForGlow = m_Fog;
		//fogForGlow.fStart = m_Fog.fStart;
		fogForGlow.fEnd = m_Fog.fEnd*2.0f;
		GetRenderSystem().setFog(fogForGlow);
		//
		for (DEQUE_MAPOBJ::iterator it = m_setRenderSceneObj.begin();
			it != m_setRenderSceneObj.end(); ++it)
		{
			(*it)->render(MATERIAL_RENDER_ALPHA);
		}
		GetRenderSystem().setWorldMatrix(Matrix::UNIT);
		for (DEQUE_MAPOBJ::iterator it = m_setRenderSceneObj.begin();
			it != m_setRenderSceneObj.end(); ++it)
		{
			(*it)->render(MATERIAL_RENDER_GLOW);
		}
	}
}

void CScene::clearObjectResources()
{
	m_ObjectInfo.clear();
}

void CScene::setObjectResources(int64 uID,const std::string& strName,const std::string& strFilename)
{
	ObjectInfo Info;
	Info.uID				= uID;
	Info.strName			= strName;
	Info.strFilename		= strFilename;
	m_ObjectInfo[Info.uID] = Info;
}

void CScene::createObjectTree(const BBox& box, size_t size)
{
	m_ObjectTree.create(box,size);
}

bool CScene::addMapObj(CMapObj* pObj)
{
	if(!pObj)
		return false;
	if (m_ObjectTree.addObject(pObj))
	{
		m_bNeedUpdate = true;
		return true;
	}
	else
	{
		delete pObj;
		//MessageBoxA(NULL,strBmdFilename.c_str(),"Error!exclude!",0);
	}
	return false;
}

bool CScene::delMapObj(CMapObj* pObj)
{
	if(!pObj)
		return false;
	if (!m_ObjectTree.delObject(pObj))// when del a object, all the bboxs were not rebuild!
	{
		return false;
	}
	// del from render
	DEQUE_MAPOBJ::iterator it = find( m_setRenderSceneObj.begin( ), m_setRenderSceneObj.end( ), pObj );
	if(it!=m_setRenderSceneObj.end())
	{
		m_setRenderSceneObj.erase(it);
	}
	{
		m_bNeedUpdate = true;
	}

	delFocusObject(pObj);
	return true;
}

C3DMapEffect* CScene::add3DMapEffect(const Vec3D& vWorldPos, char* pszIndex, BOOL bDelSelf)
{
	if (!pszIndex)
		return NULL;

	C3DMapEffect* pEffect = C3DMapEffect::CreateNew( vWorldPos, pszIndex, bDelSelf);

	if(pEffect)
		addMapObj(pEffect);
	return pEffect;
}

void CScene::del3DMapEffect(const Vec3D& vWorldPos)
{
	DEQUE_MAPOBJ setObject;
	m_ObjectTree.getObjectsByPos(vWorldPos,setObject);
	for (DEQUE_MAPOBJ::iterator it=setObject.begin();it!=setObject.end();it++)
	{
		if((*it) && ((*it)->GetObjType() == MAP_3DEFFECT || (*it)->GetObjType() == MAP_3DEFFECTNEW))
		{
			if (m_ObjectTree.delObject(*it))
			{
				C3DMapEffect* pEffect = (C3DMapEffect*)(*it);
				S_DEL(pEffect);
			}
		}
	}
}

void CScene::del3DMapEffect(C3DMapEffect* pEffect)
{
	if (m_ObjectTree.find(pEffect))
	{
		if(pEffect->GetObjType() == MAP_3DEFFECT || pEffect->GetObjType() == MAP_3DEFFECTNEW)
		{
			pEffect->Die();
		}
	}
}

CMapObj* CScene::add3DMapSceneObj(int64 uID,const Vec3D& vPos,const Vec3D& vRotate,const Vec3D& vScale)
{
	if (m_ObjectInfo.find(uID)!=m_ObjectInfo.end())
	{
		const ObjectInfo& objectInfo = m_ObjectInfo[uID];
		std::string strBmdFilename = objectInfo.strFilename;

		C3DMapSceneObj* pObject = C3DMapSceneObj::CreateNew(strBmdFilename,vPos,vRotate,vScale);
		pObject->setObjectID(uID);
		addMapObj(pObject);
		return pObject;
	}
	return NULL;
}

bool CScene::findFocusObject(CMapObj* pObject)
{
	DEQUE_MAPOBJ::iterator it = std::find( m_setFocusObjects.begin( ), m_setFocusObjects.end( ), pObject );
	if(it!=m_setFocusObjects.end())
	{
		return true;
	}
	return false;
}

void CScene::addFocusObject(CMapObj* pObject)
{
	if(findFocusObject(pObject)==false)
	{
		m_setFocusObjects.push_back(pObject);
	}
}

bool CScene::delFocusObject(CMapObj* pObject)
{
	DEQUE_MAPOBJ::iterator it = std::find( m_setFocusObjects.begin( ), m_setFocusObjects.end( ), pObject );
	if(it!=m_setFocusObjects.end())
	{
		m_setFocusObjects.erase(it);
		return true;
	}
	return false;
}

void CScene::clearFocusObjects()
{
	m_setFocusObjects.clear();
}

bool CScene::delMapObjsByFocusObjects()
{
	for(size_t i=0;i<m_setFocusObjects.size();++i)
	{
		delMapObj(m_setFocusObjects[i]);
	}
	clearFocusObjects();
	return true;
}

std::deque<CMapObj*>& CScene::getFocusObjects()
{
	return m_setFocusObjects;
}

Vec3D CScene::getFocusObjectsPos()
{
	Vec3D vPos(0.0f,0.0f,0.0f);
	for(size_t i=0;i<m_setFocusObjects.size();++i)
	{
		vPos+=m_setFocusObjects[i]->getPos();
	}
	vPos/=m_setFocusObjects.size();
	return vPos;
}

void CScene::setFocusObjectsPos(const Vec3D& vPos)
{
	Vec3D vMidPos = getFocusObjectsPos();
	for(size_t i=0;i<m_setFocusObjects.size();++i)
	{
		Vec3D vObjectPos = m_setFocusObjects[i]->getPos();
		vObjectPos+=vPos-vMidPos;
		m_setFocusObjects[i]->setPos(vObjectPos);
		updateMapObj(m_setFocusObjects[i]);
	}
}

Vec3D CScene::getFocusObjectsRotate()
{
	if(m_setFocusObjects.size()==1)
	{
		return m_setFocusObjects[0]->getRotate();
	}
	return Vec3D(0.0f,0.0f,0.0f);
}

void CScene::setFocusObjectsRotate(const Vec3D& vRotate)
{
	if(m_setFocusObjects.size()==1)
	{
		m_setFocusObjects[0]->setRotate(vRotate);
		updateMapObj(m_setFocusObjects[0]);
	}
	else
	{
		//Vec3D vMidPos = getFocusObjectsRotate();
		for(size_t i=0;i<m_setFocusObjects.size();++i)
		{
			//Vec3D vObjectPos = m_setFocusObjects[i]->getPos()+vMidPos-vPos;
			//m_setFocusObjects[i]->setPos(vObjectPos);
			updateMapObj(m_setFocusObjects[i]);
		}
	}
}

Vec3D CScene::getFocusObjectsScale()
{
	if(m_setFocusObjects.size()==1)
	{
		return m_setFocusObjects[0]->getScale();
	}
	return Vec3D(1.0f,1.0f,1.0f);
}

void CScene::setFocusObjectsScale(const Vec3D& vScale)
{
	if(m_setFocusObjects.size()==1)
	{
		m_setFocusObjects[0]->setScale(vScale);
		updateMapObj(m_setFocusObjects[0]);
	}
	else
	{
		Vec3D vMidPos = getFocusObjectsScale();
		for(size_t i=0;i<m_setFocusObjects.size();++i)
		{
			Vec3D vObjectPos = m_setFocusObjects[i]->getPos();
			vObjectPos += (vMidPos-vObjectPos)*vScale;
			m_setFocusObjects[i]->setPos(vObjectPos);
			updateMapObj(m_setFocusObjects[i]);
		}
	}
}

#include "float.h"

#include "intersect.h"
CMapObj* CScene::pickObject(const Vec3D& vRayPos , const Vec3D& vRayDir)
{
	CMapObj* pObject = NULL;
	float fFocusMin = FLT_MAX;
	for (DEQUE_MAPOBJ::iterator it = m_setRenderSceneObj.begin();
		it != m_setRenderSceneObj.end(); ++it)
	{
		float fMin, fMax;
		if ((*it)->intersect(vRayPos , vRayDir, fMin, fMax))
		{
			if (fFocusMin>fMin)
			{
				pObject = *it;
				fFocusMin=fMax;
			}
		}
	}
	return pObject;
}

void CScene::removeAllObjects()
{
	clearFocusObjects();
	m_bNeedUpdate = true;
	m_setRenderSceneObj.clear();
	m_ObjectTree.clearObjects();
}

void CScene::getAllObjects(DEQUE_MAPOBJ&  setObject)
{
	m_ObjectTree.getObjects(setObject);
}

//void CScene::ResetAnim()
//{
//	for (DEQUE_MAPOBJ::iterator it = m_setSceneObj.begin(); it != m_setSceneObj.end(); ++it)
//	{
//		(*it)->m_bAnimCalc = false;
//	}
//}

#include "LightMap.h"
void CScene::CalcLightMap()
{
	CLightMap lightMap;
	lightMap.SetScene(this);
	lightMap.CalcLightMap();
}

void CScene::setFog(const Fog& fog)
{
	m_Fog = fog;
}

void CScene::setLight(const DirectionalLight& light)
{
	m_Light = light;
}