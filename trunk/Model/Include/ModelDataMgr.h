#pragma once
#include "DataPlugsMgr.h"
#include "ModelData.h"
#include "TSingleton.h"

class CModelDataMgr: public CManager<CModelData>,public TSingleton<CModelDataMgr>
{
public:
	CModelDataMgr();
	unsigned long	RegisterModel(const std::string& strFilename);
	bool			loadModel(CModelData& modelData,const std::string& strFilename);
	CDataPlugsMgr&	getDataPlugsMgr();
private:
	CDataPlugsMgr	m_DataPlugsMgr;
};
