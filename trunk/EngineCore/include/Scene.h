#pragma once
#include "Terrain.h"
#include "MapObj.h"
#include "3DMapSceneObj.h"
#include "3DMapEffect.h"
#include "ObjectTree.h"

class CScene:public iScene, public CRenderNodel
{
public:
	struct ObjectInfo
	{
		__int64		uID;
		std::string	strName;
		std::string strFilename;
		//float		fScale;
		//bool		bIsGround;
		//bool		bHasShadow;
	};

	typedef std::map<__int64,ObjectInfo>  MAP_OBJECT_INFO;
	CScene();
	~CScene();
public:
	virtual void		GetRenderObject			(const CFrustum& frustum, DEQUE_MAPOBJ& ObjectList);
	void				UpdateRender			(const CFrustum& frustum);
	bool				updateMapObj			(CMapObj* pMapObj);
	virtual void		OnFrameMove				(double fTime, float fElapsedTime);
	virtual void		OnFrameRender			(double fTime, float fElapsedTime);
	void				drawObjects();
	void				ShowObjectTreeBox		(bool bShowObjectTreeBox){m_bShowObjectTreeBox = bShowObjectTreeBox;}
	bool				IsShowObjectTreeBox		(){return m_bShowObjectTreeBox;}

	// load
	virtual void		clearObjectResources	();
	virtual void		setObjectResources		(__int64 uID,const std::string& strName,const std::string& strFilename);
	virtual void		createObjectTree		(const BBox& box, size_t size);
	virtual void		addChild				(const char* szName, CRenderNodel* pChild);
	bool				removeRenderObj			(CMapObj* pObj);
	virtual bool		delMapObj				(CMapObj* pObj);

	C3DMapEffect*		add3DMapEffect			(const Vec3D& vWorldPos, char* pszIndex, bool bDelself = true);
	void				del3DMapEffect			(const Vec3D& vWorldPos);
	void				del3DMapEffect			(C3DMapEffect* pEffect);

	virtual CMapObj*	add3DMapSceneObj		(__int64 uID,const Vec3D& vPos,const Vec3D& vRotate,const Vec3D& vScale);
	virtual void		removeAllObjects		();
	virtual void		getAllObjects			(DEQUE_MAPOBJ&  setObject);
	// Focus Objects
	bool				findFocusObject			(CMapObj* pObject);
	void				addFocusObject			(CMapObj* pObject);
	bool				delFocusObject			(CMapObj* pObject);
	void				clearFocusObjects		();
	bool				delMapObjsByFocusObjects();
	std::deque<CMapObj*>& getFocusObjects		();
	Vec3D				getFocusObjectsPos		();
	void				setFocusObjectsPos		(const Vec3D& vPos);
	Vec3D				getFocusObjectsRotate	();
	void				setFocusObjectsRotate	(const Vec3D& vRotate);
	Vec3D				getFocusObjectsScale	();
	void				setFocusObjectsScale	(const Vec3D& vScale);
	// 
	CMapObj*			pickObject				(const Vec3D& vRayPos , const Vec3D& vRayDir);
	//void ResetAnim();
	// data
	MAP_OBJECT_INFO&	GetObjectInfo			()						{return m_ObjectInfo;}
	ObjectTree&			GetObject				()						{return m_ObjectTree;}

	void				SetTerrain				(CTerrain* pTerrain)	{m_pTerrain = pTerrain;}
	iTerrainData*		getTerrainData			()						{return m_pTerrain;}
	const iTerrainData*	getTerrainData			()const					{return m_pTerrain;}
	void				CalcLightMap			();

	void				showObject				(bool bShowObject)		{m_bShowObject = bShowObject;}
	void				showObjectBBox			(bool bShowObjectBBox)	{m_bShowObjectBBox = bShowObjectBBox;}
	bool				isShowObject			()						{return m_bShowObject;}
	bool				isShowObjectBBox		()						{return m_bShowObjectBBox;}

	void				setFog					(const Fog& fog);
	const Fog&			getFog					()						{return m_Fog;}
	void				setLight				(const DirectionalLight& light);
	const DirectionalLight& getLight			()						{return m_Light;}
	void				setTargetPos			(const Vec3D& vPos)		{m_vTargetPos = vPos;}
	const Vec3D&		getTargetPos			()const					{return m_vTargetPos;}

	void				refreshViewport			()						{m_bNeedUpdate = true;}
public:
	CTerrain*			m_pTerrain;
	ObjectTree			m_ObjectTree;
	DEQUE_MAPOBJ		m_setMapObj;
	DEQUE_MAPOBJ		m_setRenderSceneObj;
	DEQUE_MAPOBJ		m_setLightObj;
	MAP_OBJECT_INFO		m_ObjectInfo;
	bool				m_bNeedUpdate;
protected:
	std::deque<CMapObj*>	m_setFocusObjects;        // The object which has focus
	bool					m_bShowObject;
	bool					m_bShowAnimObject;
	bool					m_bShowObjectBBox;
	bool					m_bShowObjectTreeBox;

	Fog						m_Fog;
	DirectionalLight		m_Light;
	Vec3D					m_vTargetPos;
};