#pragma once
#include "Terrain.h"
#include "MapObj.h"
#include "3DMapSceneObj.h"
#include "3DMapEffect.h"
#include "ObjectTree.h"

class CScene:public iScene
{
public:
	struct ObjectInfo
	{
		int64		uID;
		std::string	strName;
		std::string strFilename;
		//float		fScale;
		//bool		bIsGround;
		//bool		bHasShadow;
	};

	typedef std::map<int64,ObjectInfo>  MAP_OBJECT_INFO;
	CScene();
	~CScene();
public:
	virtual void GetRenderObject(const CFrustum& frustum, DEQUE_MAPOBJ& ObjectList);
	void UpdateRender(const CFrustum& frustum);
	bool updateMapObj(CMapObj* pMapObj);
	virtual void OnFrameMove(double fTime, float fElapsedTime);
	virtual void OnFrameRender(double fTime, float fElapsedTime);
	void drawObjects();
	void	ShowObjectTreeBox(bool bShowObjectTreeBox){m_bShowObjectTreeBox = bShowObjectTreeBox;}
	bool	IsShowObjectTreeBox(){return m_bShowObjectTreeBox;}

	// load
	virtual void clearObjectResources();
	virtual void setObjectResources(int64 uID,const std::string& strName,const std::string& strFilename);
	virtual void createObjectTree(const BBox& box, size_t size);
	virtual bool addMapObj(CMapObj* pObj);
	virtual bool delMapObj(CMapObj* pObj);

	C3DMapEffect* add3DMapEffect(const Vec3D& vWorldPos, char* pszIndex, bool bDelself = true);
	void del3DMapEffect(const Vec3D& vWorldPos);
	void del3DMapEffect(C3DMapEffect* pEffect);

	virtual CMapObj* add3DMapSceneObj(int64 uID,const Vec3D& vPos,const Vec3D& vRotate,const Vec3D& vScale);
	virtual void removeAllObjects();
	virtual void getAllObjects(DEQUE_MAPOBJ&  setObject);
	// Focus Objects
	bool findFocusObject(CMapObj* pObject);
	void addFocusObject(CMapObj* pObject);
	bool delFocusObject(CMapObj* pObject);
	void clearFocusObjects();
	bool delMapObjsByFocusObjects();
	std::deque<CMapObj*>& getFocusObjects();
	Vec3D getFocusObjectsPos();
	void setFocusObjectsPos(const Vec3D& vPos);
	Vec3D getFocusObjectsRotate();
	void setFocusObjectsRotate(const Vec3D& vRotate);
	Vec3D getFocusObjectsScale();
	void setFocusObjectsScale(const Vec3D& vScale);


	// 
	CMapObj* pickObject(const Vec3D& vRayPos , const Vec3D& vRayDir);

	//void ResetAnim();

	// data
	MAP_OBJECT_INFO& GetObjectInfo(){return m_ObjectInfo;}

	ObjectTree& GetObject(){return m_ObjectTree;}

	void			SetTerrain(CTerrain* pTerrain){m_pTerrain = pTerrain;}
	iTerrainData*		getTerrain(){return m_pTerrain;}
	const iTerrainData*	getTerrain()const{return m_pTerrain;}
	void			CalcLightMap();

	void showStaticObject(bool bShowStaticObject){m_bShowStaticObject = bShowStaticObject;}
	void showAnimObject(bool bShowAnimObject){m_bShowAnimObject = bShowAnimObject;}
	void showObjectBBox(bool bShowObjectBBox){m_bShowObjectBBox = bShowObjectBBox;}
	bool isShowStaticObject(){return m_bShowStaticObject;}

	bool isShowAnimObject(){return m_bShowAnimObject;}
	bool isShowObjectBBox(){return m_bShowObjectBBox;}

	void setFog(const Fog& fog);
	const Fog& getFog(){return m_Fog;}
	void setLight(const DirectionalLight& light);
	const DirectionalLight& getLight(){return m_Light;}
	void setTargetPos(const Vec3D& vPos){m_vTargetPos = vPos;}
	const Vec3D& getTargetPos()const{return m_vTargetPos;}
public:
	CTerrain*		m_pTerrain;
	ObjectTree		m_ObjectTree;
	DEQUE_MAPOBJ	m_setMapObj;
	DEQUE_MAPOBJ	m_setRenderSceneObj;
	MAP_OBJECT_INFO	m_ObjectInfo;
	bool			m_bNeedUpdate;
protected:
	std::deque<CMapObj*> m_setFocusObjects;        // The object which has focus
	bool m_bShowStaticObject;
	bool m_bShowAnimObject;
	bool m_bShowObjectBBox;
	bool m_bShowObjectTreeBox;

	Fog					m_Fog;
	DirectionalLight	m_Light;
	Vec3D				m_vTargetPos;
};