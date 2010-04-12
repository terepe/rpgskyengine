#include "UIListBox.h"
#include "tinyxml.h"

CUIListBox::CUIListBox()
{
	m_dwStyle = 0;
	m_nSBWidth = 17;
	m_nSelected = -1;
	m_nSelStart = 0;
	m_nBorder = 6;
	m_nMargin = 5;
	m_nTextHeight = 0;
}

CUIListBox::~CUIListBox()
{
	RemoveAllItems();
}

void CUIListBox::OnControlRegister()
{
	CUICombo::OnControlRegister();
	RegisterControl("IDC_SCROLLBAR_LEFT",m_ScrollBar);
}

void CUIListBox::SetStyle(const std::string& strStyleName)
{
	m_Style.SetStyle(strStyleName);
	m_StyleItem.SetStyle(strStyleName+".item");
	m_StyleSelected.SetStyle(strStyleName+".selected");
}

void CUIListBox::UpdateRects()
{
	CUICombo::UpdateRects();

	m_rcSelection = m_rcBoundingBox;
	m_rcSelection.InflateRect(-m_nBorder, -m_nBorder);
	m_rcSelection.right -= m_nSBWidth+m_nBorder;
	m_rcText = m_rcSelection;
	m_rcText.InflateRect(-m_nMargin, 0);

	// Update the scrollbar's rects
	//if(pFontNode && pFontNode->nHeight)
	{
		m_ScrollBar.SetPageSize(m_rcText.getHeight() / UIGraph::GetFontSize());

		// The selected item may have been scrolled off the page.
		// Ensure that it is in page again.
		m_ScrollBar.ShowItem(m_nSelected);
	}
}

bool CUIListBox::AddItem(const std::wstring& wstrText, void *pData)
{
	return InsertItem(m_Items.size(), wstrText, pData);
}

bool CUIListBox::InsertItem(int nIndex, const std::wstring& wstrText, void *pData)
{
	UIListBoxItem *pNewItem = new UIListBoxItem;
	if(!pNewItem)
		return false;

	pNewItem->wstrText = wstrText;
	pNewItem->pData = pData;
	SetRect(&pNewItem->rcActive, 0, 0, 0, 0);
	pNewItem->bSelected = false;

	m_Items.insert(m_Items.begin()+nIndex, pNewItem);

	m_ScrollBar.SetTrackRange(0, m_Items.size());

	return true;
}

void CUIListBox::RemoveItem(int nIndex)
{
	if(nIndex < 0 || nIndex >= (int)m_Items.size())
		return;

	UIListBoxItem *pItem = m_Items[nIndex];

	delete pItem;
	m_Items.erase(m_Items.begin()+nIndex);
	m_ScrollBar.SetTrackRange(0, m_Items.size());
	if(m_nSelected >= (int)m_Items.size())
		m_nSelected = m_Items.size() - 1;

	SendEvent(EVENT_LISTBOX_SELECTION, this);
}

void CUIListBox::RemoveItemByText(WCHAR *wszText)
{
}

void CUIListBox::RemoveItemByData(void *pData)
{
}

void CUIListBox::RemoveAllItems()
{
	for(uint32 i = 0; i < m_Items.size(); ++i)
	{
		UIListBoxItem *pItem = m_Items[i];
		delete pItem;
	}
	m_nSelected = -1;
	m_Items.clear();
	m_ScrollBar.SetTrackRange(0, 1);
}

UIListBoxItem* CUIListBox::GetItem(int nIndex)
{
	if(nIndex < 0 || nIndex >= (int)m_Items.size())
		return NULL;

	return m_Items[nIndex];
}

const std::wstring& CUIListBox::GetItemText(int nInde)
{
	UIListBoxItem* pItem = GetItem(nInde);
	if (pItem)
	{
		return pItem->wstrText;
	}
	static const std::wstring wstrNULL=L"";
	return wstrNULL;
}

// For single-selection listbox, returns the index of the selected item.
// For multi-selection, returns the first selected item after the nPreviousSelected position.
// To search for the first selected item, the app passes -1 for nPreviousSelected.  For
// subsequent searches, the app passes the returned index back to GetSelectedIndex as.
// nPreviousSelected.
// Returns -1 on error or if no item is selected.
int CUIListBox::GetSelectedIndex(int nPreviousSelected)
{
	if(nPreviousSelected < -1)
		return -1;

	if(m_dwStyle & MULTISELECTION)
	{
		// Multiple selection enabled. Search for the next item with the selected flag.
		for(int i = nPreviousSelected + 1; i < (int)m_Items.size(); ++i)
		{
			UIListBoxItem *pItem = m_Items[i];

			if(pItem->bSelected)
				return i;
		}

		return -1;
	}
	else
	{
		// Single selection
		return m_nSelected;
	}
}

