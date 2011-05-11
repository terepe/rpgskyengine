#include "ModelDataMgr.h"
#include "IORead.h"
#include "FileSystem.h"

CModelDataMgr::CModelDataMgr()
{
	m_DataPlugsMgr.loadPlugs("Plugins\\*.dme");
}
/*
unsigned long CModelDataMgr::RegisterModel(const std::string& strFilename)
{
	if (strFilename.length()==0)
	{
		return 0;
	}
	if(!IOReadBase::Exists(strFilename))// Check the filename
	{
		return 0;
	}
	if (find(strFilename))// Has the same model.
	{
		return addRef(strFilename);
	}

	CModelData* pModel = new CModelData();

	return add(strFilename, pModel);
}
*/
CRenderNode* CModelDataMgr::loadModel(const char* szFilename)
{
	// 判断格式--根据文件后缀名
	std::string strExt = GetExtension(szFilename);
	CModelPlugBase* pModelPlug = (CModelPlugBase*)m_DataPlugsMgr.getPlugByExtension(strExt.c_str());
	if (pModelPlug)
	{
		return pModelPlug->importData(&CRenderNodeMgr::getSingleton(),szFilename);
	}
	return false;
}

CDataPlugsMgr& CModelDataMgr::getDataPlugsMgr()
{
	return m_DataPlugsMgr;
}