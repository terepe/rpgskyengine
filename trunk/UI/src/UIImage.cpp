#include "UIImage.h"
#include "UIDialog.h"
#include "RenderSystem.h"
#include "tinyxml.h"

CUIImage::CUIImage()
{
	m_Type = UI_CONTROL_IMAGE;

	m_nTexID = 0;
	SetRect(&m_rcTex, 0, 0, -1, -1);
	m_dwColor = 0xFFFFFFFF;
}
void CUIImage::XMLParse(TiXmlElement* pControlElement)
{
	CUIControl::XMLParse(pControlElement);
	//
	if (pControlElement->Attribute("filename"))
	{
		std::string strImageFilename;
		strImageFilename = pControlElement->Attribute("filename");
		SetFilename(strImageFilename);
	}
}

void CUIImage::OnFrameRender(double fTime, float fElapsedTime)
{    
	if(m_bVisible == false)
		return;

	CONTROL_STATE iState = CONTROL_STATE_NORMAL;

	if(m_bEnabled == false)
		iState = CONTROL_STATE_DISABLED;

	if(m_bVisible == false)
	{
		iState = CONTROL_STATE_HIDDEN;
	}
	else if(m_bEnabled == false)
	{
		iState = CONTROL_STATE_DISABLED;
	}
	else if(m_bMouseOver)
	{
		iState = CONTROL_STATE_MOUSEOVER;
	}
	else if(IsFocus())
	{
		iState = CONTROL_STATE_FOCUS;
	}

	float fBlendRate = (iState == CONTROL_STATE_PRESSED) ? 0.0f : 0.8f;

	m_Style.Blend(iState, fElapsedTime, fBlendRate);
	//UIGraph::DrawSprite(m_rcTex, m_rcBoundingBox, m_nTexID, m_dwColor);
	UIGraph::DrawSprite(m_rcBoundingBox, m_nTexID, m_Style.m_crSpriteColor[0].getColor());
	//GetRenderSystem().SetTextureStageStateDecolor();
	//UIGraph::DrawSprite(m_rcBoundingBox, m_rcBoundingBox, m_nTexID, m_Style.m_crSpriteColor[1].c);
	//GetRenderSystem().SetupRenderState();
}

void CUIImage::SetFilename(const std::string& strFilename)
{
	m_strFilename = strFilename;
	m_nTexID = GetRenderSystem().GetTextureMgr().RegisterTexture(strFilename, 1);
}