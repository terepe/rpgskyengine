#include "UIGrid.h"
#include "tinyxml.h"

CUIGrid::CUIGrid()
{
	m_Type = UI_CONTROL_GRID;
	m_nSBWidth = 16;
	m_nSelected = -1;
	m_bDrag = false;
	m_nBorder = 6;
	m_nMargin = 5;
	m_nTextHeight = 0;
}


CUIGrid::~CUIGrid()
{
	Clear();
}

void CUIGrid::SetParent(CUICombo *pControl)
{
	CUIControl::SetParent(pControl);
	m_ScrollBar.SetParent(pControl);
	//m_EditBox.SetParent(pControl);
}

void CUIGrid::XMLParse(TiXmlElement* pControlElement)
{
	CUIControl::XMLParse(pControlElement);
	// style
	if (pControlElement->Attribute("scrollbarstyle"))
	{
		m_ScrollBar.SetStyle(pControlElement->Attribute("scrollbarstyle"));
	}
	// EditBox
	//if (pControlElement->Attribute("editboxstyle"))
	//{
	//	m_EditBox.SetStyle(pControlElement->Attribute("editboxstyle"));
	//}
}


void CUIGrid::UpdateRects()
{
	CUIControl::UpdateRects();

	m_rcSelection = m_rcBoundingBox;
	m_rcSelection.right -= m_nSBWidth;
	m_rcSelection.InflateRect(-m_nBorder, -m_nBorder);
	m_rcText = m_rcSelection;
	m_rcText.InflateRect(-m_nMargin, 0);

	// Update the scrollbar's rects
	//m_ScrollBar.SetLocation(m_rcBoundingBox.right - m_nSBWidth, m_rcBoundingBox.top);
	//m_ScrollBar.SetSize(m_nSBWidth, m_height);
	{
		m_ScrollBar.SetPageSize(m_rcText.getHeight() / UIGraph::GetFontSize());

		// The selected item may have been scrolled off the page.
		// Ensure that it is in page again.
		m_ScrollBar.ShowItem(m_nSelected);
	}
}

uint32 CUIGrid::GetLineCount()
{
	if (m_Row.empty())
	{
		return 0;
	}
	return m_Cells.size()/m_Row.size();
}

int CUIGrid::AddLine()
{
	m_Cells.resize(m_Cells.size()+m_Row.size());
	m_ScrollBar.SetTrackRange(0, GetLineCount());
	return GetLineCount()-1;
}

void CUIGrid::AddRow(const std::wstring& wstrRow)
{
	m_Row.push_back(wstrRow);
}

int CUIGrid::GetRow(const std::wstring& wstrRow)
{
	for (uint32 i=0; i<m_Row.size(); ++i)
	{
		if (wstrRow == m_Row[i])
		{
			return i;
		}
	}
	return 0;
}

int	CUIGrid::GetLine(const std::wstring& wstrLine)
{
	uint32 uCount = GetLineCount();
	for (uint32 i=0; i<uCount; ++i)
	{
		if (wstrLine == m_Cells[i*m_Row.size()].wstrText)
		{
			return i;
		}
	}
	return 0;
}

CUIGrid::Cell* CUIGrid::GetCell(const std::wstring& wstrX, const std::wstring& wstrY)
{
	return GetCell(GetRow(wstrX),GetLine(wstrY));
}

void CUIGrid::SetCell(const std::wstring& wstrX, const std::wstring& wstrY, const std::wstring&wstrText, void *pData)
{
	SetCell(GetRow(wstrX), GetLine(wstrY), wstrText, pData);
}

CUIGrid::Cell* CUIGrid::GetCell(int nX, int nY)
{
	uint32 uIndex = nY*m_Row.size()+nX;
	if (m_Cells.size()>uIndex)
	{
		return &m_Cells[uIndex];
	}
	return NULL;
}
std::wstring CUIGrid::GetCellString(int nX, int nY)
{
	Cell* pCell = GetCell(nX, nY);
	if (pCell)
	{
		return pCell->wstrText;
	}
	return L"";
}


void CUIGrid::SetCell(int nX, int nY, const std::wstring&wstrText, void *pData)
{
	Cell* pCell = GetCell(nX, nY);
	if (pCell)
	{
		pCell->wstrText = wstrText;
		pCell->pData = pData;
	}
}

void CUIGrid::Clear()
{
	m_nSelected = -1;
	m_Cells.clear();
	m_Row.clear();
	m_ScrollBar.SetTrackRange(0, 1);
}

//HRESULT CUIGrid::AddItem(const WCHAR *wszText, EGridItemType eItemType, void *pData)
//{
//	UIGridItem *pNewItem = new UIGridItem(wszText, eItemType, pData);
//	if(!pNewItem)
//		return E_OUTOFMEMORY;
//
//	m_Items.push_back(pNewItem);
//	m_ScrollBar.SetTrackRange(0, m_Items.size());
//
//	return S_OK;
//}


