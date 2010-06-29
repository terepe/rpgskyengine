#pragma once
#include <vector>

class CScriptStringAnalysis
{
public: 
	CScriptStringAnalysis();
	~CScriptStringAnalysis();
public:
	void CPToX(int nCP, bool bTrail, int& nX);
	void XtoCP(int nX, int& nCP, bool& bTrail);
	bool ScriptStringAnalyse(const std::wstring& strText);
protected:
	std::vector<unsigned short int> m_CharPos;
};