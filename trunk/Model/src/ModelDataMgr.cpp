#include "ModelDataMgr.h"
#include "IORead.h"

CModelDataMgr::CModelDataMgr()
{
#ifdef _DEBUG
	loadPlugFromPath("Plugins\\debug\\");
#else
	loadPlugFromPath("Plugins\\");
#endif
}

uint32 CModelDataMgr::RegisterModel(const std::string& strFilename)
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

CModelData* CModelDataMgr::GetModel(uint32 uModelID)
{
	return getItem(uModelID);
}

bool CModelDataMgr::loadModel(CModelData& modelData,const std::string& strFilename)
{
	// 判断格式--根据文件后缀名
	std::string strExt = GetExtension(strFilename);
	for (size_t i=0;i<m_arrPlugObj.size();++i)
	{
		if (m_arrPlugObj[i].pObj->GetFormat()==strExt)
		{
			return m_arrPlugObj[i].pObj->importData(&modelData,strFilename);
		}
	}
	return false;
}

bool CModelDataMgr::loadPlugFromPath(const std::string& strPath)
{
	std::string strFindFile = strPath+"*.dll";

	WIN32_FIND_DATAA wfd;
	HANDLE hf = FindFirstFileA(strFindFile.c_str(), &wfd);
	if (INVALID_HANDLE_VALUE != hf)
	{
		createPlug(strPath + wfd.cFileName);
		while (FindNextFileA(hf, &wfd))
		{
			createPlug(strPath + wfd.cFileName);
		}
		FindClose(hf);
	}
	return true;
}

bool CModelDataMgr::createPlug(const std::string& strFilename)
{
	bool brt = FALSE;

	if (m_arrPlugObj.size() > 255){
		MessageBoxA(NULL,"插件过多", "message", MB_OK|MB_ICONINFORMATION);
		return brt;
	}

	MODEL_PLUG_ST stPs;

	ZeroMemory(&stPs, sizeof(stPs));

	stPs.hIns = LoadLibraryA(strFilename.c_str());
	if (stPs.hIns)
	{
		PFN_Model_Plug_CreateObject pFunc = (PFN_Model_Plug_CreateObject)GetProcAddress(
			stPs.hIns, "Model_Plug_CreateObject");
		if (pFunc){
			if (pFunc((void **)&stPs.pObj)){
				brt =true;
				m_arrPlugObj.push_back(stPs);
				//m_wstrFileType=s2ws(stPs.pObj->GetFormat());
			}
		}
	}
	if (!brt){
		if (stPs.pObj){
			stPs.pObj->Release();
		}
		if (stPs.hIns){
			FreeLibrary(stPs.hIns);
		}
	}
	return brt;
}

std::string CModelDataMgr::getAllExtensions()
{
	std::string strExts;
	for (size_t i=0;i<m_arrPlugObj.size();++i)
	{
		strExts+=m_arrPlugObj[i].pObj->GetFormat();
	}
	return strExts;
}