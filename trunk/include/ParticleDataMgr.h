#pragma once
#include "TMapDataMgr.h"
#include "InterfaceModel.h"
#include "TSingleton.h"

class CParticleDataMgr: public TMapDataMgrMgr<ParticleData, CParticleDataPlugBase>,public TSingleton<CParticleDataMgr>
{
public:
	CParticleDataMgr():TMapDataMgrMgr("Plugins\\*.dpa"){}
};