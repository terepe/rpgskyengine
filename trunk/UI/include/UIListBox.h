#pragma once

#include "UIControl.h"
#include "UIScrollBar.h"
#include "UIDialog.h"

//-----------------------------------------------------------------------------
// ListBox control
//-----------------------------------------------------------------------------
struct UIListBoxItem
{
	std::wstring wstrText;
	void*  pData;

	RECT  rcActive;
	bool  bSelected;
};

class DLL_EXPORT CUIListBox : public CUICombo
{
public:
	CUIListBox();
	virtual ~CUIListBox();
	virtual void	OnControlRegister();
	virtual void	SetStyle(const std::string& strStyleName);
	virtual bool    CanHaveFocus() { return (m_bVisible && m_bEnabled); }
	virtual bool    HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//
	virtual void	OnMouseMove(POINT point);
	virtual void	OnMouseWheel(POINT point,short wheelDelta);
	virtual void	OnLButtonDblClk(POINT point);
	virtual void	OnLButtonDown(POINT point);
	virtual void	OnLButtonUp(POINT point);

	virtual void    OnFrameRender(double fTime, float fElapsedTime);

	virtual void    UpdateRects();

	DWORD	GetListBoxStyle() const { return m_dwStyle; }
	size_t	GetItemCount() const { return m_Items.size(); }
	void	SetListBoxStyle(DWORD dwStyle) { m_dwStyle = dwStyle; }
	int		GetScrollBarWidth() const { return m_nSBWidth; }
	void	SetScrollBarWidth(int nWidth) { m_nSBWidth = nWidth; UpdateRects(); }
	void	SetBorder(int nBorder, int nMargin) { m_nBorder = nBorder; m_nMargin = nMargin; }
	bool	AddItem(const std::wstring& wstrText, void *pData=NULL);
	bool	InsertItem(int nIndex, const std::wstring& wstrText, void *pData=NULL);
	void	RemoveItem(int nIndex);
	void	RemoveItemByText(WCHAR *wszText);
	void	RemoveItemByData(void *pData);
	void	RemoveAllItems();

	int		getItemIndexByPoint(POINT point);
	int		getItemIndexByData(void* pData);

	UIListBoxItem*	GetItem(int nIndex);
	const std::wstring& GetItemText(int nInde);
	int				GetSelectedIndex(int nPreviousSelected = -1);
	void*			GetSelectedData(int nPreviousSelected = -1);
	UIListBoxItem*	GetSelectedItem(int nPreviousSelected = -1);

	bool    ContainsItem(const std::wstring& wstrText, UINT iStart=0);
	int     FindItem(const std::wstring& wstrText, UINT iStart=0);
	bool	SelectItem(int nNewIndex);

	void*   getItemDataByText(const std::wstring& wstrText);
	void*   getItemDataByIndex(size_t index);

	bool	selectByText(const std::wstring& wstrText);
	bool	selectByData(void* pData);  

	CUIScrollBar&	GetScrollBar(){return m_ScrollBar;}
	enum STYLE { MULTISELECTION = 1 };

protected:
	CUIStyle m_StyleItem;
	CUIStyle m_StyleSelected;
	CRect<int> 	m_rcText;      // Text rendering bound
	CRect<int> 	m_rcSelection; // Selection box bound
	CUIScrollBar	m_ScrollBar;
	int		m_nSBWidth;
	int		m_nBorder;
	int		m_nMargin;
	int		m_nTextHeight;  // Height of a single line of text
	DWORD	m_dwStyle;    // List box style
	int		m_nSelected;    // Index of the selected item for single selection list box
	int		m_nSelStart;    // Index of the item where selection starts (for handling multi-selection)

	std::vector<UIListBoxItem*> m_Items;
};