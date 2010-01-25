#include "UniBuffer.h"

#define UNISCRIBE_DLLNAME L"\\usp10.dll"

// DXUT_MAX_EDITBOXLENGTH is the maximum string length allowed in edit boxes,
// including the NULL terminator.
// 
// Uniscribe does not support strings having bigger-than-16-bits length.
// This means that the string must be less than 65536 characters long,
// including the NULL terminator.
#define DXUT_MAX_EDITBOXLENGTH 0xFFFF

// Static member initialization
HINSTANCE CUniBuffer::s_hDll = NULL;
const SCRIPT_LOGATTR* (WINAPI *CUniBuffer::_ScriptString_pLogAttr)(SCRIPT_STRING_ANALYSIS) = Dummy_ScriptString_pLogAttr;
const int* (WINAPI *CUniBuffer::_ScriptString_pcOutChars)(SCRIPT_STRING_ANALYSIS) = Dummy_ScriptString_pcOutChars;


void CUniBuffer::Initialize()
{
	if(s_hDll) // Only need to do once
		return;

	WCHAR wszPath[MAX_PATH+1];
	if(!::GetSystemDirectoryW(wszPath, MAX_PATH+1))
		return;
	std::wstring wstrPath = wszPath;
	wstrPath+=UNISCRIBE_DLLNAME;

	s_hDll = LoadLibraryW(wstrPath.c_str());
	if(s_hDll)
	{
		FARPROC Temp;
		GETPROCADDRESS(s_hDll, ScriptString_pLogAttr, Temp);
		GETPROCADDRESS(s_hDll, ScriptString_pcOutChars, Temp);
	}
}



void CUniBuffer::Uninitialize()
{
	if(s_hDll)
	{
		PLACEHOLDERPROC(ScriptString_pLogAttr);
		PLACEHOLDERPROC(ScriptString_pcOutChars);

		FreeLibrary(s_hDll);
		s_hDll = NULL;
	}
}


// Uniscribe -- Analyse() analyses the string in the buffer
void CUniBuffer::Analyse()
{
	if(m_bAnalyseRequired)
	{
		m_bAnalyseRequired = !m_Analysis.ScriptStringAnalyse(m_wstrBuffer);
	}
}



CUniBuffer::CUniBuffer(int nInitialSize)
{
	CUniBuffer::Initialize();  // ensure static vars are properly init'ed first
	m_bAnalyseRequired = true;
}

CUniBuffer::~CUniBuffer()
{
}

WCHAR& CUniBuffer::operator[](int n)  // No param checking
{
	// This version of operator[] is called only
	// if we are asking for write access, so
	// re-analysis is required.
	m_bAnalyseRequired = true;
	return m_wstrBuffer[n];
}



void CUniBuffer::Clear()
{
	m_wstrBuffer.clear();
	m_bAnalyseRequired = true;
}



// Inserts the char at specified index.
// If nIndex == -1, insert to the end.

bool CUniBuffer::InsertChar(int nIndex, WCHAR wChar)
{
	assert(nIndex >= 0);

	if(nIndex < 0 || nIndex > m_wstrBuffer.length())
		return false;  // invalid index

	m_wstrBuffer.insert(m_wstrBuffer.begin()+nIndex, wChar);
	m_bAnalyseRequired = true;
	return true;
}



// Removes the char at specified index.
// If nIndex == -1, remove the last char.

bool CUniBuffer::RemoveChar(int nIndex,size_t count)
{
	if(m_wstrBuffer.length()==0 || nIndex >= m_wstrBuffer.length())
		return false;  // Invalid index

	m_wstrBuffer.erase(nIndex,count);
	m_bAnalyseRequired = true;
	return true;
}

// Inserts the first nCount characters of the string pStr at specified index.
// If nCount == -1, the entire string is inserted.
// If nIndex == -1, insert to the end.