void* CUIListBox::GetSelectedData(int nPreviousSelected)
{
	UIListBoxItem* pUIListBoxItem = GetSelectedItem(nPreviousSelected);
	if (pUIListBoxItem)
	{
		return pUIListBoxItem->pData;
	}
	return NULL;
}

UIListBoxItem* CUIListBox::GetSelectedItem(int nPreviousSelected)
{
	return GetItem(GetSelectedIndex(nPreviousSelected));
}

bool CUIListBox::ContainsItem(const std::wstring& wstrText, UINT iStart)
{
	return (-1 != FindItem(wstrText, iStart));
}

int CUIListBox::FindItem(const std::wstring& wstrText, size_t iStart)
{
	for(size_t i = iStart; i < m_Items.size(); i++)
	{
		UIListBoxItem* pItem = m_Items[i];
		if(wstrText == pItem->wstrText)
		{
			return i;
		}
	}
	return -1;
}

bool CUIListBox::SelectItem(int nNewIndex)
{
	// If no item exists, do nothing.
	if(m_Items.size() == 0)
		return false;

	int nOldSelected = m_nSelected;

	// Adjust m_nSelected
	m_nSelected = nNewIndex;

	// Perform capping
	if(m_nSelected < 0)
		m_nSelected = 0;
	if(m_nSelected >= (int)m_Items.size())
		m_nSelected = m_Items.size() - 1;

	if(nOldSelected != m_nSelected)
	{
		if(m_dwStyle & MULTISELECTION)
		{
			m_Items[m_nSelected]->bSelected = true;
		}

		// Update selection start
		m_nSelStart = m_nSelected;

		// Adjust scroll bar
		m_ScrollBar.ShowItem(m_nSelected);
	}
	SendEvent(EVENT_LISTBOX_SELECTION, this);
	return true;
}

void* CUIListBox::getItemDataByText(const std::wstring& wstrText)
{
	int index = FindItem(wstrText);
	return getItemDataByIndex(index);
}

void* CUIListBox::getItemDataByIndex(size_t index)
{
	UIListBoxItem* pItem=GetItem(index);
	if(pItem)
	{
		return pItem->pData;
	}
	return NULL;
}

bool CUIListBox::selectByText(const std::wstring& wstrText)
{
	int index = FindItem(wstrText);
	if(index == -1)
		return false;
	return SelectItem(index);
}

bool CUIListBox::selectByData(void* pData)
{
	for(size_t i=0; i < m_Items.size(); ++i)
	{
		UIListBoxItem* pItem = m_Items[i];
		if(pItem->pData == pData)
		{
			return SelectItem(i);
		}
	}
	return false;
}

bool CUIListBox::HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(!m_bEnabled || !m_bVisible)
		return false;

	// Let the scroll bar have a chance to handle it first
	if(m_ScrollBar.HandleKeyboard(uMsg, wParam, lParam))
		return true;

	switch(uMsg)
	{
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_UP:
		case VK_DOWN:
		case VK_NEXT:
		case VK_PRIOR:
		case VK_HOME:
		case VK_END:
			{
				// If no item exists, do nothing.
				if(m_Items.size() == 0)
					return true;

				int nOldSelected = m_nSelected;

				// Adjust m_nSelected
				switch(wParam)
				{
				case VK_UP: --m_nSelected; break;
				case VK_DOWN: ++m_nSelected; break;
				case VK_NEXT: m_nSelected += m_ScrollBar.GetPageSize() - 1; break;
				case VK_PRIOR: m_nSelected -= m_ScrollBar.GetPageSize() - 1; break;
				case VK_HOME: m_nSelected = 0; break;
				case VK_END: m_nSelected = m_Items.size() - 1; break;
				}

				// Perform capping
				if(m_nSelected < 0)
					m_nSelected = 0;
				if(m_nSelected >= (int)m_Items.size())
					m_nSelected = m_Items.size() - 1;

				if(nOldSelected != m_nSelected)
				{
					if(m_dwStyle & MULTISELECTION)
					{
						// Multiple selection

						// Clear all selection
						for(int i = 0; i < (int)m_Items.size(); ++i)
						{
							UIListBoxItem *pItem = m_Items[i];
							pItem->bSelected = false;
						}

						if(GetKeyState(VK_SHIFT) < 0)
						{
							// Select all items from m_nSelStart to
							// m_nSelected
							int nEnd = __max(m_nSelStart, m_nSelected);

							for(int n = __min(m_nSelStart, m_nSelected); n <= nEnd; ++n)
								m_Items[n]->bSelected = true;
						}
						else
						{
							m_Items[m_nSelected]->bSelected = true;

							// Update selection start
							m_nSelStart = m_nSelected;
						}
					} else
						m_nSelStart = m_nSelected;

					// Adjust scroll bar

					m_ScrollBar.ShowItem(m_nSelected);

					// Send notification

					SendEvent(EVENT_LISTBOX_SELECTION, this);
				}
				return true;
			}

			// Space is the hotkey for double-clicking an item.
			//
		case VK_SPACE:
			SendEvent(EVENT_LISTBOX_ITEM_DBLCLK, this);
			return true;
		}
		break;
	}

	return false;
}

