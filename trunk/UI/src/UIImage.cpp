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

	m_Style.Blend(m_rcBoundingBox, GetState(), fElapsedTime);
	//UIGraph::DrawSprite(m_rcTex, m_rcBoundingBox, m_nTexID, m_dwColor);
	UIGraph::DrawSprite(m_rcBoundingBox, m_nTexID, m_Style.m_mapSprite[0].vColor.getColor());
	//GetRenderSystem().SetTextureStageStateDecolor();
	//UIGraph::DrawSprite(m_rcBoundingBox, m_rcBoundingBox, m_nTexID, m_Style.m_crSpriteColor[1].c);
	//GetRenderSystem().SetupRenderState();
}

void CUIImage::SetFilename(const std::string& strFilename)
{
	m_strFilename = strFilename;
	m_nTexID = GetRenderSystem().GetTextureMgr().RegisterTexture(strFilename, 1);
}