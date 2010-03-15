#pragma once
#include "manager.h"
#include "Material.h"

class DLL_EXPORT CMaterialMgr
{
public:
	CMaterialMgr();
	~CMaterialMgr();

	//virtual CMaterial* createItem(const std::string& strMaterialName) = 0;
	CMaterial& getItem(const std::string& strMaterialName);
	//bool registerItemsFromFile(const std::string& strMaterialFilename) = 0;
	bool createFromFile(const std::string& strFilename, const std::string& strPath="");
protected:
	std::map<std::string, CMaterial> m_Items;
};