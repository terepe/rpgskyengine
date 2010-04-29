#pragma once
#include "DataPlugsMgr.h"
#include "ModelData.h"
#include "TSingleton.h"

class CModelDataMgr: public CManager<CModelData>,public TSingleton<CModelDataMgr>
{
public:
	CModelDataMgr();
	uint32 RegisterModel(const std::string& strFilename);
	CModelData* GetModel(uint32 uModelID);
	bool loadModel(CModelData& modelData,const std::string& strFilename);
	CDataPlugsMgr& getDataPlugsMgr();
private:
	CDataPlugsMgr m_DataPlugsMgr;
};
