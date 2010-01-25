#pragma once
#include "UIControl.h"

class DLL_EXPORT CScriptStringAnalysis
{
public: 
	CScriptStringAnalysis();
	~CScriptStringAnalysis();
public:
	void CPToX(int nCP, bool bTrail, int& nX);
	void XtoCP(int nX, int& nCP, bool& bTrail);
	bool ScriptStringAnalyse(const std::wstring& strText);
protected:
	std::vector<int16> m_CharPos;
};