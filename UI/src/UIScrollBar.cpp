#include "UIScrollBar.h"
#include "UIDialog.h"

// Minimum scroll bar thumb size
#define SCROLLBAR_MINTHUMBSIZE 17

// Delay and repeat period when clicking on the scroll bar arrows
#define SCROLLBAR_ARROWCLICK_DELAY  0.33
#define SCROLLBAR_ARROWCLICK_REPEAT 0.05

CUIScrollBar::CUIScrollBar()
{
	m_Type = UI_CONTROL_SCROLLBAR;
	m_bShowThumb = true;
	SetRect(&m_rcUpButton, 0, 0, 0, 0);
	SetRect(&m_rcDownButton, 0, 0, 0, 0);
	SetRect(&m_rcTrack, 0, 0, 0, 0);
	SetRect(&m_rcThumb, 0, 0, 0, 0);
	m_nPosition = 0;
	m_nPageSize = 1;
	m_nStart = 0;
	m_nEnd = 1;
	m_Arrow = CLEAR;
	m_dArrowTS = 0.0;
}

CUIScrollBar::~CUIScrollBar()
{
}

void CUIScrollBar::XMLParse(TiXmlElement* pControlElement)
{
	CUIControl::XMLParse(pControlElement);
	//
}

void CUIScrollBar::SetStyle(const std::string& strStyleName)
{
	m_StyleTrack.SetStyle(strStyleName+".track");
	m_StyleUpButton.SetStyle(strStyleName+".upbutton");
	m_StyleDownButton.SetStyle(strStyleName+".downbutton");
	m_StyleThumb.SetStyle(strStyleName+".thumb");
}

void CUIScrollBar::UpdateRects()
{
	CUIControl::UpdateRects();

	// Make the buttons square

	SetRect(&m_rcUpButton, m_rcBoundingBox.left, m_rcBoundingBox.top,
		m_rcBoundingBox.right, m_rcBoundingBox.top + RectWidth(m_rcBoundingBox));
	SetRect(&m_rcDownButton, m_rcBoundingBox.left, m_rcBoundingBox.bottom - RectWidth(m_rcBoundingBox),
		m_rcBoundingBox.right, m_rcBoundingBox.bottom);
	SetRect(&m_rcTrack, m_rcUpButton.left, m_rcUpButton.bottom,
		m_rcDownButton.right, m_rcDownButton.top);
	m_rcThumb.left = m_rcUpButton.left;
	m_rcThumb.right = m_rcUpButton.right;

	UpdateThumbRect();
}

// Compute the dimension of the scroll thumb
void CUIScrollBar::UpdateThumbRect()
{
	if(m_nEnd - m_nStart > m_nPageSize)
	{
		int nThumbHeight = __max(RectHeight(m_rcTrack) * m_nPageSize / (m_nEnd - m_nStart), SCROLLBAR_MINTHUMBSIZE);
		int nMaxPosition = m_nEnd - m_nStart - m_nPageSize;
		m_rcThumb.top = m_rcTrack.top + (m_nPosition - m_nStart) * (RectHeight(m_rcTrack) - nThumbHeight)
			/ nMaxPosition;
		m_rcThumb.bottom = m_rcThumb.top + nThumbHeight;
		m_bShowThumb = true;

	} 
	else
	{
		// No content to scroll
		m_rcThumb.bottom = m_rcThumb.top;
		m_bShowThumb = false;
	}
}

// Scroll() scrolls by nDelta items.  A positive value scrolls down, while a negative
// value scrolls up.
void CUIScrollBar::Scroll(int nDelta)
{
	// Perform scroll
	m_nPosition += nDelta;

	// Cap position
	Cap();

	// Update thumb position
	UpdateThumbRect();
}

void CUIScrollBar::ShowItem(int nIndex)
{
	// Cap the index

	if(nIndex < 0)
		nIndex = 0;

	if(nIndex >= m_nEnd)
		nIndex = m_nEnd - 1;

	// Adjust position

	if(m_nPosition > nIndex)
		m_nPosition = nIndex;
	else
		if(m_nPosition + m_nPageSize <= nIndex)
			m_nPosition = nIndex - m_nPageSize + 1;

	UpdateThumbRect();
}

bool CUIScrollBar::HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return false;
}

static int ThumbOffsetY;

void CUIScrollBar::OnMouseMove(POINT point)
{
	m_LastMouse = point;
	if(IsPressed())
	{
		m_rcThumb.bottom += point.y - ThumbOffsetY - m_rcThumb.top;
		m_rcThumb.top = point.y - ThumbOffsetY;
		if(m_rcThumb.top < m_rcTrack.top)
			OffsetRect(&m_rcThumb, 0, m_rcTrack.top - m_rcThumb.top);
		else
			if(m_rcThumb.bottom > m_rcTrack.bottom)
				OffsetRect(&m_rcThumb, 0, m_rcTrack.bottom - m_rcThumb.bottom);

		// Compute first item index based on thumb position

		int nMaxFirstItem = m_nEnd - m_nStart - m_nPageSize;  // Largest possible index for first item
		int nMaxThumb = RectHeight(m_rcTrack) - RectHeight(m_rcThumb);  // Largest possible thumb position from the top

		m_nPosition = m_nStart +
			(m_rcThumb.top - m_rcTrack.top +
			nMaxThumb / (nMaxFirstItem * 2)) * // Shift by half a row to avoid last row covered by only one pixel
			nMaxFirstItem  / nMaxThumb;
	}
}

