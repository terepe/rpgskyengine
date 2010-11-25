#pragma once
#include "Terrain.h"
#include "MapObj.h"
#include "3DMapSceneObj.h"
#include "3DMapEffect.h"
#include "ObjectTree.h"

class CFocusNode: public CRenderNode
{
	virtual	int			getType				() {return NODE_BASE;}
	Vec3D				getCenterPos		();
	void				setCenterPos		(const Vec3D& vPos);
	Vec3D				getCenterRotate		();
	void				setCenterRotate		(const Vec3D& vRotate);
	Vec3D				getCenterScale		();
	void				setCenterScale		(const Vec3D& vScale);
};