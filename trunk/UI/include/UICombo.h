#pragma once

#include "UIControl.h"

#define BEGIN_CONTROL_REGISTER(_class)\
{\
	typedef _class ThisClass;

#define ON_BTN_CLICKED(_id, _fun)\
	RegisterControlEvent(#_id, (PEVENT)&ThisClass::_fun);

#define END_CONTROL_REGISTER()\
}

void UISetDialogRes(const std::string& strDialogRes);
const std::string& UIGetDialogRes();

class CUIButton;
class CUIStatic;
class CUIImage;
class CUICheckBox;
class CUIRadioButton;
class CUIComboBox;
class CUISlider;
class CUIEditBox;
class CUIIMEEditBox;
class CUIListBox;
class CUIScrollBar;

class CUICombo;
typedef void (__thiscall CUICombo::*PEVENT)(void);

class DLL_EXPORT CUICombo : public CUIControl
{
public:
	CUICombo();
	~CUICombo();
public:
	virtual bool Create(CUICombo* pParent = NULL);
	virtual bool Create(const std::string& strID, CUICombo* pParent=NULL);

	virtual bool isCombo(){return true;}

	virtual void XMLParse(TiXmlElement* pControlElement);
	virtual void SetStyle(const std::string& strStyleName);
	virtual void OnChildSize(const RECT& rc);
	virtual void OnSize(const RECT& rc);
	virtual void UpdateRects();
	void XMLParseControls(TiXmlElement* pElement);
	bool LoadXml(const std::string& strFilename, const std::string& strDialog);

	// my message
	virtual bool postMsg(const std::string& strMsg);
	virtual bool postMsg(uint32 uEvent);

	// Windows消息处理
	virtual bool postMsg(WPARAM wParam, LPARAM lParam);
	virtual bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Control creation
	void AddControl(CUIControl* pControl);
	void RegisterControl(const std::string& strID, CUIControl& control);

	// Control retrieval
	CUIStatic*      GetStatic(const std::string& strID) { return (CUIStatic*) GetControl(strID, UI_CONTROL_STATIC); }
	CUIImage*		GetImage(const std::string& strID) { return (CUIImage*) GetControl(strID, UI_CONTROL_IMAGE); }
	CUIButton*      GetButton(const std::string& strID) { return (CUIButton*) GetControl(strID, UI_CONTROL_BUTTON); }
	CUICheckBox*    GetCheckBox(const std::string& strID) { return (CUICheckBox*) GetControl(strID, UI_CONTROL_CHECKBOX); }
	CUIRadioButton* GetRadioButton(const std::string& strID) { return (CUIRadioButton*) GetControl(strID, UI_CONTROL_RADIOBUTTON); }
	CUIComboBox*    GetComboBox(const std::string& strID) { return (CUIComboBox*) GetControl(strID, UI_CONTROL_COMBOBOX); }
	CUISlider*      GetSlider(const std::string& strID) { return (CUISlider*) GetControl(strID, UI_CONTROL_SLIDER); }
	CUIEditBox*     GetEditBox(const std::string& strID) { return (CUIEditBox*) GetControl(strID, UI_CONTROL_EDITBOX); }
	CUIIMEEditBox*  GetIMEEditBox(const std::string& strID) { return (CUIIMEEditBox*) GetControl(strID, UI_CONTROL_IMEEDITBOX); }
	CUIListBox*     GetListBox(const std::string& strID) { return (CUIListBox*) GetControl(strID, UI_CONTROL_LISTBOX); }

	CUIControl* GetControl(const std::string& strID);
	CUIControl* GetControl(const std::string& strID, uint32 nControlType);

	CUIControl* GetControlAtPoint(POINT pt);
	CUICombo* GetChildDialogAtPoint(POINT pt);

	bool GetControlEnabled(const std::string& strID);
	void SetControlEnabled(const std::string& strID, bool bEnabled);

	bool isControlVisible(const std::string& strID);
	void setControlVisible(const std::string& strID, bool bEnabled);

	virtual void SetVisible(bool bVisible);

	void ClearRadioButtonGroup(uint32 nGroup);

	// Methods called by controls
	void progressEvent(uint32 uEvent, CUIControl* pControl);
	void RequestFocus(CUIControl* pControl);

	virtual void ClientToScreen(RECT& rc);
	virtual void ScreenToClient(RECT& rc);

	// Attributes
//	static void SetRefreshTime(float fTime){ s_fTimeRefresh = fTime; }

	CUIControl* getFirstControl();
	CUIControl* getLastControl();
	CUIControl* GetNextControl(CUIControl* pControl);
	CUIControl* GetPrevControl(CUIControl* pControl);

	void RemoveControl(const std::string& strID);
	void RemoveAllControls();

	// Sets the callback used to notify the app of control events
	void EnableNonUserEvents(bool bEnable) { m_bNonUserEvents = bEnable; }
	void EnableKeyboardInput(bool bEnable) { m_bKeyboardInput = bEnable; }
	bool IsKeyboardInputEnabled() const { return m_bKeyboardInput; }

	// Device state notification
	void Refresh();
	virtual void OnFrameMove(double fTime, float fElapsedTime);
	virtual void OnFrameRender(double fTime, float fElapsedTime);

	// 鼠标响应
	virtual void OnMouseMove(POINT pt);
	virtual void OnMouseWheel(POINT point,short wheelDelta);
	virtual void OnLButtonDblClk(POINT point);
	virtual void OnLButtonDown(POINT point);
	virtual void OnLButtonUp(POINT point);
	virtual void OnRButtonDblClk(POINT point);
	virtual void OnRButtonDown(POINT point);
	virtual void OnRButtonUp(POINT point);
	virtual void OnMButtonDblClk(POINT point);
	virtual void OnMButtonDown(POINT point);
	virtual void OnMButtonUp(POINT point);
	// Shared resource access. Indexed fonts and textures are shared among
	// all the controls.

	static void ClearState();
	static void ClearFocus();
	void FocusDefaultControl();

	bool m_bNonUserEvents;
	bool m_bKeyboardInput;
//protected:
	struct EVENTMAP_ENTRY
	{
		std::string strID;
		uint32 uEvent;	// ui event
		PEVENT pfn;		// routine to call (or special value)
	};
	void RegisterControlEvent(const std::string& strID, PEVENT pfn, uint32 uEvent=EVENT_DEFAULT);
	void RegisterEvent(std::string strEvent, PEVENT pfn);
	void RegisterEvent(uint32 uEvent, PEVENT pfn);
protected:
	std::vector<EVENTMAP_ENTRY>		m_EventMapEntry;
	std::map<std::string,PEVENT>	m_mapStrEvent;
	std::map<uint32,PEVENT>			m_mapEvent;

//	static double s_fTimeRefresh;
	double m_fTimeLastRefresh;

	virtual void OnControlRegister();

	// Control events
	bool OnCycleFocus(bool bForward);

	std::vector<CUIControl*>	m_Controls;
	std::vector<CUIControl*>	m_AssignControls;
};