bool CUniBuffer::InsertString(uint32 uIndex, const WCHAR *pStr, int nCount)
{
	if(uIndex > m_wstrBuffer.length())
		return false;  // invalid index

	if(-1 == nCount)
		nCount = lstrlenW(pStr);

	m_wstrBuffer.insert(uIndex, pStr);
	m_bAnalyseRequired = true;

	return true;
}

bool CUniBuffer::SetText(const std::wstring& wstrText)
{
	m_wstrBuffer = wstrText;
	m_bAnalyseRequired = true;
	return true;
}

void CUniBuffer::CPtoX(int nCP, bool bTrail, int& nX)
{
	nX = 0;  // Default
	Analyse();
	m_Analysis.CPToX(nCP, bTrail, nX);
}

bool CUniBuffer::XtoCP(int nX, int& nCP, bool& bTrail)
{
	nCP = 0;
	bTrail = false;  // Default
	Analyse();
	m_Analysis.XtoCP(nX, nCP, bTrail);

	// If the coordinate falls outside the text region, we
	// can get character positions that don't exist.  We must
	// filter them here and convert them to those that do exist.
	if(nCP == -1 && bTrail == true)
	{
		nCP = 0; bTrail = false;
	}
	else if(nCP > m_wstrBuffer.length() && bTrail == false)
	{
		nCP = m_wstrBuffer.length(); bTrail = true;
	}

	return true;
}

void CUniBuffer::GetPriorItemPos(int nCP, int *pPrior)
{
// 	*pPrior = nCP;  // Default is the char itself
// 
	//Analyse();
// 
// 	const SCRIPT_LOGATTR *pLogAttr = _ScriptString_pLogAttr(m_Analysis);
// 	if(!pLogAttr)
// 		return;
// 
// 	if(!_ScriptString_pcOutChars(m_Analysis))
// 		return;
// 	int nInitial = *_ScriptString_pcOutChars(m_Analysis);
// 	if(nCP - 1 < nInitial)
// 		nInitial = nCP - 1;
// 	for(int i = nInitial; i > 0; --i)
// 		if(pLogAttr[i].fWordStop ||       // Either the fWordStop flag is set
// 			(!pLogAttr[i].fWhiteSpace &&  // Or the previous char is whitespace but this isn't.
// 			pLogAttr[i-1].fWhiteSpace))
// 		{
// 			*pPrior = i;
// 			return;
// 		}
// 		// We have reached index 0.  0 is always a break point, so simply return it.
// 		*pPrior = 0;
}



void CUniBuffer::GetNextItemPos(int nCP, int *pPrior)
{
// 	*pPrior = nCP;  // Default is the char itself
// 
// 	HRESULT hr = S_OK;
	//Analyse();
// 	if(FAILED(hr))
// 		return;
// 
// 	const SCRIPT_LOGATTR *pLogAttr = _ScriptString_pLogAttr(m_Analysis);
// 	if(!pLogAttr)
// 		return;
// 
// 	if(!_ScriptString_pcOutChars(m_Analysis))
// 		return;
// 	int nInitial = *_ScriptString_pcOutChars(m_Analysis);
// 	if(nCP + 1 < nInitial)
// 		nInitial = nCP + 1;
// 	for(int i = nInitial; i < *_ScriptString_pcOutChars(m_Analysis) - 1; ++i)
// 	{
// 		if(pLogAttr[i].fWordStop)      // Either the fWordStop flag is set
// 		{
// 			*pPrior = i;
// 			return;
// 		}
// 		else
// 			if(pLogAttr[i].fWhiteSpace &&  // Or this whitespace but the next char isn't.
// 				!pLogAttr[i+1].fWhiteSpace)
// 			{
// 				*pPrior = i+1;  // The next char is a word stop
// 				return;
// 			}
// 	}
// 	// We have reached the end. It's always a word stop, so simply return it.
// 	*pPrior = *_ScriptString_pcOutChars(m_Analysis) - 1;
}
