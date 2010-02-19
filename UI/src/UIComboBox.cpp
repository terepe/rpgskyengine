#include "UIComboBox.h"

CUIComboBox::CUIComboBox()
{
	m_Type = UI_CONTROL_COMBOBOX;
}

CUIComboBox::~CUIComboBox()
{
}

void CUIComboBox::OnControlRegister()
{
	CUICombo::OnControlRegister();
	RegisterControl("IDC_LISTBOX",m_ListBox);
	RegisterControlEvent("IDC_LISTBOX",(PEVENT)&CUIComboBox::OnSelectionChanged);

	m_ListBox.SetVisible(false);
}

void CUIComboBox::OnSelectionChanged()
{
	if(!m_ListBox.IsVisible())
		SendEvent(EVENT_COMBOBOX_SELECTION_CHANGED, this);
}

bool CUIComboBox::ContainsPoint(POINT pt) 
{ 
	return PtInRect(&m_rcBoundingBox, pt) || (m_ListBox.IsVisible() && m_ListBox.ContainsPoint(pt)); 
}

void CUIComboBox::OnFocusOut()
{
	CUICombo::OnFocusOut();
	m_ListBox.SetVisible(false);
}

bool CUIComboBox::HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	const DWORD REPEAT_MASK = (0x40000000);

	if(!m_bEnabled || !m_bVisible)
		return false;

	// Let the scroll bar have a chance to handle it first
	if(CUIComboBox::HandleKeyboard(uMsg, wParam, lParam))
		return true;

	switch(uMsg)
	{
	case WM_KEYDOWN:
		{
			switch(wParam)
			{
			case VK_RETURN:
				if(m_ListBox.IsVisible())
				{
					// hh 2010-2-27 It's no idea.
					//if(m_iSelected != m_iFocused)
					//{
					//	m_iSelected = m_iFocused;
					//	SendEvent(EVENT_COMBOBOX_SELECTION_CHANGED, this);
					//}
					m_ListBox.SetVisible(false);
					SetFocus(false);

					return true;
				}
				break;

			case VK_F4:
				// Filter out auto-repeats
				if(lParam & REPEAT_MASK)
					return true;
				m_ListBox.SetVisible(!m_ListBox.IsVisible());
				if(!m_ListBox.IsVisible())
				{
					SendEvent(EVENT_COMBOBOX_SELECTION_CHANGED, this);
					SetFocus(false);
				}
				return true;
			case VK_LEFT:
			case VK_UP:
				if(!m_ListBox.IsVisible())
				{
					m_ListBox.SelectItem(m_ListBox.GetSelectedIndex()-1);
				}
				return true;
			case VK_RIGHT:
			case VK_DOWN:
				if(!m_ListBox.IsVisible())
				{
					m_ListBox.SelectItem(m_ListBox.GetSelectedIndex()+1);
				}
				return true;
			}
			break;
		}
	}
	return false;
}

void CUIComboBox::OnMouseMove(POINT point)
{
}

void CUIComboBox::OnLButtonDown(POINT point)
{
	CUICombo::OnLButtonDown(point);
	if(PtInRect(&m_rcBoundingBox, point))
	{
		// Pressed while inside the control
		SetPressed(true);
		SetCapture(UIGetHWND());

		SetFocus();

		// Toggle dropdown
		if(IsFocus())
		{
			m_ListBox.SetVisible(!m_ListBox.IsVisible());

			if(!m_ListBox.IsVisible())
			{
				SetFocus(false);
			}
		}
		return;
	}

	// Mouse click not on main control or in dropdown, fire an event if needed
	if(m_ListBox.IsVisible())
	{
		if(!m_ListBox.ContainsPoint(point))
		{
			SendEvent(EVENT_COMBOBOX_SELECTION_CHANGED, this);
			m_ListBox.SetVisible(false);
		}
	}
}

void CUIComboBox::OnLButtonUp(POINT point)
{
	CUICombo::OnLButtonUp(point);
	if(IsPressed() && ContainsPoint(point))
	{
		// Button click
		SetPressed(false);
		ReleaseCapture();
	}
}

void CUIComboBox::OnMouseWheel(POINT point,short wheelDelta)
{
	int zDelta = wheelDelta / WHEEL_DELTA;
	if(m_ListBox.IsVisible())
	{
		m_ListBox.OnMouseWheel(point,wheelDelta);
	}
	else
	{
		if(zDelta > 0)
		{
			m_ListBox.SelectItem(m_ListBox.GetSelectedIndex()-1);
		}
		else
		{
			m_ListBox.SelectItem(m_ListBox.GetSelectedIndex()+1);
		}
	}
}

void CUIComboBox::OnHotkey()
{
	if(m_ListBox.IsVisible())
		return;

	if(m_ListBox.GetItemCount()==0)
		return;

	if(GetParentDialog()->IsKeyboardInputEnabled())
	{
		SetFocus();
	}

	if(m_ListBox.GetSelectedIndex()>=m_ListBox.GetItemCount()-1)
	{
		m_ListBox.SelectItem(0);
	}
	else
	{
		m_ListBox.SelectItem(m_ListBox.GetSelectedIndex()+1);
	}
}

void CUIComboBox::OnFrameRender(double fTime, float fElapsedTime)
{
	CUICombo::OnFrameRender(fTime, fElapsedTime);

	CONTROL_STATE iState = GetState();
	float fBlendRate = (iState == CONTROL_STATE_PRESSED) ? 0.0f : 0.8f;

	if(m_ListBox.IsVisible())
	{
		iState = CONTROL_STATE_PRESSED;
	}

	// Main text box
	GetText(); // For Temp
	m_Style.draw(m_rcBoundingBox,m_wstrText,iState, fElapsedTime, fBlendRate);
}

const std::wstring&	CUIComboBox::GetText()
{
	UIListBoxItem* pItem = m_ListBox.GetSelectedItem();
	if(pItem)
	{
		m_wstrText = pItem->wstrText;
	}
	return m_wstrText;
}

CUIListBox& CUIComboBox::getListBox()
{
	return m_ListBox;
}