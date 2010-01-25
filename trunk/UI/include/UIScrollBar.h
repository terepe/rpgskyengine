#pragma once

#include "UIControl.h"

//-----------------------------------------------------------------------------
// Scrollbar control
//-----------------------------------------------------------------------------
class DLL_EXPORT CUIScrollBar : public CUIControl
{
public:
	CUIScrollBar();
	virtual ~CUIScrollBar();

	virtual void	XMLParse(TiXmlElement* pControlElement);
	virtual void	SetStyle(const std::string& strStyleName);
	virtual bool    HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//
	virtual void	OnMouseMove(POINT point);
	virtual void	OnLButtonDown(POINT point);
	virtual void	OnLButtonUp(POINT point);

	virtual void    OnFrameRender(double fTime, float fElapsedTime);
	virtual void    UpdateRects();

	void			SetTrackRange(int nStart, int nEnd);
	int				GetTrackPos() { return m_nPosition; }
	void			SetTrackPos(int nPosition) { m_nPosition = nPosition; Cap(); UpdateThumbRect(); }
	int				GetPageSize() { return m_nPageSize; }
	void			SetPageSize(int nPageSize) { m_nPageSize = nPageSize; Cap(); UpdateThumbRect(); }

	void			Scroll(int nDelta);    // Scroll by nDelta items (plus or minus)
	void			ShowItem(int nIndex);  // Ensure that item nIndex is displayed, scroll if necessary

protected:
	CUIStyle m_StyleTrack;
	CUIStyle m_StyleUpButton;
	CUIStyle m_StyleDownButton;
	CUIStyle m_StyleThumb;

	// ARROWSTATE indicates the state of the arrow buttons.
	// CLEAR            No arrow is down.
	// CLICKED_UP       Up arrow is clicked.
	// CLICKED_DOWN     Down arrow is clicked.
	// HELD_UP          Up arrow is held down for sustained period.
	// HELD_DOWN        Down arrow is held down for sustained period.
	enum ARROWSTATE { CLEAR, CLICKED_UP, CLICKED_DOWN, HELD_UP, HELD_DOWN };

	void			UpdateThumbRect();
	void			Cap();  // Clips position at boundaries. Ensures it stays within legal range.

	bool		m_bShowThumb;
	RECT		m_rcUpButton;
	RECT		m_rcDownButton;
	RECT		m_rcTrack;
	RECT		m_rcThumb;
	int			m_nPosition;  // Position of the first displayed item
	int			m_nPageSize;  // How many items are displayable in one page
	int			m_nStart;     // First item
	int			m_nEnd;       // The index after the last item
	POINT		m_LastMouse;// Last mouse position
	ARROWSTATE	m_Arrow; // State of the arrows
	double		m_dArrowTS;  // Timestamp of last arrow event.
};