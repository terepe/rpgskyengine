#include "MaterialMgr.h"
#include "IORead.h"
#include "CSVFile.h"
#include "RenderSystem.h"
#include "FileSystem.h"

CMaterialMgr::CMaterialMgr()
{
}

CMaterialMgr::~CMaterialMgr()
{
}

CMaterial& CMaterialMgr::getItem(const std::string& strMaterialName)
{
	return m_Items[strMaterialName];
}

