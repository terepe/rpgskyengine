#pragma once
#include "ModelData.h"
#include "TSingleton.h"

typedef struct{
	CModelPlugBase * pObj;
	HINSTANCE hIns;
}MODEL_PLUG_ST, * LPMODEL_PLUG_ST;

class CModelDataMgr: public CManager<CModelData>,public TSingleton<CModelDataMgr>
{
public:
	CModelDataMgr();
	uint32 RegisterModel(const std::string& strFilename);
	CModelData* GetModel(uint32 uModelID);
	bool loadModel(CModelData& modelData,const std::string& strFilename);
	std::string getAllExtensions();
private:
	bool loadPlugFromPath(const std::string& strPath);
	bool createPlug(const std::string& strFilename);

	std::vector<MODEL_PLUG_ST> m_arrPlugObj;
};
