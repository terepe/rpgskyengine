#pragma once
#include "manager.h"
#include "Material.h"
#include "DataPlugsMgr.h"

class CMaterialMgr
{
public:
	CMaterialMgr();
	~CMaterialMgr();
	CMaterial& getItem(const std::string& strMaterialName);
	bool loadMaterial(const std::string& strFilename);
protected:
	std::map<std::string, CMaterial> m_Items;
private:
	CDataPlugsMgr m_DataPlugsMgr;
};