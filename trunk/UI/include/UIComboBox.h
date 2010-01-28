#pragma once

#include "UIButton.h"
#include "UIScrollBar.h"

//-----------------------------------------------------------------------------
// ComboBox control
//-----------------------------------------------------------------------------
struct UIComboBoxItem
{
	std::wstring wstrText;
	void*  pData;

	RECT  rcActive;
	bool  bVisible;
};


class DLL_EXPORT CUIComboBox : public CUIButton
{
public:
	CUIComboBox();
	virtual ~CUIComboBox();
	virtual void SetParent(CUICombo* pControl);
	virtual void XMLParse(TiXmlElement* pControlElement);
	virtual void SetStyle(const std::string& strStyleName);
	virtual bool ContainsPoint(POINT pt); 

	virtual bool HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam);
	// ¥¶¿Ì Û±Í
	virtual void OnMouseMove(POINT point);
	virtual void OnLButtonDown(POINT point);
	virtual void OnLButtonUp(POINT point);
	virtual void OnMouseWheel(POINT point,short wheelDelta);
	//
	virtual void OnHotkey();

	virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
	virtual void OnFocusOut();
	virtual void OnFrameRender(double fTime, float fElapsedTime);

	virtual void OnSize(const RECT& rc);
	virtual void UpdateRects(); 

	bool	AddItem(const std::wstring& wstrText, void* pData=NULL);
	void    RemoveAllItems();
	void    RemoveItem(UINT index);
	bool    ContainsItem(const std::wstring& wstrText, UINT iStart=0);
	int     FindItem(const std::wstring& wstrText, UINT iStart=0);
	void*   GetItemData(const std::wstring& wstrText);
	void*   GetItemData(uint32 nIndex);
	void    SetDropHeight(UINT nHeight) { m_nDropHeight = nHeight; UpdateRects(); }
	int     GetScrollBarWidth() const { return m_nSBWidth; }
	void    SetScrollBarWidth(int nWidth) { m_nSBWidth = nWidth; UpdateRects(); }

	void*   GetSelectedData();
	UIComboBoxItem* GetSelectedItem();
	virtual const std::wstring&	GetText();
	int		GetSelectedItemIndex() const {return m_iSelected; }

	size_t	GetItemCount() { return m_Items.size(); }
	UIComboBoxItem* GetItem(UINT index) { return m_Items[index]; }

	bool	SetSelectedByIndex(UINT index);
	bool	SetSelectedByText(const std::wstring& wstrText);
	bool	SetSelectedByData(void* pData);  

protected:
	CUIStyle m_StyleDropdown;
	CUIStyle m_StyleItem;
	CUIStyle m_StyleSelected;

	int     m_iSelected;
	int     m_iFocused;
	int     m_nDropHeight;
	CUIScrollBar m_ScrollBar;
	int     m_nSBWidth;

	bool    m_bOpened;

	RECT m_rcDropdown;

	std::vector<UIComboBoxItem*> m_Items;
};