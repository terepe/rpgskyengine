#include "UIComboBox.h"

CUIComboBox::CUIComboBox()
{
	m_Type = UI_CONTROL_COMBOBOX;

	m_nDropHeight = 100;

	m_nSBWidth = 16;
	m_bOpened = false;
	m_iSelected = -1;
	m_iFocused = -1;
	// Update the scrollbar's rects
	m_ScrollBar.m_width=m_nSBWidth;
	//m_ScrollBar.rcOffset;
	m_ScrollBar.m_nPercentHeight=100;
	m_ScrollBar.m_uAlign=ALIGN_RIGHT;
	//m_ScrollBar.SetLocation(m_rcDropdown.right, m_rcDropdown.top+2);
	//m_ScrollBar.SetSize(m_nSBWidth, RectHeight(m_rcDropdown)-2);
}

void CUIComboBox::SetParent(CUICombo *pControl)
{
	CUIControl::SetParent(pControl);
	m_ScrollBar.SetParent(pControl);
}

CUIComboBox::~CUIComboBox()
{
	RemoveAllItems();
}

void CUIComboBox::XMLParse(TiXmlElement* pControlElement)
{
	CUIButton::XMLParse(pControlElement);
	//
}

void CUIComboBox::SetStyle(const std::string& strStyleName)
{
	m_Style.SetStyle(strStyleName);
	m_StyleDropdown.SetStyle(strStyleName+".dropdown");
	m_StyleItem.SetStyle(strStyleName+".item");
	m_StyleSelected.SetStyle(strStyleName+".selected");
}

bool CUIComboBox::ContainsPoint(POINT pt) 
{ 
	return PtInRect(&m_rcBoundingBox, pt) || (m_bOpened && PtInRect(&m_rcDropdown, pt)); 
}

void CUIComboBox::OnSize(const RECT& rc)
{
	CUIButton::OnSize(rc);
	m_ScrollBar.OnSize(m_rcDropdown);
}

void CUIComboBox::UpdateRects()
{
	CUIButton::UpdateRects();

	m_rcDropdown = m_rcBoundingBox;
	OffsetRect(&m_rcDropdown, 0, (int) (0.90f * RectHeight(m_rcBoundingBox)));
	m_rcDropdown.bottom += m_nDropHeight;
	//m_rcDropdown.right -= m_nSBWidth;

	//if(pFontNode && pFontNode->nHeight)
	{
		m_ScrollBar.SetPageSize(RectHeight(m_rcDropdown) /*/ pFontNode->nHeight*/);

		// The selected item may have been scrolled off the page.
		// Ensure that it is in page again.
		m_ScrollBar.ShowItem(m_iSelected);
	}
}

void CUIComboBox::OnFocusOut()
{
	CUIButton::OnFocusOut();

	m_bOpened = false;
}

