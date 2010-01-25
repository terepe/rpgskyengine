#pragma once
#include "manager.h"
#include "Shader.h"

class DLL_EXPORT CShaderMgr : public CManager<CShader>
{
public:
	CShaderMgr();
	~CShaderMgr();

	virtual CShader* createItem(const std::string& strFilename) = 0;
	bool createSharedShader(const std::string& strFilename);
	CShader* getSharedShader();
	uint32 registerItem(const std::string& strFilename);
protected:
	uint32	m_uShareShaderID;
};