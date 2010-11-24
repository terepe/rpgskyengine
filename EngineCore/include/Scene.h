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
	virtual void		addChild				(CRenderNodel* pChild);
	virtual void		removeChild				(CRenderNodel* pChild);
	bool				removeRenderObj			(CMapObj* pObj);

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

	iTerrainData*		getTerrainData			()						{return m_pTerrain;}
	const iTerrainData*	getTerrainData			()const					{return m_pTerrain;}
	void				CalcLightMap			();
	GET_SET_VARIABLE(CTerrain*,m_p,Terrain );

	GET_SET_VARIABLE(bool,m_b,ShowObject);
	GET_SET_VARIABLE(bool,m_b,ShowObjectBBox);

	CONST_GET_SET_VARIABLE(DirectionalLight&,m_,Light);
	CONST_GET_SET_VARIABLE(Fog&,m_,Fog);
	CONST_GET_SET_VARIABLE(Vec3D&,m_v,TargetPos);

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