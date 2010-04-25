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
	CRect<float> scale = m_Style.m_mapDefault[0].scale;

	CRect<int> rcViewport = GetBoundingBox();
	int nWidth = rcViewport.getWidth()*0.5f;
	int nHeight = rcViewport.getHeight()*0.5f;
	rcViewport.left		+= nWidth*(1.0f-scale.left);
	rcViewport.right	-= nWidth*(1.0f-scale.right);
	rcViewport.top		+= nHeight*(1.0f-scale.top);
	rcViewport.bottom	-= nHeight*(1.0f-scale.bottom);
	return rcViewport;
}