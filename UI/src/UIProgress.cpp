#include "UIProgress.h"
#include "UIDialog.h"

CUIProgress::CUIProgress()
{
	m_Type = UI_CONTROL_PROGRESS;
	m_nMin = 0;
	m_nMax = 100;
	m_nValue = 50;
}

void CUIProgress::XMLParse(TiXmlElement* pControlElement)
{
	CUIControl::XMLParse(pControlElement);
}

void CUIProgress::UpdateRects()
{
	CUIControl::UpdateRects();
	m_rcButton = m_rcBoundingBox;
	int nButtonX = (int) ((m_nValue - m_nMin) * (float)RectWidth(m_rcBoundingBox) / (m_nMax - m_nMin));
	m_rcButton.right = m_rcButton.left + nButtonX;
}

bool CUIProgress::SetValue(int nValue)
{
	nValue = __max(m_nMin, nValue);
	nValue = __min(m_nMax, nValue);
	if(nValue == m_nValue)
		return false;
	m_nValue = nValue;
	UpdateRects();
	return true;
}

void CUIProgress::SetRange(int nMin, int nMax) 
{
	m_nMin = nMin;
	m_nMax = nMax;
	SetValue(m_nValue);
}

void CUIProgress::OnFrameRender(double fTime, float fElapsedTime)
{
	int nOffsetX = 0;
	int nOffsetY = 0;

	CONTROL_STATE iState = GetState();
	if(iState == CONTROL_STATE_PRESSED)
	{
		nOffsetX = 1;
		nOffsetY = 2;
	}
	else if(iState == CONTROL_STATE_MOUSEOVER)
	{
		nOffsetX = -1;
		nOffsetY = -2;
	}

	float fBlendRate = (iState == CONTROL_STATE_PRESSED) ? 0.0f : 0.8f;

	m_Style.draw(m_rcBoundingBox,L"",iState, fElapsedTime, fBlendRate);
	//m_StyleButton.draw(m_rcButton,m_wstrText,iState, fElapsedTime, fBlendRate);
}