//
//HRESULT CUIGrid::InsertItem(int nIndex, const WCHAR *wszText, EGridItemType eItemType, void *pData)
//{
//	UIGridItem *pNewItem = new UIGridItem(wszText, eItemType, pData);
//	if(!pNewItem)
//		return E_OUTOFMEMORY;
//
//	m_Items.insert(m_Items.begin()+nIndex, pNewItem);
//	m_ScrollBar.SetTrackRange(0, m_Items.size());
//
//	return S_OK;
//}


//
//void CUIGrid::RemoveItem(int nIndex)
//{
//	if(nIndex < 0 || nIndex >= (int)m_Items.size())
//		return;
//
//	UIGridItem *pItem = m_Items[nIndex];
//
//	delete pItem;
//	m_Items.erase(m_Items.begin()+nIndex);
//	m_ScrollBar.SetTrackRange(0, m_Items.size());
//	if(m_nSelected >= (int)m_Items.size())
//		m_nSelected = m_Items.size() - 1;
//
//	SendEvent(EVENT_LISTBOX_SELECTION);
//}


//void CUIGrid::RemoveAllItems()
//{
//	for(int i = 0; i < m_Items.size(); ++i)
//	{
//		UIGridItem *pItem = m_Items[i];
//		delete pItem;
//	}
//	m_nSelected = -1;
//	m_Items.clear();
//	m_ScrollBar.SetTrackRange(0, 1);
//}
//
//
//
//UIGridItem *CUIGrid::GetItem(int nIndex)
//{
//	if(nIndex < 0 || nIndex >= (int)m_Items.size())
//		return NULL;
//
//	return m_Items[nIndex];
//}
//


// For single-selection listbox, returns the index of the selected item.
// For multi-selection, returns the first selected item after the nPreviousSelected position.
// To search for the first selected item, the app passes -1 for nPreviousSelected.  For
// subsequent searches, the app passes the returned index back to GetSelectedIndex as.
// nPreviousSelected.
// Returns -1 on error or if no item is selected.
int CUIGrid::GetSelectedIndex(int nPreviousSelected)
{
	// Single selection
	return m_nSelected;
}



//void CUIGrid::SelectItem(int nNewIndex)
//{
//	// If no item exists, do nothing.
//	if(m_Items.size() == 0)
//		return;
//
//	int nOldSelected = m_nSelected;
//
//	// Adjust m_nSelected
//	m_nSelected = nNewIndex;
//
//	// Perform capping
//	if(m_nSelected < 0)
//		m_nSelected = 0;
//	if(m_nSelected >= (int)m_Items.size())
//		m_nSelected = m_Items.size() - 1;
//
//	if(nOldSelected != m_nSelected)
//	{
//		// Adjust scroll bar
//		m_ScrollBar.ShowItem(m_nSelected);
//	}
//
//	SendEvent(EVENT_LISTBOX_SELECTION);
//}



bool CUIGrid::HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(!m_bEnabled || !m_bVisible)
		return false;

	// Let the scroll bar have a chance to handle it first
	if(m_ScrollBar.HandleKeyboard(uMsg, wParam, lParam))
		return true;
	//if(m_EditBox.HandleKeyboard(uMsg, wParam, lParam))
	//	return true;

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
				if(GetLineCount() == 0)
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
				case VK_END: m_nSelected = GetLineCount() - 1; break;
				}

				// Perform capping
				if(m_nSelected < 0)
					m_nSelected = 0;
				if(m_nSelected >= (int)GetLineCount())
					m_nSelected = GetLineCount() - 1;

				if(nOldSelected != m_nSelected)
				{
					// Adjust scroll bar
					m_ScrollBar.ShowItem(m_nSelected);

					// Send notification
					SendEvent(EVENT_LISTBOX_SELECTION);
				}
				return true;
			}

			// Space is the hotkey for double-clicking an item.
			//
		case VK_SPACE:
			SendEvent(EVENT_LISTBOX_ITEM_DBLCLK);
			return true;
		}
		break;
	}

	return false;
}



