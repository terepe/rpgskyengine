#pragma once
#include "InterfaceDataPlugsBase.h"
#include "iSkeleton.h"
#include "iMesh.h"
#include "iParticle.h"

class iRenderNode
{
public:
	virtual iRenderNode*	getChild			(const char* szName)=0;
	virtual void			addChild			(iRenderNode* pChild)=0;
};

class CMaterial;
class iRenderNodeMgr
{
public:
	iRenderNodeMgr(){};
	virtual ~iRenderNodeMgr(){};

	virtual iRenderNode*	loadRenderNode(const char* szFilename)=0;
	virtual iRenderNode*	createRenderNode(iSkeletonData* data)=0;
	virtual iRenderNode*	createRenderNode(ParticleData* data)=0;
	virtual iRenderNode*	createRenderNode(iLodMesh* data)=0;
	virtual iSkeletonData*	createSkeletonData(const char* szName)=0;
	virtual ParticleData*	createParticleData(const char* szName)=0;
	virtual iLodMesh*		createLodMesh(const char* szName)=0;
	virtual CMaterial*		createMaterial(const char* szName)=0;
};

//////////////////////////////////////////////////////////////////////////
class CModelPlugBase:public CDataPlugBase
{
public:
	CModelPlugBase(){};
	virtual ~CModelPlugBase(){};
	virtual iRenderNode* importData(iRenderNodeMgr* pRenderNodeMgr, const char* szFilename)=0;
};