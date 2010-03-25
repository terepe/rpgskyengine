#include "UIDisplay.h"
#include "UIDialog.h"

CUIDisplay::CUIDisplay()
{
	m_Type = UI_CONTROL_DISPLAY;
}

void CUIDisplay::XMLParse(TiXmlElement* pControlElement)
{
	CUIControl::XMLParse(pControlElement);
}

void CUIDisplay::OnFrameRender(double fTime, float fElapsedTime)
{
	m_Style.draw(m_rcBoundingBox,L"",GetState(),fElapsedTime);
}

CRect<int> CUIDisplay::getViewport()
{
	CRect<float> scale = m_Style.m_mapSquare[0].scale;

	CRect<int> rcViewport = GetBoundingBox();
	rcViewport.left+= rcViewport.getWidth()*0.5f*(1.0f-scale.left);
	rcViewport.right-=rcViewport.getWidth()*0.5f*(1.0f-scale.right);
	rcViewport.top+= rcViewport.getHeight()*0.5f*(1.0f-scale.top);
	rcViewport.bottom-=rcViewport.getHeight()*0.5f*(1.0f-scale.bottom);
	return rcViewport;
}