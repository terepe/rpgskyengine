#include "ModelDataMgr.h"
#include "IORead.h"
#include "FileSystem.h"

CModelDataMgr::CModelDataMgr()
{
	m_DataPlugsMgr.createPlugFromPath("Plugins\\","Model_Plug_CreateObject");
}

unsigned long CModelDataMgr::RegisterModel(const std::string& strFilename)
{
	if (strFilename.length()==0)
	{
		return 0;
	}
	if(!IOReadBase::Exists(strFilename))// 检查文件是否存在
	{
		return 0;
	}
	if (find(strFilename))// 有一样的纹理在了 不用再创建了啦
	{
		return addRef(strFilename);
	}

	CModelData* pModel = new CModelData();
	//if (strExt==".sm")
	//{
	//	pModel = new CModelData();
	//}
	//else if (strExt==".m2")
	//{
	//	pModel = new CM2Model();
	//}
	//else if (strExt==".bmd")
	//{
	//	pModel = new CBmdModel();
	//}
	return add(strFilename, pModel);
}

CModelData* CModelDataMgr::GetModel(unsigned long uModelID)
{
	return getItem(uModelID);
}

bool CModelDataMgr::loadModel(CModelData& modelData,const std::string& strFilename)
{
	// 判断格式--根据文件后缀名
	std::string strExt = GetExtension(strFilename);
	CModelPlugBase* pModelPlug = (CModelPlugBase*)m_DataPlugsMgr.getPlugByExtension(strExt);
	if (pModelPlug)
	{
		return pModelPlug->importData(&modelData,strFilename);
	}
	return false;
}

CDataPlugsMgr& CModelDataMgr::getDataPlugsMgr()
{
	return m_DataPlugsMgr;
}