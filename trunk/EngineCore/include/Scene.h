#pragma once
#include "Terrain.h"
#include "MapObj.h"
#include "3DMapSceneObj.h"
#include "3DMapEffect.h"
#include "ObjectTree.h"

class DLL_EXPORT CScene:public iScene
{
public:
	struct ObjectInfo
	{
		uint32		uID;
		std::string	strName;
		std::string strFilename;
		//float		fScale;
		//bool		bIsGround;
		//bool		bHasShadow;
	};

	typedef std::map<uint32,ObjectInfo>  MAP_OBJECT_INFO;
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
	virtual void setObjectResources(uint32 uID,const std::string& strName,const std::string& strFilename);
	virtual void createObjectTree(const BBox& box, size_t size);
	virtual bool addMapObj(CMapObj* pObj);
	virtual bool delMapObj(CMapObj* pObj);

	C3DMapEffect* add3DMapEffect( Vec3D vWorldPos, char* pszIndex, BOOL bDelself = true);
	void del3DMapEffect(Vec3D vWorldPos);
	void del3DMapEffect(C3DMapEffect* pEffect);

	virtual CMapObj* add3DMapSceneObj(uint32 uID,Vec3D vPos,Vec3D vRotate, float fScale);
	virtual void removeAllObjects();
	virtual void getAllObjects(DEQUE_MAPOBJ&  setObject);
	//
	void setObjectFocus(CMapObj* pObject);
	CMapObj* getObjectFocus();

	CMapObj* pickObject(const Vec3D& vRayPos , const Vec3D& vRayDir);

	//void ResetAnim();

	// data
	MAP_OBJECT_INFO& GetObjectInfo(){return m_ObjectInfo;}

	ObjectTree& GetObject(){return m_ObjectTree;}

	void			SetTerrain(CTerrain* pTerrain){m_pTerrain = pTerrain;}
	iTerrain*		getTerrain(){return m_pTerrain;}
	const iTerrain*	getTerrain()const{return m_pTerrain;}
	void			CalcLightMap();

	void showStaticObject(bool bShowStaticObject){m_bShowStaticObject = bShowStaticObject;}
	void showAnimObject(bool bShowAnimObject){m_bShowAnimObject = bShowAnimObject;}
	void showObjectBBox(bool bShowObjectBBox){m_bShowObjectBBox = bShowObjectBBox;}
	bool isShowStaticObject(){return m_bShowStaticObject;}

	bool isShowAnimObject(){return m_bShowAnimObject;}
	bool isShowObjectBBox(){return m_bShowObjectBBox;}

	CMapObj* getFocusObject(){return m_pObjectFocus;}

	void setFog(const Fog& fog);
	const Fog& getFog(){return m_Fog;}
	void setLight(const DirectionalLight& light);
	const DirectionalLight& getLight(){return m_Light;}
	void setTargetPos(Vec3D vPos){m_vTargetPos = vPos;}
	const Vec3D& getTargetPos()const{return m_vTargetPos;}
public:
	CTerrain*		m_pTerrain;
	ObjectTree		m_ObjectTree;
	DEQUE_MAPOBJ	m_setMapObj;
	DEQUE_MAPOBJ	m_setRenderSceneObj;
	MAP_OBJECT_INFO	m_ObjectInfo;
	bool			m_bNeedUpdate;
protected:
	CShader* m_pShaderFocus;
	CMapObj* m_pObjectFocus;        // The object which has focus
	bool m_bShowStaticObject;
	bool m_bShowAnimObject;
	bool m_bShowObjectBBox;
	bool m_bShowObjectTreeBox;



	Fog					m_Fog;
	DirectionalLight	m_Light;
	Vec3D				m_vTargetPos;
};