#pragma once
#include "manager.h"
#include "Shader.h"

class CShaderMgr : public CManager<CShader>
{
public:
	CShaderMgr();
	~CShaderMgr();

	virtual CShader* createItem(const std::string& strFilename) = 0;
	bool createSharedShader(const std::string& strFilename);
	CShader* getSharedShader();
	unsigned long registerItem(const std::string& strFilename);
protected:
	unsigned long	m_uShareShaderID;
};