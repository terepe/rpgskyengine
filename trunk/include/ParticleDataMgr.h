#pragma once
#include "TMapDataMgr.h"
#include "InterfaceModel.h"

class CParticleDataMgr: public TMapDataMgrMgr<CParticleData, CParticleDataPlugBase>
{
public:
	CParticleDataMgr():TMapDataMgrMgr("Plugins\\*.dpa"){}
};