void CUIListBox::OnMouseMove(POINT point)
{
	CUICombo::OnMouseMove(point);
	if (m_ScrollBar.ContainsPoint(point))
	{
		return;
	}
	if(IsPressed())
	{
		// Compute the index of the item below cursor

		int nItem;
		if(m_nTextHeight)
			nItem = m_ScrollBar.GetTrackPos() + (point.y - m_rcText.top) / m_nTextHeight;
		else
			nItem = -1;

		// Only proceed if the cursor is on top of an item.

		if(nItem >= (int)m_ScrollBar.GetTrackPos() &&
			nItem < (int)m_Items.size() &&
			nItem < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize())
		{
			m_nSelected = nItem;
			SendEvent(EVENT_LISTBOX_SELECTION, this);
		}
		else if(nItem < (int)m_ScrollBar.GetTrackPos())
		{
			// User drags the mouse above window top
			m_ScrollBar.Scroll(-1);
			m_nSelected = m_ScrollBar.GetTrackPos();
			SendEvent(EVENT_LISTBOX_SELECTION, this);
		}
		else if(nItem >= m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize())
		{
			// User drags the mouse below window bottom
			m_ScrollBar.Scroll(1);
			m_nSelected = __min((int)m_Items.size(), m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize()) - 1;					SendEvent(EVENT_LISTBOX_SELECTION, this);
		}
	}
}

void CUIListBox::OnMouseWheel(POINT point,short wheelDelta)
{
	UINT uLines;
	SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &uLines, 0);
	int nScrollAmount = int(wheelDelta) / WHEEL_DELTA * uLines;
	m_ScrollBar.Scroll(-nScrollAmount);
}

int CUIListBox::getItemIndexByPoint(POINT point)
{
	if(m_Items.size() > 0 && m_rcSelection.ptInRect(point))
	{
		if(m_nTextHeight)
		{
			int index = m_ScrollBar.GetTrackPos() + (point.y - m_rcText.top) / m_nTextHeight;
			if(index >= m_ScrollBar.GetTrackPos() &&
				index < (int)m_Items.size() &&
				index < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize())
			{
				return index;
			}
		}
	}
	return -1;
}

int	CUIListBox::getItemIndexByData(void* pData)
{
	for (size_t i=0;i<m_Items.size();++i)
	{
		if (m_Items[i]->pData==pData)
		{
			return i;
		}
	}
	return -1;
}

void CUIListBox::OnLButtonDblClk(POINT point)
{
	SetFocus();
	int nClicked = getItemIndexByPoint(point);
	if (nClicked!=-1)
	{
		m_nSelected = nClicked;
		SendEvent(EVENT_LISTBOX_ITEM_DBLCLK, this);
	}
}

