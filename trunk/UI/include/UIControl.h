#pragma once

#include "UIGraph.h"
#include <usp10.h>
#include <dimm.h>
#include <winuser.h>
#include <vector>
#include "Timer.h"

class CUICombo;

HWND UIGetHWND();
void UISetHWND(HWND hWnd);

#define UITRACE	(__noop)

inline int RectWidth(RECT &rc) { return ((rc).right - (rc).left); }
inline int RectHeight(RECT &rc) { return ((rc).bottom - (rc).top); }
// Align
#define ALIGN_TOP						0x00000000
#define ALIGN_LEFT						0x00000000
#define ALIGN_CENTER					0x00000001
#define ALIGN_RIGHT						0x00000002
#define ALIGN_VCENTER					0x00000004
#define ALIGN_BOTTOM					0x00000008
inline uint32 StrToAlign(std::string& strAlign);

#ifndef SPI_GETWHEELSCROLLLINES
#define SPI_GETWHEELSCROLLLINES    0x0068
#endif

#ifndef WM_XBUTTONDOWN
#define WM_XBUTTONDOWN 0x020B // (not always defined)
#endif
#ifndef WM_XBUTTONUP
#define WM_XBUTTONUP 0x020C // (not always defined)
#endif
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A // (not always defined)
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120 // (not always defined)
#endif

// Defines and macros 
#define EVENT_DEFAULT						0x0001
#define EVENT_BUTTON_CLICKED				0x0001
#define EVENT_BUTTON_DBLCLK				    0x0101

#define EVENT_RADIOBUTTON_CHANGED			0x0001
#define EVENT_CHECKBOX_CHANGED				0x0001
#define EVENT_LISTBOX_SELECTION_END         0x0001
#define EVENT_SLIDER_VALUE_CHANGED          0x0001
#define EVENT_COMBOBOX_SELECTION_CHANGED    0x0001


#define EVENT_EDITBOX_STRING                0x0601
// EVENT_EDITBOX_CHANGE is sent when the listbox content changes
// due to user input.
#define EVENT_EDITBOX_CHANGE                0x0602
#define EVENT_LISTBOX_ITEM_DBLCLK           0x0701
// EVENT_LISTBOX_SELECTION is fired off when the selection changes in
// a single selection list box.
#define EVENT_LISTBOX_SELECTION             0x0702

#define EVENT_SETFOCUS						0x0901
#define EVENT_KILLFOCUS						0x0902
// Enums for pre-defined control types

enum UI_CONTROL_TYPE 
{
	UI_CONTROL_DIALOG,
	UI_CONTROL_COMBO,		
	UI_CONTROL_BUTTON,
	UI_CONTROL_STATIC,
	UI_CONTROL_IMAGE,
	UI_CONTROL_CHECKBOX,
	UI_CONTROL_RADIOBUTTON,
	UI_CONTROL_COMBOBOX,
	UI_CONTROL_SLIDER,
	UI_CONTROL_PROGRESS,
	UI_CONTROL_EDITBOX,
	UI_CONTROL_IMEEDITBOX,
	UI_CONTROL_LISTBOX,
	UI_CONTROL_GRID,
	UI_CONTROL_SCROLLBAR,
	UI_CONTROL_DISPLAY,
	UI_CONTROL_MAX,
};

#define MAX_CONTROL_STATES 6

class TiXmlElement;
class DLL_EXPORT CUIControl
{
public:
	static CUIStyle s_TipStyle;
	CUIControl();
	virtual ~CUIControl();

	virtual void SetParent(CUICombo *pControl);
	virtual CUICombo* GetParentDialog() { return m_pParentDialog; }

	virtual bool isCombo(){return false;}

	virtual void XMLParse(TiXmlElement* pControlElement);
	virtual void SetText(const std::wstring& wstrText){}

	virtual void OnInit() { }
	virtual void Refresh();

	virtual void OnFrameMove(double fTime, float fElapsedTime) {}
	virtual void OnFrameRender(double fTime, float fElapsedTime) {}

