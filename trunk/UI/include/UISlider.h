#pragma once

#include "UIControl.h"

//-----------------------------------------------------------------------------
// Slider control
//-----------------------------------------------------------------------------
class DLL_EXPORT CUISlider : public CUIControl
{
public:
	CUISlider();
	virtual void	XMLParse(TiXmlElement* pControlElement);
	virtual void SetStyle(const std::string& strStyleName);
	virtual bool	ContainsPoint(POINT pt); 
	virtual bool	CanHaveFocus() { return (m_bVisible && m_bEnabled); }
	virtual bool	HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//
	virtual void OnMouseMove(POINT pt);
	virtual void OnMouseWheel(POINT point,short wheelDelta);
	virtual void OnLButtonDown(POINT point);
	virtual void OnLButtonUp(POINT point);
	//
	virtual void	UpdateRects(); 
	virtual void	OnFrameRender(double fTime, float fElapsedTime);
	bool			SetValue(int nValue);
	int				GetValue() const { return m_nValue; };
	int				getMin() const { return m_nMin; };
	int				getMax() const { return m_nMax; };
	void			GetRange(int &nMin, int &nMax) const { nMin = m_nMin; nMax = m_nMax; }
	void			SetRange(int nMin, int nMax);
protected:
	void			SetValueInternal(int nValue);
	int				ValueFromPos(int x); 
protected:
	CUIStyle m_StyleButton;
	int		m_nValue;
	int		m_nMin;
	int		m_nMax;
	int		m_nDragX;      // Mouse position at start of drag
	int		m_nDragOffset; // Drag offset from the center of the button
	int		m_nButtonX;
	RECT	m_rcButton;
};