bool CUIComboBox::HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	const DWORD REPEAT_MASK = (0x40000000);

	if(!m_bEnabled || !m_bVisible)
		return false;

	// Let the scroll bar have a chance to handle it first
	if(m_ScrollBar.HandleKeyboard(uMsg, wParam, lParam))
		return true;

	switch(uMsg)
	{
	case WM_KEYDOWN:
		{
			switch(wParam)
			{
			case VK_RETURN:
				if(m_bOpened)
				{
					if(m_iSelected != m_iFocused)
					{
						m_iSelected = m_iFocused;
						SendEvent(EVENT_COMBOBOX_SELECTION_CHANGED, this);
					}
					m_bOpened = false;

					SetFocus(false);

					return true;
				}
				break;

			case VK_F4:
				// Filter out auto-repeats
				if(lParam & REPEAT_MASK)
					return true;

				m_bOpened = !m_bOpened;

				if(!m_bOpened)
				{
					SendEvent(EVENT_COMBOBOX_SELECTION_CHANGED, this);

					SetFocus(false);
				}

				return true;

			case VK_LEFT:
			case VK_UP:
				if(m_iFocused > 0)
				{
					m_iFocused--;
					m_iSelected = m_iFocused;

					if(!m_bOpened)
						SendEvent(EVENT_COMBOBOX_SELECTION_CHANGED, this);
				}

				return true;

			case VK_RIGHT:
			case VK_DOWN:
				if(m_iFocused+1 < (int)GetItemCount())
				{
					m_iFocused++;
					m_iSelected = m_iFocused;

					if(!m_bOpened)
						SendEvent(EVENT_COMBOBOX_SELECTION_CHANGED, this);
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
	if (m_ScrollBar.ContainsPoint(point))
	{
		m_ScrollBar.OnMouseMove(point);
		return;
	}
	//if(m_ScrollBar.HandleMouse(uMsg, point, wParam, lParam))
	//	return true;

	if(m_bOpened && PtInRect(&m_rcDropdown, point))
	{
		// Determine which item has been selected
		for(uint32 i=0; i < m_Items.size(); i++)
		{
			UIComboBoxItem* pItem = m_Items[i];
			if(pItem -> bVisible &&
				PtInRect(&pItem->rcActive, point))
			{
				m_iFocused = i;
			}
		}
	}
}

void CUIComboBox::OnLButtonDown(POINT point)
{
	if (m_ScrollBar.ContainsPoint(point))
	{
		m_ScrollBar.OnLButtonDown(point);
		return;
	}
	if(PtInRect(&m_rcBoundingBox, point))
	{
		// Pressed while inside the control
		SetPressed(true);
		SetCapture(UIGetHWND());

		SetFocus();

		// Toggle dropdown
		if(IsFocus())
		{
			m_bOpened = !m_bOpened;

			if(!m_bOpened)
			{
				SetFocus(false);
			}
		}
		return;
	}

	// Perhaps this click is within the dropdown
	if(m_bOpened && PtInRect(&m_rcDropdown, point))
	{
		// Determine which item has been selected
		for(uint32 i=m_ScrollBar.GetTrackPos(); i < m_Items.size(); i++)
		{
			UIComboBoxItem* pItem = m_Items[i];
			if(pItem -> bVisible &&
				PtInRect(&pItem->rcActive, point))
			{
				m_iFocused = m_iSelected = i;
				SendEvent(EVENT_COMBOBOX_SELECTION_CHANGED, this);
				m_bOpened = false;

				SetFocus(false);

				break;
			}
		}

		return;
	}

	// Mouse click not on main control or in dropdown, fire an event if needed
	if(m_bOpened)
	{
		m_iFocused = m_iSelected;

		SendEvent(EVENT_COMBOBOX_SELECTION_CHANGED, this);
		m_bOpened = false;
	}
}

void CUIComboBox::OnLButtonUp(POINT point)
{
	if (m_ScrollBar.ContainsPoint(point))
	{
		m_ScrollBar.OnLButtonUp(point);
		return;
	}
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
	if(m_bOpened)
	{
		UINT uLines;
		SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &uLines, 0);
		m_ScrollBar.Scroll(-zDelta * uLines);
	}
	else
	{
		if(zDelta > 0)
		{
			if(m_iFocused > 0)
			{
				m_iFocused--;
				m_iSelected = m_iFocused;     

				if(!m_bOpened)
					SendEvent(EVENT_COMBOBOX_SELECTION_CHANGED, this);
			}          
		}
		else
		{
			if(m_iFocused+1 < (int)GetItemCount())
			{
				m_iFocused++;
				m_iSelected = m_iFocused;   

				if(!m_bOpened)
					SendEvent(EVENT_COMBOBOX_SELECTION_CHANGED, this);
			}
		}
	}
}

void CUIComboBox::OnHotkey()
{
	if(m_bOpened)
		return;

	if(m_iSelected == -1)
		return;

	if(GetParentDialog()->IsKeyboardInputEnabled())
	{
		SetFocus();
	}

	m_iSelected++;

	if(m_iSelected >= (int) m_Items.size())
		m_iSelected = 0;

	m_iFocused = m_iSelected;
	SendEvent(EVENT_COMBOBOX_SELECTION_CHANGED, this);
}

void CUIComboBox::OnFrameRender(double fTime, float fElapsedTime)
{
	CONTROL_STATE iState = CONTROL_STATE_NORMAL;

	if(!m_bOpened)
		iState = CONTROL_STATE_HIDDEN;

	// If we have not initialized the scroll bar page size,
	// do that now.
	static bool bSBInit;
	if(!bSBInit)
	{
		// Update the page size of the scroll bar////////////////////////
		//if(m_pParentDialog->GetManager()->GetFontNode(pStyle->iFont)->nHeight)
		//	m_ScrollBar.SetPageSize(RectHeight(m_rcDropdown) / m_pParentDialog->GetManager()->GetFontNode(pStyle->iFont)->nHeight);
		//else
			m_ScrollBar.SetPageSize(RectHeight(m_rcDropdown));
		bSBInit = true;
	}

	// Scroll bar
	if(m_bOpened)
	{
		m_ScrollBar.OnFrameRender(fTime, fElapsedTime);
	}

	//Dropdown
	m_StyleDropdown.draw(m_rcDropdown,L"",iState, fElapsedTime);

	// Selection outline
	{
			int curY = m_rcDropdown.top;
			int nRemainingHeight = RectHeight(m_rcDropdown);
			//WCHAR strDropdown[4096] = {0};

			for(uint32 i = m_ScrollBar.GetTrackPos(); i < m_Items.size(); i++)
			{
				UIComboBoxItem* pItem = m_Items[i];

				// Make sure there's room left in the dropdown

				nRemainingHeight -= UIGraph::GetFontSize(); //pFont->nHeight???????????????????????????????????
				if(nRemainingHeight < 0)
				{
					pItem->bVisible = false;
					continue;
				}

				SetRect(&pItem->rcActive, m_rcDropdown.left, curY, m_rcDropdown.right, curY + UIGraph::GetFontSize());
				curY += UIGraph::GetFontSize()/*?????????????????/pFont->nHeight*/;

				pItem->bVisible = true;

				if(m_bOpened)
				{
					if((int)i == m_iFocused)
					{
						m_StyleSelected.draw(pItem->rcActive, pItem->wstrText,iState, fElapsedTime);
					}
					else
					{
						m_StyleItem.draw(pItem->rcActive, pItem->wstrText,iState, fElapsedTime);
					}
				}
			}
	}


	iState = CONTROL_STATE_NORMAL;

	if(m_bVisible == false)
		iState = CONTROL_STATE_HIDDEN;
	else if(m_bEnabled == false)
		iState = CONTROL_STATE_DISABLED;
	else if(IsPressed())
	{
		iState = CONTROL_STATE_PRESSED;
	}
	else if(m_bMouseOver)
	{
		iState = CONTROL_STATE_MOUSEOVER;
	}
	else if(IsFocus())
		iState = CONTROL_STATE_FOCUS;

	float fBlendRate = (iState == CONTROL_STATE_PRESSED) ? 0.0f : 0.8f;

	if(m_bOpened)
	{
		iState = CONTROL_STATE_PRESSED;
	}

	// Main text box
	std::wstring wstrTextSelected;
	if(m_iSelected >= 0 && m_iSelected < (int) m_Items.size())
	{
		UIComboBoxItem* pItem = m_Items[m_iSelected];
		if(pItem != NULL)
		{
			wstrTextSelected = pItem->wstrText;
		}
	}
	m_Style.draw(m_rcBoundingBox,wstrTextSelected,iState, fElapsedTime, fBlendRate);
}

bool CUIComboBox::AddItem(const std::wstring& wstrText, void* pData)
{
	// Create a new item and set the data
	UIComboBoxItem* pItem = new UIComboBoxItem;

	ZeroMemory(pItem, sizeof(UIComboBoxItem));

	pItem->wstrText = wstrText;
	pItem->pData = pData;

	m_Items.push_back(pItem);

	// Update the scroll bar with new range
	m_ScrollBar.SetTrackRange(0, m_Items.size());

	// If this is the only item in the list, it's selected
	if(GetItemCount() == 1)
	{
		m_iSelected = 0;
		m_iFocused = 0;
		SendEvent(EVENT_COMBOBOX_SELECTION_CHANGED, this);
	}

	return S_OK;
}

void CUIComboBox::RemoveItem(UINT index)
{
	UIComboBoxItem* pItem = m_Items[index];
	S_DEL(pItem);

	m_Items.erase(m_Items.begin()+index);

	m_ScrollBar.SetTrackRange(0, m_Items.size());
	if(m_iSelected >= m_Items.size())
		m_iSelected = m_Items.size() - 1;
}

void CUIComboBox::RemoveAllItems()
{
	for(uint32 i=0; i < m_Items.size(); i++)
	{
		UIComboBoxItem* pItem = m_Items[i];
		S_DEL(pItem);
	}

	m_Items.clear();
	m_ScrollBar.SetTrackRange(0, 1);
	m_iFocused = m_iSelected = -1;
}

bool CUIComboBox::ContainsItem(const std::wstring& wstrText, UINT iStart)
{
	return (-1 != FindItem(wstrText, iStart));
}

int CUIComboBox::FindItem(const std::wstring& wstrText, UINT iStart)
{
	for(uint32 i = iStart; i < m_Items.size(); i++)
	{
		UIComboBoxItem* pItem = m_Items[i];

		if(wstrText == pItem->wstrText)
		{
			return i;
		}
	}

	return -1;
}

void* CUIComboBox::GetSelectedData()
{
	UIComboBoxItem* pItem = GetSelectedItem();
	if(pItem)
	{
		return pItem->pData;
	}
	return NULL;
}

UIComboBoxItem* CUIComboBox::GetSelectedItem()
{
	if(m_iSelected < 0)
		return NULL;

	return m_Items[m_iSelected];
}

const std::wstring&	CUIComboBox::GetText()
{
	UIComboBoxItem* pItem = GetSelectedItem();
	if(pItem)
	{
		m_wstrText = pItem->wstrText;
	}
	return m_wstrText;
}

void* CUIComboBox::GetItemData(const std::wstring& wstrText)
{
	int index = FindItem(wstrText);
	if(index == -1)
	{
		return NULL;
	}

	UIComboBoxItem* pItem = m_Items[index];
	if(pItem == NULL)
	{
		return NULL;
	}

	return pItem->pData;
}

void* CUIComboBox::GetItemData(uint32 uIndex)
{
	if(uIndex >= m_Items.size())
		return NULL;
	return m_Items[uIndex]->pData;
}

bool CUIComboBox::SetSelectedByIndex(UINT index)
{
	if(index >= GetItemCount())
		return false;
	m_iFocused = m_iSelected = index;
	SendEvent(EVENT_COMBOBOX_SELECTION_CHANGED, this);
	return true;
}

bool CUIComboBox::SetSelectedByText(const std::wstring& wstrText)
{
	int index = FindItem(wstrText);
	if(index == -1)
		return false;
	m_iFocused = m_iSelected = index;
	SendEvent(EVENT_COMBOBOX_SELECTION_CHANGED, this);
	return true;
}

bool CUIComboBox::SetSelectedByData(void* pData)
{
	for(uint32 i=0; i < m_Items.size(); i++)
	{
		UIComboBoxItem* pItem = m_Items[i];

		if(pItem->pData == pData)
		{
			m_iFocused = m_iSelected = i;
			SendEvent(EVENT_COMBOBOX_SELECTION_CHANGED, this);
			return true;
		}
	}
	return false;
}