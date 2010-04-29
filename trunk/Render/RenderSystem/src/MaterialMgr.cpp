#include "MaterialMgr.h"
#include "IORead.h"
#include "CSVFile.h"
#include "RenderSystem.h"
#include "FileSystem.h"

CMaterialMgr::CMaterialMgr()
{
#ifdef _DEBUG
	m_DataPlugsMgr.createPlugFromPath("Plugins\\debug\\","Material_Data_Plug_CreateObject");
#else
	m_DataPlugsMgr.createPlugFromPath("Plugins\\","Material_Data_Plug_CreateObject");
#endif
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
