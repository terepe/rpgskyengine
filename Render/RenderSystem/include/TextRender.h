#pragma once
#include "UBB.h"
#include "Graphics.h"
#include <vector>


class CTextRender
{
public:
	CTextRender(); 
	~CTextRender();
public:
	void registerFont(const std::wstring& strFontName, const std::string& strFilename); 
	void Init(const std::wstring& wstrFontName, uint32 uSize); 
	void Free();

	void OnFrameMove();

	void DrawTextVB(int nVertexCount, void* pVB);
	void drawText(const std::wstring& strText, int cchText, const CRect<float>& rcDes, UINT format, Color32 dwColor = 0xffffffff,CRect<float>* prcRet=NULL);
	void drawText(const std::wstring& strText, int x, int y, Color32 dwColor = 0xffffffff);

	void SetInsertionPos(int x, int y) { m_nX = x; m_nY = y; }
	void SetForegroundColor(Color32 color) { m_dwColor = color; }
	void drawText(const std::string& strText);
	void drawText(const std::wstring& strText);

	// ubb
	void BuildUBB(CUBB* pUBB, const std::wstring& strText, const RECT& rc);
	void DrawUBB(CUBB* pUBB);
	void DrawUBB(const std::wstring& strText, const RECT& rc);

	//
	int GetCharHeight() const;

	bool		m_bHasUpdata;
//protected:
	TexCharInfo* GetCharInfo(wchar_t ch);
protected:
	Color32		m_dwColor;
	int			m_nX;
	int			m_nY;
	int			m_nLineHeight;
protected:
	void*		m_library; 
	int			m_nH; 
	void*		m_Face;
protected:
	CTexture*	m_pTextTexture;
	int m_nTextTexWidth;
	int m_nTextTexHeight;

	std::map<wchar_t,TexCharInfo> m_CharsInfo;

	bool m_bUpdated;

	std::map<std::wstring, std::string> m_mapFontFilename;
	std::wstring	m_wstrDefaultFont;
//UBB
}; 

DLL_EXPORT CTextRender& GetTextRender();