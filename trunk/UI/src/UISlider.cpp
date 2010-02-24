#include "UISlider.h"
#include "UIDialog.h"


// Minimum scroll bar thumb size
#define SCROLLBAR_MINTHUMBSIZE 17

CUISlider::CUISlider()
{
	m_Type = UI_CONTROL_SLIDER;
	m_nMin = 0;
	m_nMax = 100;
	m_nValue = 50;
	m_bH = true;
	m_nPageSize = 1;
}

void CUISlider::SetStyle(const std::string& strStyleName)
{
	m_Style.SetStyle(strStyleName);
	m_StyleButton.SetStyle(strStyleName+".button");
}

void CUISlider::UpdateRects()
{
	CUIControl::UpdateRects();
	m_bH = RectWidth(m_rcBoundingBox)>RectHeight(m_rcBoundingBox);
	m_rcButton = m_rcBoundingBox;
	if(m_bH)
	{
		int nRangeLength = m_nMax-m_nMin;
		int nButtonSize = __max(RectWidth(m_rcBoundingBox)*m_nPageSize/(nRangeLength+m_nPageSize),SCROLLBAR_MINTHUMBSIZE);
		m_rcButton.right = m_rcButton.left + nButtonSize;
		if(nRangeLength>0)
		{
			int nButtonX = (int) ((m_nValue - m_nMin) * (float)(RectWidth(m_rcBoundingBox)-nButtonSize)/nRangeLength);
			OffsetRect(&m_rcButton, nButtonX, 0);
		}
	}
	else
	{
		int nRangeLength = m_nMax-m_nMin;
		int nButtonSize = __max(RectHeight(m_rcBoundingBox)*m_nPageSize/(nRangeLength+m_nPageSize),SCROLLBAR_MINTHUMBSIZE);
		m_rcButton.bottom = m_rcButton.top + nButtonSize;
		if(nRangeLength>0)
		{
			int nButtonY = (int) ((m_nValue - m_nMin) * (float)(RectHeight(m_rcBoundingBox)-nButtonSize)/nRangeLength);
			OffsetRect(&m_rcButton, 0, nButtonY);
		}
	}
}

int CUISlider::ValueFromPos(POINT pt)
{
	if(m_bH)
	{
		int nButtonSize = RectWidth(m_rcButton);
		float fValuePerPixel = (float)(m_nMax - m_nMin)/(float)(RectWidth(m_rcBoundingBox)-nButtonSize);
		return (int) (0.5f + m_nMin + fValuePerPixel * (pt.x - m_rcBoundingBox.left-nButtonSize));
	}
	else
	{
		int nButtonSize = RectHeight(m_rcButton);
		float fValuePerPixel = (float)(m_nMax - m_nMin)/(float)(RectHeight(m_rcBoundingBox)-nButtonSize);
		return (int) (0.5f + m_nMin + fValuePerPixel * (pt.y - m_rcBoundingBox.top-nButtonSize));
	}
	return 0;
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
			case VK_UP:
				SetValueInternal(m_nValue - 1);
				return true;

			case VK_RIGHT:
			case VK_DOWN:
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
		pt.x+=m_ptDragOffset.x;
		pt.y+=m_ptDragOffset.y;
		SetValueInternal(ValueFromPos(pt));
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
		m_ptDragOffset.x = m_rcButton.right - point.x;
		m_ptDragOffset.y = m_rcButton.bottom - point.y;
		SetFocus();
		return;
	}
	if(PtInRect(&m_rcBoundingBox, point))
	{
		m_ptDragOffset.x=0;
		m_ptDragOffset.y=0;
		//SetPressed(true);
		SetFocus();
		if(m_bH)
		{
			if(point.x > m_rcButton.left)
			{
				SetValueInternal(m_nValue + __max(m_nPageSize-1,1));
				return;
			}
			if(point.x < m_rcButton.right)
			{
				SetValueInternal(m_nValue - __max(m_nPageSize-1,1));
				return;
			}
		}
		else
		{
			if(point.y > m_rcButton.top)
			{
				SetValueInternal(m_nValue + m_nPageSize - 1);
				return;
			}
			if(point.y < m_rcButton.bottom)
			{
				SetValueInternal(m_nValue - m_nPageSize + 1);
				return;
			}
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
	UpdateRects();
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
	CONTROL_STATE iState = GetState();
	m_Style.draw(m_rcBoundingBox,L"",iState, fElapsedTime);
	m_StyleButton.draw(m_rcButton,L"",iState, fElapsedTime);
}