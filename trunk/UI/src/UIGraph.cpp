#include "UIGraph.h"
#include "RenderSystem.h"
#include "Graphics.h"
#include "TextRender.h"

struct UI_SCREEN_VERTEX
{
	float x, y, z, h;
	Color32 color;
	float tu, tv;

	static DWORD FVF;
};
DWORD UI_SCREEN_VERTEX::FVF = FVF_XYZRHW | FVF_DIFFUSE | FVF_TEX1;


struct UI_SCREEN_VERTEX_UNTEX
{
	float x, y, z, h;
	Color32 color;

	static DWORD FVF;
};
DWORD UI_SCREEN_VERTEX_UNTEX::FVF = FVF_XYZRHW | FVF_DIFFUSE;

void UIGraph::DrawRect(const CRect<float>& rcDest, Color32 color)
{
	CRenderSystem& R = GetRenderSystem();

	// Since we're doing our own drawing here we need to flush the sprites
	R.SetFVF(UI_SCREEN_VERTEX::FVF);

	R.SetTextureColorOP(0, TBOP_SOURCE2);
	R.SetTextureAlphaOP(0, TBOP_SOURCE2);

	GetGraphics().DrawRect(rcDest, color.c);

	R.SetTextureColorOP(0, TBOP_MODULATE);
	R.SetTextureAlphaOP(0, TBOP_MODULATE);
}

void UIGraph::FillRect(const CRect<float>& rcDest, Color32 color)
{
	CRenderSystem& R = GetRenderSystem();

	// Since we're doing our own drawing here we need to flush the sprites
	R.SetFVF(UI_SCREEN_VERTEX::FVF);


	R.SetTextureColorOP(0, TBOP_SOURCE2);
	R.SetTextureAlphaOP(0, TBOP_SOURCE2);

	GetGraphics().FillRect(rcDest, color.c);

	R.SetTextureColorOP(0, TBOP_MODULATE);
	R.SetTextureAlphaOP(0, TBOP_MODULATE);
}

void UIGraph::DrawPolyLine(POINT* apPoints, UINT nNumPoints, Color32 color)
{
	UI_SCREEN_VERTEX* vertices = new UI_SCREEN_VERTEX[ nNumPoints ];
	if(vertices == NULL)
		return;

	UI_SCREEN_VERTEX* pVertex = vertices;
	POINT* pt = apPoints;
	for(UINT i=0; i < nNumPoints; i++)
	{
		pVertex->x = /*m_x +*/ (float) pt->x;
		pVertex->y = /*m_y +*/ (float) pt->y;
		pVertex->z = 0.5f;
		pVertex->h = 1.0f;
		pVertex->color = color;
		pVertex->tu = 0.0f;
		pVertex->tv = 0.0f;

		pVertex++;
		pt++;
	}
	CRenderSystem& R = GetRenderSystem();

	R.SetFVF(UI_SCREEN_VERTEX::FVF);

	R.SetTextureColorOP(0, TBOP_SOURCE2);
	R.SetTextureAlphaOP(0, TBOP_SOURCE2);

	R.DrawPrimitiveUP(VROT_LINE_STRIP, nNumPoints - 1, vertices, sizeof(UI_SCREEN_VERTEX));

	R.SetTextureColorOP(0, TBOP_MODULATE);
	R.SetTextureAlphaOP(0, TBOP_MODULATE);

	S_DELS(vertices);
}

void UIGraph::DrawSprite3x3Grid(const CRect<float>& rcSrc, const CRect<float>& rcCenterSrc, const CRect<float>& rcDest, UINT TextureID, Color32 color)
{
	if(color.a == 0)
		return;
	GetGraphics().Draw3x3Grid(rcSrc, rcCenterSrc, rcDest, TextureID, color);
}

void UIGraph::DrawSprite3x3GridWrap(const CRect<float>& rcSrc, const CRect<float>& rcCenterSrc, const CRect<float>& rcDest, UINT TextureID, Color32 color)
{
	if(color.a == 0)
		return;
	GetGraphics().Draw3x3GridWrap(rcSrc, rcCenterSrc, rcDest, TextureID, color);
}

void UIGraph::DrawSprite(const CRect<float>& rcSrc, const CRect<float>& rcDest, UINT TextureID, Color32 color)
{
	if(color.a == 0)
		return;
	GetGraphics().DrawTex3D(rcSrc, rcDest, TextureID, color);
}

void UIGraph::DrawSprite(const CRect<float>& rcDest, UINT TextureID, Color32 color)
{
	if(color.a == 0)
		return;
	GetGraphics().DrawTex(rcDest, TextureID, color);
}

void UIGraph::CalcTextRect(const std::wstring& wstrText, CRect<float>& rcDest)
{
	if(wstrText.empty())
	{
		return;
	}
	if(wstrText[0]==L'[')
	{
		RECT rect=GetTextRender().CalcUBBRect(wstrText, rcDest.getRECT());
		rcDest = rect;
	}
	else
	{
		GetTextRender().drawText(wstrText, -1, rcDest,DTL_CALCRECT,0,&rcDest);
	}
}

void UIGraph::DrawText(const std::wstring& wstrText, CUIStyle& style, int nIndex,const CRect<float>& rcDest, bool bShadow, int nCount)
{
	Color32 color = 0xFFFFFFFF;//style.m_mapFont[nIndex].color.getColor();
	uint32 uFormat =  0;//style.GetCyclostyle().m_FontStyle[nIndex].uFormat;
	// No need to draw fully transparent layers
	if(color.a == 0)
		return;

	GetTextRender().drawText(wstrText,nCount,rcDest,uFormat, color);
}

void UIGraph::InitFont(const std::wstring& wstrFontName, uint32 uSize)
{
	GetTextRender().Init(wstrFontName,uSize);
}

int UIGraph::GetFontSize()
{
	return GetTextRender().GetCharHeight();
}