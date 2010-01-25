#pragma once

#include "UIStatic.h"
#include "UIDialog.h"

//-----------------------------------------------------------------------------
// Button control
//-----------------------------------------------------------------------------
class DLL_EXPORT CUIButton : public CUIStatic
{
public:
	CUIButton();
	virtual void OnPressDown();
	virtual void OnPressUp();
	// 处理键盘
	virtual bool HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam);
	// 处理鼠标
	virtual void OnLButtonDown(POINT point);
	virtual void OnLButtonDblClk(POINT point);
	virtual void OnLButtonUp(POINT point);
	virtual void OnHotkey();
	virtual bool ContainsPoint(POINT pt);
	// 是否能获取焦点
	virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }

	// 渲染
	virtual void OnFrameRender(double fTime, float fElapsedTime);

protected:
	virtual CONTROL_STATE GetState();
};