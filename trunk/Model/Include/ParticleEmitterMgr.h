#pragma once
#include "TMapDataMgr.h"
#include "InterfaceModel.h"

class CParticleEmitterMgr: public TMapDataMgrMgr<CParticleEmitter, CParticleEmitterDataPlugBase>
{
public:
	CParticleEmitterMgr():TMapDataMgrMgr("Plugins\\","ParticleEmitter_Data_Plug_CreateObject"){}
};