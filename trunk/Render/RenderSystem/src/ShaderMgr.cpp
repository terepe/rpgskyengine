#include "ShaderMgr.h"
#include "IORead.h"

CShaderMgr::CShaderMgr():
m_uShareShaderID(0)
{
}
CShaderMgr::~CShaderMgr()
{
}

bool CShaderMgr::createSharedShader(const std::string& strFilename)
{
	m_uShareShaderID = registerItem(strFilename);
	return m_uShareShaderID!=0;
}

CShader* CShaderMgr::getSharedShader()
{
	return getItem(m_uShareShaderID);
}

uint32 CShaderMgr::registerItem(const std::string& strFilename)
{
	if(!IOReadBase::Exists(strFilename))
	{
		return 0;
	}
	if (find(strFilename))
	{
		return addRef(strFilename);
	}
	CShader* pShader = createItem(strFilename);
	return add(strFilename, pShader);
}