	// Windows message handler
	virtual bool MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam) { return false; }

	virtual bool HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam) { return false; }
	//响应鼠标的事件
	//除了MouseMove、MouseWheel其他都得通过ContainsPoint的条件才能响应
	virtual void OnMouseMove(POINT point){;}
	virtual void OnMouseWheel(POINT point,short wheelDelta){;}
	virtual void OnLButtonDblClk(POINT point){;}
	virtual void OnLButtonDown(POINT point){;}
	virtual void OnLButtonUp(POINT point){;}
	virtual void OnRButtonDblClk(POINT point){;}
	virtual void OnRButtonDown(POINT point){;}
	virtual void OnRButtonUp(POINT point){;}
	virtual void OnMButtonDblClk(POINT point){;}
	virtual void OnMButtonDown(POINT point){;}
	virtual void OnMButtonUp(POINT point){;}

	virtual bool CanHaveFocus() { return false; }
	virtual void SetFocus(bool bFocus=true);
	virtual void OnFocusIn() { SendEvent(EVENT_SETFOCUS, this);}
	virtual void OnFocusOut() { SendEvent(EVENT_KILLFOCUS, this); }
	virtual void OnMouseEnter() { m_bMouseOver = true; }
	virtual void OnMouseLeave() { m_bMouseOver = false; }
	virtual void OnHotkey() {}
	virtual void SendEvent(uint32 uEvent, CUIControl* pControl);

	virtual bool ContainsPoint(POINT pt);

	virtual void SetEnabled(bool bEnabled) { m_bEnabled = bEnabled; }
	virtual bool GetEnabled() { return m_bEnabled; }
	virtual void SetVisible(bool bVisible) { m_bVisible = bVisible; }
	virtual bool IsVisible() { return m_bVisible; }

	virtual void SetTip(const std::wstring& wstrTip) { m_wstrTip = wstrTip; }
	virtual void drawTip(const RECT& rc, double fTime, float fElapsedTime);

	virtual void SetStyle(const std::string& strStyleName);

	UI_CONTROL_TYPE GetType() const { return m_Type; }

	const std::string&  GetID()const{ return m_strID; }
	void SetID(const std::string& strID){m_strID = strID;}

	virtual void OnMove(int x, int y);
	virtual void OnSize(const RECT& rc);
	void SetLocation(int x, int y);
	void SetSize(int nWidth, int nHeight, bool bPercentWidth = false, bool bPercentHeight = false);
	void SetAlign(uint32 uAlign);

	int GetWidth() { return m_width; }
	int GetHeight() { return m_height; }

	void SetHotkey(std::string& strHotkey);
	void SetHotkey(UINT nHotkey) { m_nHotkey = nHotkey; }
	UINT GetHotkey() { return m_nHotkey; }

	void SetUserData(void *pUserData) { m_pUserData = pUserData; }
	void *GetUserData() const { return m_pUserData; }

	void SetDefault(bool bIsDefault) { m_bIsDefault = bIsDefault; }
	bool IsDefault() const { return m_bIsDefault; }

	//virtual void SetTextColor(D3DCOLOR Color);
	//CUIStyle* GetStyle(UINT iElement);

	RECT& GetBoundingBox(){return m_rcBoundingBox;}

	bool IsPressed(){return this==s_pControlPressed;}
	void SetPressed(bool bPressed){s_pControlPressed=bPressed?this:NULL;}
	bool IsFocus(){return this==s_pControlFocus;}
	static CUIControl* s_pControlFocus;        // The control which has focus
	static CUIControl* s_pControlPressed;      // The control currently pressed
	static CUIControl* s_pControlMouseOver;    // The control which is hovered over

	bool m_bVisible;                // Shown/hidden flag
	bool m_bMouseOver;              // Mouse pointer is above control
	bool m_bIsDefault;              // Is the default control

	// Size, scale, and positioning members
	int m_x, m_y;
	int m_width, m_height;
	RECT rcOffset;
	int m_nPercentWidth;
	int m_nPercentHeight;
	uint32 m_uAlign;

	CUIStyle m_Style;

	virtual void ClientToScreen(RECT& rc);
	virtual void ScreenToClient(RECT& rc);
	virtual void UpdateRects();
protected:
	virtual CONTROL_STATE GetState();
protected:
	CUICombo*			m_pParentDialog;    // Parent container

	std::string			m_strID;			// ID string
	std::wstring		m_wstrTip;
	UI_CONTROL_TYPE		m_Type;				// Control type, set once in constructor  
	UINT				m_nHotkey;			// Virtual key code for this control's hotkey
	void				*m_pUserData;		// Data associated with this control that is set by user.

	bool				m_bEnabled;			// Enabled/disabled flag

	RECT				m_rcBoundingBox;	// Rectangle defining the active region of the control
};