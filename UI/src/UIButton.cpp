#include "UIButton.h"

CUIButton::CUIButton()
{
	m_Type = UI_CONTROL_BUTTON;
	m_nHotkey = 0;
}

void CUIButton::OnPressDown()
{
}

void CUIButton::OnPressUp()
{
	SendEvent(EVENT_BUTTON_CLICKED, this);
}

bool CUIButton::HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(!m_bEnabled || !m_bVisible)
		return false;

	switch(uMsg)
	{
	case WM_KEYDOWN:
		{
			switch(wParam)
			{
			case VK_SPACE:
				SetPressed(true);
				OnPressDown();
				return true;
			}
		}
	case WM_KEYUP:
		{
			switch(wParam)
			{
			case VK_SPACE:
				if(IsPressed())
				{
					SetPressed(false);
					OnPressUp();
				}
				return true;
			}
		}
	}
	return false;
}

void CUIButton::OnLButtonDown(POINT point)
{
	SetPressed(true);
	SetCapture(UIGetHWND());
	SetFocus();
}

void CUIButton::OnLButtonDblClk(POINT point)
{
	SendEvent(EVENT_BUTTON_DBLCLK, this);
}

void CUIButton::OnLButtonUp(POINT point)
{
	if(IsPressed())
	{
		SetPressed(false);
		ReleaseCapture();
		// Button click
		OnPressUp();
	}
}

void CUIButton::OnHotkey()
{
	if(GetParentDialog()->IsKeyboardInputEnabled())
		SetFocus();
	OnPressUp();
}

bool CUIButton::ContainsPoint(POINT pt)
{
	return PtInRect(&m_rcBoundingBox, pt)==TRUE;
}

void CUIButton::OnFrameRender(double fTime, float fElapsedTime)
{
	CONTROL_STATE iState = GetState();
	float fBlendRate = (iState == CONTROL_STATE_PRESSED) ? 0.0f : 0.8f;
	m_Style.draw(m_rcBoundingBox,m_wstrText,iState, fElapsedTime, fBlendRate);
	//if (iState == CONTROL_STATE_PRESSED)
	//{
	//	OffsetRect(&rcText, 1, 2);
	//}
}