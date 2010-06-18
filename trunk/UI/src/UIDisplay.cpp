#include "UIDisplay.h"
#include "UIDialog.h"

CUIDisplay::CUIDisplay()
{
	m_Type = UI_CONTROL_DISPLAY;
}

void CUIDisplay::XMLParse(const TiXmlElement* pControlElement)
{
	CUIControl::XMLParse(pControlElement);
}

void CUIDisplay::OnFrameRender(double fTime, float fElapsedTime)
{
	m_Style.draw(m_rcBoundingBox,L"",GetState(),fElapsedTime);
}

CRect<int> CUIDisplay::getViewport()
{
	m_Style.m_mapStyleDrawData[0].updateRect(GetBoundingBox().getRECT());
	return m_Style.m_mapStyleDrawData[0].rc.getRECT();
}