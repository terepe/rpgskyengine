#pragma once
#include "DataPlugsMgr.h"
#include "TSingleton.h"
#include "SkeletonNode.h"
#include "Particle.h"
#include "SkinModel.h"

class CRenderNodeMgr:public iRenderNodeMgr, public TSingleton<CRenderNodeMgr>
{
public:
	CRenderNodeMgr();
	virtual ~CRenderNodeMgr(){};
	virtual CRenderNode*	loadRenderNode(const char* szFilename);
	virtual CRenderNode*	createRenderNode(iSkeletonData* pData);
	virtual CRenderNode*	createRenderNode(ParticleData* pData);
	virtual CRenderNode*	createRenderNode(iLodMesh* pData);
	virtual iSkeletonData*	createSkeletonData(const char* szName);
	virtual ParticleData*	createParticleData(const char* szName);
	virtual iLodMesh*		createLodMesh(const char* szName);
	virtual CMaterial*		createMaterial(const char* szName);
private:
	CDataPlugsMgr	m_DataPlugsMgr;
	std::map<std::string, CSkeletonData>		m_mapSkeletonData;
	std::map<std::string, ParticleData>			m_mapParticleData;
	std::map<std::string, CLodMesh>				m_mapLodMesh;
};
