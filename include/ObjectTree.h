#pragma once
#include "MapObj.h"
#include <algorithm>

class ObjectTree
{
	ObjectTree* pChild;
	BBox bbox;
	DEQUE_MAPOBJ m_setObjet;
public:
	ObjectTree();
	~ObjectTree();
	const BBox& getBBox()const;
	void clearObjects();
	//void setupObjectBBox();
	static void getObjectsByBBox(const BBox& box,const std::vector<CMapObj*>& setSrcObject, std::vector<CMapObj*>& setDestObject);
	//void getObjectTreesByFrustum(const CFrustum& frustum, std::vector<ObjectTree*>& setObjectTree);
	void getObjects(DEQUE_MAPOBJ& setObject);
	void getObjectsByPos(Vec3D vPos, DEQUE_MAPOBJ& setObject);
	void getObjectsByCell(Pos2D posCell, DEQUE_MAPOBJ& setObject);
	void getObjectsByFrustum(const CFrustum& frustum, DEQUE_MAPOBJ& setObject);
	ObjectTree* getNodeByAABB(const BBox& box);
	bool addObject(CMapObj* pObject);
	bool delObject(CMapObj* pObject);
	bool eraseObject(CMapObj* pObject);
	bool updateObject(CMapObj* pObject);
	ObjectTree* find(CMapObj* pObject);
	void create(const BBox& box, size_t size);
	void process();
};

