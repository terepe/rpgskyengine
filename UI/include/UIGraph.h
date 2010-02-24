#pragma once
#include "UIStyle.h"

#if defined(_DEBUG)
#pragma comment(lib, "rendersystemd.lib")
#pragma comment(lib, "tinyxmld.lib")
#else
#pragma comment(lib, "rendersystem.lib")
#pragma comment(lib, "tinyxml.lib")
#endif

class UIGraph
{
public:
	static void DrawRect(const RECT& rcDest, Color32 color);
	static void FillRect(const RECT& rcDest, Color32 color);
	static void DrawPolyLine(POINT* apPoints, UINT nNumPoints, Color32 color);
	static void DrawSprite(const RECT& rcSrc, const RECT& rcDest, UINT TextureID, Color32 color);
	static void DrawSprite(const RECT& rcDest, UINT TextureID, Color32 color);
	static void DrawSprite3x3Grid(const RECT& rcSrc, const RECT& rcCenterSrc, const RECT& rcDest, UINT TextureID, Color32 color);
	static void DrawSprite3x3GridWrap(const RECT& rcSrc, const RECT& rcCenterSrc, const RECT& rcDest, UINT TextureID, Color32 color);
	static void CalcTextRect(const std::wstring& wstrText, RECT& rcDest);
	static void DrawText(const std::wstring& wstrText, CUIStyle& style, int nIndex, RECT& rcDest, bool bShadow = false, int nCount = -1);
	static void InitFont(const std::wstring& wstrFontName, uint32 uSize);
	static int	 GetFontSize();
};