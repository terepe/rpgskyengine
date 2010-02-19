#pragma once
#include "UIListBox.h"

class DLL_EXPORT CUIComboBox : public CUICombo
{
public:
	CUIComboBox();
	virtual ~CUIComboBox();
	virtual void OnControlRegister();
	void OnSelectionChanged();

	virtual bool ContainsPoint(POINT pt); 

	virtual bool HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam);
	// �������
	virtual void OnMouseMove(POINT point);
	virtual void OnLButtonDown(POINT point);
	virtual void OnLButtonUp(POINT point);
	virtual void OnMouseWheel(POINT point,short wheelDelta);
	//
	virtual void OnHotkey();

	virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
	virtual void OnFocusOut();
	virtual void OnFrameRender(double fTime, float fElapsedTime);

	//void    SetDropHeight(UINT nHeight) { m_nDropHeight = nHeight; UpdateRects(); }
	virtual const std::wstring&	GetText();
	CUIListBox& getListBox();
protected:
	CUIListBox	m_ListBox;
	std::wstring m_wstrText;      // Window text
};