bool CUIGrid::HandleMouse(UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam)
{
	if(!m_bEnabled || !m_bVisible)
		return false;

	// First acquire focus
	if(WM_LBUTTONDOWN == uMsg)
	{
		SetFocus();
	}

	// Let the scroll bar handle it first.
	//if(m_ScrollBar.HandleMouse(uMsg, pt, wParam, lParam))
	//	return true;
	//if(m_EditBox.HandleMouse(uMsg, pt, wParam, lParam))
	//	return true;

	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		// Check for clicks in the text area
		if(GetLineCount() > 0 && m_rcSelection.ptInRect(pt))
		{
			// Compute the index of the clicked item

			int nClicked;
			if(m_nTextHeight)
				nClicked = m_ScrollBar.GetTrackPos() + (pt.y - m_rcText.top) / m_nTextHeight;
			else
				nClicked = -1;

			// Only proceed if the click falls on top of an item.

			if(nClicked >= m_ScrollBar.GetTrackPos() &&
				nClicked < (int)GetLineCount() &&
				nClicked < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize())
			{
				SetCapture(UIGetHWND());
				m_bDrag = true;

				// If this is a double click, fire off an event and exit
				// since the first click would have taken care of the selection
				// updating.
				if(uMsg == WM_LBUTTONDBLCLK)
				{
					SendEvent(EVENT_LISTBOX_ITEM_DBLCLK);
					return true;
				}
				m_nSelected = nClicked;

				SendEvent(EVENT_LISTBOX_SELECTION);

				//m_EditBox.SetText(m_Items[m_nSelected]->GetValString());
			}

			return true;
		}
		break;

	case WM_LBUTTONUP:
		{
			ReleaseCapture();
			m_bDrag = false;

			if(m_nSelected != -1)
			{
				SendEvent(EVENT_LISTBOX_SELECTION_END);
			}
			return false;
		}

	case WM_MOUSEMOVE:
		if(m_bDrag)
		{
			// Compute the index of the item below cursor

			int nItem;
			if(m_nTextHeight)
				nItem = m_ScrollBar.GetTrackPos() + (pt.y - m_rcText.top) / m_nTextHeight;
			else
				nItem = -1;

			// Only proceed if the cursor is on top of an item.

			if(nItem >= (int)m_ScrollBar.GetTrackPos() &&
				nItem < (int)GetLineCount() &&
				nItem < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize())
			{
				m_nSelected = nItem;
				SendEvent(EVENT_LISTBOX_SELECTION);
			}
			else if(nItem < (int)m_ScrollBar.GetTrackPos())
			{
				// User drags the mouse above window top
				m_ScrollBar.Scroll(-1);
				m_nSelected = m_ScrollBar.GetTrackPos();
				SendEvent(EVENT_LISTBOX_SELECTION);
			}
			else if(nItem >= m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize())
			{
				// User drags the mouse below window bottom
				m_ScrollBar.Scroll(1);
				m_nSelected = __min((int)GetLineCount(), m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize()) - 1;
				SendEvent(EVENT_LISTBOX_SELECTION);
			}
		}
		break;
	case WM_MOUSEWHEEL:
		{
			UINT uLines;
			SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &uLines, 0);
			int nScrollAmount = int((short)HIWORD(wParam)) / WHEEL_DELTA * uLines;
			m_ScrollBar.Scroll(-nScrollAmount);
			return true;
		}
	}

	return false;
}




void CUIGrid::OnFrameRender(double fTime, float fElapsedTime)
{
	if(m_bVisible == false)
		return;

	//m_Style.Blend(CONTROL_STATE_NORMAL, fElapsedTime);

	//UIGraph::DrawSprite(m_Style, 0, m_rcBoundingBox);

	// Render the text
	if(GetLineCount() > 0)
	{
		// Find out the height of a single line of text
		CRect<int> rc = m_rcText;
		CRect<int> rcSel = m_rcSelection;
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
		{
			for(uint32 x=0; x<m_Row.size(); ++x )
			{
				CRect<int> rcVal = rc;
				int nWidth = (rc.right - rc.left)/m_Row.size();
				rcVal.left = rc.left+nWidth*x;
				rcVal.right = rcVal.left+nWidth;

				UIGraph::DrawText(m_Row[x], m_Style, 0, rcVal.getRECT());
			}
			rc.InflateRect(0, m_nTextHeight);
		}
		for(int y = m_ScrollBar.GetTrackPos(); y < (int)GetLineCount(); ++y)
		{
			if(rc.bottom > m_rcText.bottom)
				break;

			int nTextStyle = (y == m_nSelected)?1:0;
			if(y == m_nSelected)
			{
				rcSel.top = rc.top; rcSel.bottom = rc.bottom;
		//		UIGraph::DrawSprite(m_Style, 1, rcSel);
				//UIGraph::DrawText(pCell->m_wstrText, m_Style, 1, rc);
				//UIGraph::DrawText(pCell->GetValString(), m_Styles[0], rcVal);
				//m_EditBox.SetLocation(rcVal.left-8,rcVal.top-8);
				//m_EditBox.SetSize(rcVal.right-rcVal.left+16,rcVal.bottom-rcVal.top+16);
				//m_EditBox.UpdateRects();
				//m_EditBox.OnFrameRender(fTime, fElapsedTime);
			}
			for(uint32 x=0; x<m_Row.size(); ++x )
			{
				CRect<int> rcVal = rc;
				int nWidth = (rc.right - rc.left)/m_Row.size();
				rcVal.left = rc.left+nWidth*x;
				rcVal.right = rcVal.left+nWidth;

				Cell* pCell = GetCell(x, y);
				if(pCell)
				{
					UIGraph::DrawText(pCell->wstrText, m_Style, nTextStyle, rcVal.getRECT());
				}
			}
			rc.InflateRect(0, m_nTextHeight);
		}
	}

	// Render the scroll bar

	m_ScrollBar.OnFrameRender(fTime, fElapsedTime);
}