void CUIScrollBar::OnLButtonDown(POINT point)
{
	// Check for click on up button
	if(PtInRect(&m_rcUpButton, point))
	{
		SetCapture(UIGetHWND());
		if(m_nPosition > m_nStart)
			--m_nPosition;
		UpdateThumbRect();
		m_Arrow = CLICKED_UP;
		m_dArrowTS = GetGlobalTimer().GetAbsoluteTime();//DXUTGetTime();
		return;
	}

	// Check for click on down button
	if(PtInRect(&m_rcDownButton, point))
	{
		SetCapture(UIGetHWND());
		if(m_nPosition + m_nPageSize < m_nEnd)
			++m_nPosition;
		UpdateThumbRect();
		m_Arrow = CLICKED_DOWN;
		m_dArrowTS = GetGlobalTimer().GetAbsoluteTime();//DXUTGetTime();
		return;
	}

	// Check for click on thumb
	if(PtInRect(&m_rcThumb, point))
	{
		SetCapture(UIGetHWND());
		SetPressed(true);
		ThumbOffsetY = point.y - m_rcThumb.top;
		return;
	}

	// Check for click on track
	if(m_rcThumb.left <= point.x &&
		m_rcThumb.right > point.x)
	{
		SetCapture(UIGetHWND());
		if(m_rcThumb.top > point.y &&
			m_rcTrack.top <= point.y)
		{
			Scroll(-(m_nPageSize - 1));
			return;
		}
		else if(m_rcThumb.bottom <= point.y &&
			m_rcTrack.bottom > point.y)
		{
			Scroll(m_nPageSize - 1);
			return;
		}
	}
}

void CUIScrollBar::OnLButtonUp(POINT point)
{
	SetPressed(false);
	ReleaseCapture();
	UpdateThumbRect();
	m_Arrow = CLEAR;
}

void CUIScrollBar::OnFrameRender(double fTime, float fElapsedTime)
{
	// Check if the arrow button has been held for a while.
	// If so, update the thumb position to simulate repeated
	// scroll.
	if(m_Arrow != CLEAR)
	{
		double dCurrTime = GetGlobalTimer().GetAbsoluteTime();//DXUTGetTime();
		if(PtInRect(&m_rcUpButton, m_LastMouse))
		{
			switch(m_Arrow)
			{
			case CLICKED_UP:
				if(SCROLLBAR_ARROWCLICK_DELAY < dCurrTime - m_dArrowTS)
				{
					Scroll(-1);
					m_Arrow = HELD_UP;
					m_dArrowTS = dCurrTime;
				}
				break;
			case HELD_UP:
				if(SCROLLBAR_ARROWCLICK_REPEAT < dCurrTime - m_dArrowTS)
				{
					Scroll(-1);
					m_dArrowTS = dCurrTime;
				}
				break;
			}
		} else
			if(PtInRect(&m_rcDownButton, m_LastMouse))
			{
				switch(m_Arrow)
				{
				case CLICKED_DOWN:
					if(SCROLLBAR_ARROWCLICK_DELAY < dCurrTime - m_dArrowTS)
					{
						Scroll(1);
						m_Arrow = HELD_DOWN;
						m_dArrowTS = dCurrTime;
					}
					break;
				case HELD_DOWN:
					if(SCROLLBAR_ARROWCLICK_REPEAT < dCurrTime - m_dArrowTS)
					{
						Scroll(1);
						m_dArrowTS = dCurrTime;
					}
					break;
				}
			}
	}

	CONTROL_STATE iState = CONTROL_STATE_NORMAL;

	if(m_bVisible == false)
		iState = CONTROL_STATE_HIDDEN;
	else if(m_bEnabled == false || m_bShowThumb == false)
		iState = CONTROL_STATE_DISABLED;
	else if(m_bMouseOver)
		iState = CONTROL_STATE_MOUSEOVER;
	else if(IsFocus())
		iState = CONTROL_STATE_FOCUS;

	float fBlendRate = (iState == CONTROL_STATE_PRESSED) ? 0.0f : 0.8f;

	m_StyleTrack.draw(m_rcTrack,L"",iState,fElapsedTime,fBlendRate);
	m_StyleUpButton.draw(m_rcUpButton,L"",iState,fElapsedTime,fBlendRate);
	m_StyleDownButton.draw(m_rcDownButton,L"",iState,fElapsedTime,fBlendRate);
	m_StyleThumb.draw(m_rcThumb,L"",iState,fElapsedTime,fBlendRate);
}

void CUIScrollBar::SetTrackRange(int nStart, int nEnd)
{
	m_nStart = nStart; m_nEnd = nEnd;
	Cap();
	UpdateThumbRect();
}

void CUIScrollBar::Cap()  // Clips position at boundaries. Ensures it stays within legal range.
{
	if(m_nPosition < m_nStart ||
		m_nEnd - m_nStart <= m_nPageSize)
	{
		m_nPosition = m_nStart;
	}
	else
		if(m_nPosition + m_nPageSize > m_nEnd)
			m_nPosition = m_nEnd - m_nPageSize;
}