#include "UISlider.h"
#include "UIDialog.h"


CUISlider::CUISlider()
{
	m_Type = UI_CONTROL_SLIDER;
	m_nMin = 0;
	m_nMax = 100;
	m_nValue = 50;
}

void CUISlider::XMLParse(TiXmlElement* pControlElement)
{
	CUIControl::XMLParse(pControlElement);
	//
}

void CUISlider::SetStyle(const std::string& strStyleName)
{
	m_Style.SetStyle(strStyleName);
	m_StyleButton.SetStyle(strStyleName+".button");
}

bool CUISlider::ContainsPoint(POINT pt) 
{ 
	return (PtInRect(&m_rcBoundingBox, pt) || 
		PtInRect(&m_rcButton, pt)); 
}

void CUISlider::UpdateRects()
{
	CUIControl::UpdateRects();

	m_rcButton = m_rcBoundingBox;
	m_rcButton.right = m_rcButton.left + RectHeight(m_rcButton);
	OffsetRect(&m_rcButton, -RectWidth(m_rcButton)/2, 0);

	m_nButtonX = (int) ((m_nValue - m_nMin) * (float)RectWidth(m_rcBoundingBox) / (m_nMax - m_nMin));
	OffsetRect(&m_rcButton, m_nButtonX, 0);
}

int CUISlider::ValueFromPos(int x)
{ 
	float fValuePerPixel = (float)(m_nMax - m_nMin) / RectWidth(m_rcBoundingBox);
	return (int) (0.5f + m_nMin + fValuePerPixel * (x - m_rcBoundingBox.left)) ; 
}


bool CUISlider::HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(!m_bEnabled || !m_bVisible)
		return false;

	switch(uMsg)
	{
	case WM_KEYDOWN:
		{
			switch(wParam)
			{
			case VK_HOME:
				SetValueInternal(m_nMin);
				return true;

			case VK_END:
				SetValueInternal(m_nMax);
				return true;

			case VK_LEFT:
			case VK_DOWN:
				SetValueInternal(m_nValue - 1);
				return true;

			case VK_RIGHT:
			case VK_UP:
				SetValueInternal(m_nValue + 1);
				return true;

			case VK_NEXT:
				SetValueInternal(m_nValue - (10 > (m_nMax - m_nMin) / 10 ? 10 : (m_nMax - m_nMin) / 10));
				return true;

			case VK_PRIOR:
				SetValueInternal(m_nValue + (10 > (m_nMax - m_nMin) / 10 ? 10 : (m_nMax - m_nMin) / 10));
				return true;
			}
			break;
		}
	}


	return false;
}

void CUISlider::OnMouseMove(POINT pt)
{
	if(IsPressed())
	{
		SetValueInternal(ValueFromPos(m_rcBoundingBox.left+pt.x+m_nDragOffset));
	}
}

void CUISlider::OnMouseWheel(POINT point,short wheelDelta)
{
	int nScrollAmount = int(wheelDelta) / WHEEL_DELTA;
	SetValueInternal(m_nValue - nScrollAmount);
}

void CUISlider::OnLButtonDown(POINT point)
{
	if(PtInRect(&m_rcButton, point))
	{
		// Pressed while inside the control
		SetPressed(true);
		SetCapture(UIGetHWND());

		//m_nDragX = pt.x;
		m_nDragOffset = m_nButtonX - point.x;
		SetFocus();
		return;
	}
	if(PtInRect(&m_rcBoundingBox, point))
	{
		//m_nDragX = point.x;
		m_nDragOffset = 0;
		//SetPressed(true);
		SetFocus();
		if(point.x > m_nButtonX + m_rcBoundingBox.left)
		{
			SetValueInternal(m_nValue + 1);
			return;
		}
		if(point.x < m_nButtonX + m_rcBoundingBox.left)
		{
			SetValueInternal(m_nValue - 1);
			return;
		}
	}
}

void CUISlider::OnLButtonUp(POINT point)
{
	if(IsPressed())
	{
		SetPressed(false);
		ReleaseCapture();
		SendEvent(EVENT_SLIDER_VALUE_CHANGED, this);
	}
}

bool CUISlider::SetValue(int nValue)
{
	nValue = __max(m_nMin, nValue);
	nValue = __min(m_nMax, nValue);
	if(nValue == m_nValue)
		return false;
	m_nValue = nValue;
	UpdateRects();
	return true;
}

void CUISlider::SetRange(int nMin, int nMax) 
{
	m_nMin = nMin;
	m_nMax = nMax;
	SetValue(m_nValue);
}

void CUISlider::SetValueInternal(int nValue)
{
	if (SetValue(nValue))
	{
		SendEvent(EVENT_SLIDER_VALUE_CHANGED, this);
	}
}

void CUISlider::OnFrameRender(double fTime, float fElapsedTime)
{
	CONTROL_STATE iState = CONTROL_STATE_NORMAL;

	if(m_bVisible == false)
	{
		iState = CONTROL_STATE_HIDDEN;
	}
	else if(m_bEnabled == false)
	{
		iState = CONTROL_STATE_DISABLED;
	}
	else if(IsPressed())
	{
		iState = CONTROL_STATE_PRESSED;
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

	m_Style.draw(m_rcBoundingBox,L"",iState, fElapsedTime, fBlendRate);
	m_StyleButton.draw(m_rcButton,L"",iState, fElapsedTime, fBlendRate);
}