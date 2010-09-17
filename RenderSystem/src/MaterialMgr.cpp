#include "MaterialMgr.h"
#include "FileSystem.h"

CMaterialMgr::CMaterialMgr()
{
	m_DataPlugsMgr.createPlugFromPath("Plugins\\","Material_Data_Plug_CreateObject");
}

CMaterialMgr::~CMaterialMgr()
{
}

CMaterial& CMaterialMgr::getItem(const std::string& strMaterialName)
{
	return m_Items[strMaterialName];
}

bool CMaterialMgr::loadMaterial(const std::string& strFilename)
{
	// 判断格式--根据文件后缀名
	std::string strExt = GetExtension(strFilename);
	CMaterialDataPlugBase* pModelPlug = (CMaterialDataPlugBase*)m_DataPlugsMgr.getPlugByExtension(strExt);
	if (pModelPlug)
	{
		return pModelPlug->importData(m_Items,strFilename);
	}
	return false;
}
