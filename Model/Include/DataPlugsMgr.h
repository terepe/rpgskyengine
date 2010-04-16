#pragma once
#include "InterfaceDataPlugsBase.h"
#include <string>
#include <vector>

typedef struct{
	CDataPlugBase * pObj;
	HINSTANCE hIns;
}DATA_PLUG_ST, * LPDATA_PLUG_ST;

class CDataPlugsMgr
{
public:
	CDataPlugBase* getPlugByExtension(const std::string& strExt);
	bool createPlugFromPath(const std::string& strPath,const std::string& strFun);
	bool createPlug(const std::string& strFilename,const std::string& strFun);
	std::string getAllExtensions();
private:
	std::vector<DATA_PLUG_ST> m_setPlugObj;
};