#pragma once
#include "UIControl.h"
#include "ScriptStringAnalysis.h"

class DLL_EXPORT CUniBuffer
{
public:
	CUniBuffer(int nInitialSize = 1);
	~CUniBuffer();

	static void Initialize();
	static void Uninitialize();

	size_t	GetTextSize()  { return m_wstrBuffer.length(); }
	const WCHAR* GetBuffer()const { return m_wstrBuffer.c_str(); }
	const WCHAR& operator[](int n) const { return m_wstrBuffer[n]; }
	WCHAR& operator[](int n);
	void Clear();

	bool InsertChar(int nIndex, WCHAR wChar); // Inserts the char at specified index. If nIndex == -1, insert to the end.
	bool RemoveChar(int nIndex,size_t count=1);  // Removes the char at specified index. If nIndex == -1, remove the last char.
	bool InsertString(uint32 uIndex, const WCHAR *pStr, int nCount = -1);  // Inserts the first nCount characters of the string pStr at specified index.  If nCount == -1, the entire string is inserted. If nIndex == -1, insert to the end.
	bool SetText(const std::wstring& wstrText);

	// Uniscribe
	void CPtoX(int nCP, bool bTrail, int& nX);
	bool XtoCP(int nX, int& nCP, bool& bTrail);
	void GetPriorItemPos(int nCP, int *pPrior);
	void GetNextItemPos(int nCP, int *pPrior);

private:
	void Analyse();      // Uniscribe -- Analyse() analyses the string in the buffer

	std::wstring m_wstrBuffer;
	// Uniscribe-specific
	//UIFontNode* m_pFontNode;          // Font node for the font that this buffer uses
	bool m_bAnalyseRequired;            // True if the string has changed since last analysis.
	CScriptStringAnalysis m_Analysis;

private:
	// Empty implementation of the Uniscribe API
	static const SCRIPT_LOGATTR* WINAPI Dummy_ScriptString_pLogAttr(SCRIPT_STRING_ANALYSIS) { return NULL; }
	static const int* WINAPI Dummy_ScriptString_pcOutChars(SCRIPT_STRING_ANALYSIS) { return NULL; }

	// Function pointers
	static const SCRIPT_LOGATTR* (WINAPI *_ScriptString_pLogAttr)(SCRIPT_STRING_ANALYSIS);
	static const int* (WINAPI *_ScriptString_pcOutChars)(SCRIPT_STRING_ANALYSIS);

	static HINSTANCE s_hDll;  // Uniscribe DLL handle

};

#define GETPROCADDRESS(Module, APIName, Temp) \
	Temp = GetProcAddress(Module, #APIName); \
	if(Temp) \
	*(FARPROC*)&_##APIName = Temp

#define PLACEHOLDERPROC(APIName) \
	_##APIName = Dummy_##APIName