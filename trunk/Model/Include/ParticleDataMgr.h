#pragma once
#include "TMapDataMgr.h"
#include "InterfaceModel.h"

class CParticleDataMgr: public TMapDataMgrMgr<ParticleData, CParticleDataPlugBase>
{
public:
	CParticleDataMgr():TMapDataMgrMgr("Plugins\\*.dpa"){}
};