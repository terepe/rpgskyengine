#pragma once
#include "TMapDataMgr.h"
#include "Material.h"

class CMaterialMgr: public TMapDataMgrMgr<CMaterial, CMaterialDataPlugBase>
{
public:
	CMaterialMgr():TMapDataMgrMgr("Plugins\\*.dma"){}
};