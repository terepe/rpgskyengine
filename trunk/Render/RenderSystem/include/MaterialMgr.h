#pragma once
#include "manager.h"
#include "Material.h"

class DLL_EXPORT CMaterialMgr
{
public:
	CMaterialMgr();
	~CMaterialMgr();
	CMaterial& getItem(const std::string& strMaterialName);
protected:
	std::map<std::string, CMaterial> m_Items;
};