void CUIListBox::OnLButtonDown(POINT point)
{
	SetFocus();
	if (m_ScrollBar.ContainsPoint(point))
	{
		m_ScrollBar.OnLButtonDown(point);
		return;
	}
	int nClicked = getItemIndexByPoint(point);
	if (nClicked!=-1)
	{
		SetCapture(UIGetHWND());
		SetPressed(true);
		m_nSelected = nClicked;

			//if(!(wParam & MK_SHIFT))??????????????????
			//	m_nSelStart = m_nSelected;??????????????????

			// If this is a multi-selection listbox, update per-item
			// selection data.

			/**if(m_dwStyle & MULTISELECTION)
			{
				// Determine behavior based on the state of Shift and Ctrl

				UIListBoxItem *pSelItem = m_Items[m_nSelected];
				if((wParam & (MK_SHIFT|MK_CONTROL)) == MK_CONTROL)
				{
					// Control click. Reverse the selection of this item.

					pSelItem->bSelected = !pSelItem->bSelected;
				}
				else if((wParam & (MK_SHIFT|MK_CONTROL)) == MK_SHIFT)
				{
					// Shift click. Set the selection for all items
					// from last selected item to the current item.
					// Clear everything else.

					int nBegin = __min(m_nSelStart, m_nSelected);
					int nEnd = __max(m_nSelStart, m_nSelected);

					for(int i = 0; i < nBegin; ++i)
					{
						UIListBoxItem *pItem = m_Items[i];
						pItem->bSelected = false;
					}

					for(int i = nEnd + 1; i < (int)m_Items.size(); ++i)
					{
						UIListBoxItem *pItem = m_Items[i];
						pItem->bSelected = false;
					}

					for(int i = nBegin; i <= nEnd; ++i)
					{
						UIListBoxItem *pItem = m_Items[i];
						pItem->bSelected = true;
					}
				}
				else if((wParam & (MK_SHIFT|MK_CONTROL)) == (MK_SHIFT|MK_CONTROL))
				{
					// Control-Shift-click.

					// The behavior is:
					//   Set all items from m_nSelStart to m_nSelected to
					//     the same state as m_nSelStart, not including m_nSelected.
					//   Set m_nSelected to selected.

					int nBegin = __min(m_nSelStart, m_nSelected);
					int nEnd = __max(m_nSelStart, m_nSelected);

					// The two ends do not need to be set here.

					bool bLastSelected = m_Items[m_nSelStart]->bSelected;
					for(int i = nBegin + 1; i < nEnd; ++i)
					{
						UIListBoxItem *pItem = m_Items[i];
						pItem->bSelected = bLastSelected;
					}

					pSelItem->bSelected = true;

					// Restore m_nSelected to the previous value
					// This matches the Windows behavior

					m_nSelected = m_nSelStart;
				}
				else
				{
					// Simple click.  Clear all items and select the clicked
					// item.


					for(int i = 0; i < (int)m_Items.size(); ++i)
					{
						UIListBoxItem *pItem = m_Items[i];
						pItem->bSelected = false;
					}

					pSelItem->bSelected = true;
				}
			}*/  // End of multi-selection case

			SendEvent(EVENT_LISTBOX_SELECTION, this);
	}
}

void CUIListBox::OnLButtonUp(POINT point)
{
	if (m_ScrollBar.ContainsPoint(point))
	{
		m_ScrollBar.OnLButtonUp(point);
		return;
	}
	ReleaseCapture();
	SetPressed(false);

	if(m_nSelected != -1)
	{
		// Set all items between m_nSelStart and m_nSelected to
		// the same state as m_nSelStart
		int nEnd = __max(m_nSelStart, m_nSelected);
		int nBegin = __min(m_nSelStart, m_nSelected);

		for(int n = nBegin + 1; n < nEnd; ++n)
			m_Items[n]->bSelected = m_Items[m_nSelStart]->bSelected;
		m_Items[m_nSelected]->bSelected = m_Items[m_nSelStart]->bSelected;

		// If m_nSelStart and m_nSelected are not the same,
		// the user has dragged the mouse to make a selection.
		// Notify the application of this.
		if(m_nSelStart != m_nSelected)
			SendEvent(EVENT_LISTBOX_SELECTION, this);

		SendEvent(EVENT_LISTBOX_SELECTION_END, this);
	}
}

void CUIListBox::OnFrameRender(double fTime, float fElapsedTime)
{
	CUICombo::OnFrameRender(fTime, fElapsedTime);

	CONTROL_STATE iState = GetState();

	// Render the text
	if(m_Items.size() > 0)
	{
		// Find out the height of a single line of text
		CRect<int> rc = m_rcText;
		rc.bottom = rc.top+UIGraph::GetFontSize();

		// Update the line height formation
		m_nTextHeight = rc.bottom - rc.top;

		static bool bSBInit;
		if(!bSBInit)
		{
			// Update the page size of the scroll bar
			if(m_nTextHeight)
				m_ScrollBar.SetPageSize(m_rcText.getHeight() / m_nTextHeight);
			else
				m_ScrollBar.SetPageSize(m_rcText.getHeight());
			bSBInit = true;
		}

		rc.right = m_rcText.right;
		for(int i = m_ScrollBar.GetTrackPos(); i < (int)m_Items.size(); ++i)
		{
			if(rc.bottom > m_rcText.bottom)
				break;

			UIListBoxItem *pItem = m_Items[i];

			// Determine if we need to render this item with the
			// selected element.
			bool bSelectedStyle = false;

			if(!(m_dwStyle & MULTISELECTION) && i == m_nSelected)
				bSelectedStyle = true;
			else
				if(m_dwStyle & MULTISELECTION)
				{
					if(IsPressed() &&
						((i >= m_nSelected && i < m_nSelStart) ||
						(i <= m_nSelected && i > m_nSelStart)))
						bSelectedStyle = m_Items[m_nSelStart]->bSelected;
					else
						if(pItem->bSelected)
							bSelectedStyle = true;
				}

				if(bSelectedStyle)
				{
					m_StyleSelected.draw(rc, pItem->wstrText,iState, fElapsedTime);
				}
				else
				{
					m_StyleItem.draw(rc, pItem->wstrText,iState, fElapsedTime);
				}
				rc.offset(0, m_nTextHeight);
		}
	}
}