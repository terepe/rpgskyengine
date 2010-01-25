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

void UIGraph::DrawRect(const RECT& rcDest, Color32 color)
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

void UIGraph::FillRect(const RECT& rcDest, Color32 color)
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

void UIGraph::DrawSprite(CUIStyle& style, int nIndex, const RECT& rcDest)
{
		Color32 color = style.m_crSpriteColor[nIndex].getColor();
		// No need to draw fully transparent layers
		if(color.a == 0)
			return;
		const CUISpriteCyclostyle& SpriteCyclostyle = style.GetCyclostyle().m_SpriteStyle[nIndex];
		switch(SpriteCyclostyle.m_nSpriteLayoutType)
		{
		case SPRITE_LAYOUT_SIMPLE:
			{
				return DrawSprite(SpriteCyclostyle.m_rcBorder,
					rcDest, SpriteCyclostyle.m_nTexture, color.c);
			}
			break;
		case SPRITE_LAYOUT_3X3GRID:
			{
				return DrawSprite3x3Grid(SpriteCyclostyle.m_rcBorder,
					SpriteCyclostyle.m_rcCenter,
					rcDest, SpriteCyclostyle.m_nTexture, color.c);
			}
			break;
		case SPRITE_LAYOUT_3X3GRID_WRAP:
			{
				return DrawSprite3x3GridWrap(SpriteCyclostyle.m_rcBorder,
					SpriteCyclostyle.m_rcCenter,
					rcDest, SpriteCyclostyle.m_nTexture, color.c);
			}
			break;
		case SPRITE_LAYOUT_DISPERSE_3X3GRID:
			break;
		default:
			break;
		}
}

void UIGraph::DrawSprite3x3Grid(const RECT& rcSrc, const RECT& rcCenterSrc, const RECT& rcDest, UINT TextureID, Color32 color)
{
	if(color.a == 0)
		return;
	GetGraphics().Draw3x3Grid(rcSrc, rcCenterSrc, rcDest, TextureID, color);
}

void UIGraph::DrawSprite3x3GridWrap(const RECT& rcSrc, const RECT& rcCenterSrc, const RECT& rcDest, UINT TextureID, Color32 color)
{
	if(color.a == 0)
		return;
	GetGraphics().Draw3x3GridWrap(rcSrc, rcCenterSrc, rcDest, TextureID, color);
}

void UIGraph::DrawSprite(const RECT& rcSrc, const RECT& rcDest, UINT TextureID, Color32 color)
{
	if(color.a == 0)
		return;
	GetGraphics().DrawTex(rcSrc, rcDest, TextureID, color);
}

void UIGraph::DrawSprite(const RECT& rcDest, UINT TextureID, Color32 color)
{
	if(color.a == 0)
		return;
	GetGraphics().DrawTex(rcDest, TextureID, color);
}

void UIGraph::CalcTextRect(const std::wstring& wstrText, RECT& rcDest)
{
	GetTextRender().drawText(wstrText, -1, rcDest,DTL_CALCRECT);
}

void UIGraph::DrawText(const std::wstring& wstrText, CUIStyle& style, int nIndex, RECT& rcDest, bool bShadow, int nCount)
{
	Color32 color = style.m_crFontColor[nIndex].getColor();
	uint32 uFormat =  style.GetCyclostyle().m_FontStyle[nIndex].uFormat;
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