#pragma once

#include "UIControl.h"

//-----------------------------------------------------------------------------
// Static control
//-----------------------------------------------------------------------------
class DLL_EXPORT CUIStatic : public CUIControl
{
public:
	CUIStatic();
	virtual void	OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);
	virtual bool	ContainsPoint(POINT pt) { return false; }
	virtual const std::wstring&	GetText() { return m_wstrText; }
	size_t			GetTextLength() { return m_wstrText.size(); }
	virtual void	SetText(const std::wstring& wstrText);
	void			SetText(int32 nValue);
protected:
	std::wstring m_wstrText;      // Window text  
};
