#include "UICombo.h"

#include "UIStatic.h"
#include "UIButton.h"
#include "UICheckBox.h"
#include "UIComboBox.h"
#include "UIRadioButton.h"
#include "UIImage.h"
#include "UIListBox.h"
#include "UIGrid.h"
#include "UIScrollBar.h"
#include "UISlider.h"
#include "UIProgress.h"
#include "UIEditBox.h"
#include "UIIMEEditBox.h"
#include "UIDisplay.h"
#include "UIComboNumber.h"
#include "UIComboColor.h"
#include "tinyxml.h"

std::string g_strDialogResFilename;

void UISetDialogRes(const std::string& strDialogRes)
{
	g_strDialogResFilename = strDialogRes;
}
const std::string& UIGetDialogRes()
{
	return g_strDialogResFilename;
}

//////////////////////////////////////////////////////////////////////////
CUICombo::CUICombo()
{
	m_Type = UI_CONTROL_COMBO;

	m_fTimeLastRefresh = 0;

	m_pParentDialog = NULL;

	m_bNonUserEvents = false;
	m_bKeyboardInput = false;
}

CUICombo::~CUICombo()
{
	for(uint32 i=0; i < m_AssignControls.size(); i++)
	{
		S_DEL(m_AssignControls[i]);
	}
}

bool CUICombo::Create(CUICombo* pParent)
{
	SetParent(pParent);
	//if (GetParentDialog()!=NULL)
	//{
	//	GetParentDialog()->RegisterDialog(this);
	//}
	//if (false == LoadXml(UIGetDialogRes(), m_strID))
	//{
	//	return false;
	//}
	//if (false == OnInitDialog())
	//{
	//	return false;
	//}
	return true;
}

bool CUICombo::Create(const std::string& strID, CUICombo* pParent)
{
	SetID(strID);
	return Create(pParent);
}

void CUICombo::OnControlRegister()
{
}

void CUICombo::XMLParse(TiXmlElement* pElement)
{
	CUIControl::XMLParse(pElement);
	// control
	XMLParseControls(pElement);
}

void CUICombo::SetStyle(const std::string& strStyleName)
{
	m_Style.SetStyle(strStyleName);
}

void CUICombo::UpdateRects()
{
	CUIControl::UpdateRects();
}

void CUICombo::XMLParseControls(TiXmlElement* pElement)
{
	TiXmlElement *pControlElement = pElement->FirstChildElement("element");
	while (pControlElement)
	{
		CUIControl* pControl = NULL;

		std::string strElement = pControlElement->Attribute("type");
		//
		if (pControlElement->Attribute("id"))
		{
			std::string strID = pControlElement->Attribute("id");
			pControl = GetControl(strID);
			if (pControl)
			{
				RemoveControl(strID);
				AddControl(pControl);
			}
		}
		if (pControl==NULL)
		{
			if(strElement=="button")
			{
				pControl =  new CUIButton();
			}
			else if(strElement=="static")
			{
				pControl =  new CUIStatic();
			}
			else if(strElement=="image")
			{
				pControl =  new CUIImage();
			}
			else if(strElement=="checkbox")
			{
				pControl =  new CUICheckBox();
			}
			else if(strElement=="radiobutton")
			{
				pControl =  new CUIRadioButton();
			}
			else if(strElement=="slider")
			{
				pControl =  new CUISlider();
			}
			else if(strElement=="progress")
			{
				pControl =  new CUIProgress();
			}
			else if(strElement=="editbox")
			{
				pControl =  new CUIEditBox();
			}
			else if(strElement=="imeeditbox")
			{
				pControl =  new CUIIMEEditBox();
			}
			else if(strElement=="grid")
			{
				pControl =  new CUIGrid();
			}
			else if(strElement=="combo")
			{
				pControl =  new CUICombo();
			}
			else if(strElement=="scrollbar")
			{
				pControl =  new CUIScrollBar();
			}
			else if(strElement=="listbox")
			{
				pControl =  new CUIListBox();
			}
			else if(strElement=="combobox")
			{
				pControl =  new CUIComboBox();
			}
			else if(strElement=="numeditbox")
			{
				pControl =  new CUIComboNumber();
			}
			else if(strElement=="color")
			{
				pControl =  new CUIComboColor();
			}
			else
			{
				pControl =  new CUICombo();
			}
			if (pControl)
			{
				m_AssignControls.push_back(pControl);
				AddControl(pControl);
			}
		}
		// 公共属性
		if (pControl)
		{
			if (pControl->isCombo())
			{
				((CUICombo*)pControl)->OnControlRegister();
				((CUICombo*)pControl)->LoadXml(UIGetDialogRes(), strElement);
			}
			pControl->XMLParse(pControlElement);
		}
		pControlElement = pControlElement->NextSiblingElement("element");
	}
}
#include "IORead.h"
bool CUICombo::LoadXml(const std::string& strFilename, const std::string& strDialog)
{
	TiXmlDocument myDocument;
	{
		IOReadBase* pRead = IOReadBase::autoOpen(strFilename);
		if (pRead)
		{
			size_t uFilesize = pRead->GetSize();
			char* pBuf = new char[uFilesize+1];
			if (pBuf)
			{
				pBuf[uFilesize] = 0;
				pRead->Read(pBuf, uFilesize);
				myDocument.LoadFormMemory(pBuf, uFilesize, TIXML_ENCODING_UTF8);
				delete[] pBuf;
			}
			IOReadBase::autoClose(pRead);
		}
	}
	if (myDocument.Error())
	{
		return false;
	}
	//获得根元素，即root。
	TiXmlElement *pRootElement = myDocument.RootElement();
	if (pRootElement==NULL)
	{
		return false;
	}
	//获得第一个dialog节点。
	TiXmlElement *pDialogElement = pRootElement->FirstChildElement("dialog");
	while (pDialogElement)
	{
		if (pDialogElement->Attribute("id"))
		{
			if (strDialog == pDialogElement->Attribute("id"))
			{
				XMLParse(pDialogElement);
				break;
			}
		}
		// 查找下一个dialog
		pDialogElement = pDialogElement->NextSiblingElement("dialog");
	}
	return true;
}

void CUICombo::RemoveControl(const std::string& strID)
{
	for(std::vector<CUIControl*>::iterator it=m_Controls.begin(); it != m_Controls.end(); it++)
	{
		CUIControl* pControl = *it;
		if(pControl->GetID() == strID)
		{
			// Clean focus first
			ClearFocus();

			// Clear references to this control
			if(s_pControlFocus == pControl)
				s_pControlFocus = NULL;
			if(s_pControlPressed == pControl)
				s_pControlPressed = NULL;
			if(s_pControlMouseOver == pControl)
				s_pControlMouseOver = NULL;
			m_Controls.erase(it);
			return;
		}
	}
}

void CUICombo::RemoveAllControls()
{
	if(s_pControlFocus && s_pControlFocus->GetParentDialog() == this)
		s_pControlFocus = NULL;
	if(s_pControlPressed && s_pControlPressed->GetParentDialog() == this)
		s_pControlPressed = NULL;
	if(s_pControlMouseOver && s_pControlMouseOver->GetParentDialog() == this)
		s_pControlMouseOver = NULL;

	m_Controls.clear();
}

void CUICombo::Refresh()
{
	clearFocus();

	if(s_pControlMouseOver)
		s_pControlMouseOver->OnMouseLeave();

	s_pControlPressed = NULL;
	s_pControlMouseOver = NULL;

	for(uint32 i=0; i < m_Controls.size(); i++)
	{
		CUIControl* pControl = m_Controls[i];
		pControl->Refresh();
	}

	if(m_bKeyboardInput)
		FocusDefaultControl();
}

void CUICombo::OnFrameMove(double fTime, float fElapsedTime)
{
	for(uint32 i=0; i < m_Controls.size(); i++)
	{
		if (m_Controls[i])
		{
			m_Controls[i]->OnFrameMove(fTime, fElapsedTime);
		}
	}
}

void CUICombo::OnFrameRender(double fTime, float fElapsedTime)
{
	// If this assert triggers, you need to call CUIComboResMgr::On*Device() from inside
	// the application's device callbacks.  See the SDK samples for an example of how to do this.

	// See if the dialog needs to be refreshed

	////////if(m_fTimeLastRefresh < s_fTimeRefresh)
	////////{
	////////	m_fTimeLastRefresh = DXUTGetTime();
	////////	Refresh();
	////////}

	// For invisible dialog, out now.

	m_Style.draw(m_rcBoundingBox, L"",GetState(), fElapsedTime);

	// render controls
	for(uint32 i=0; i < m_Controls.size(); i++)
	{
		CUIControl* pControl = m_Controls[i];   

		if (pControl->IsFocus())
		{
			continue;
		}
		pControl->OnFrameRender(fTime, fElapsedTime);
	}
	for(uint32 i=0; i < m_Controls.size(); i++)
	{
		CUIControl* pControl = m_Controls[i];   

		if (pControl->IsFocus())
		{
			pControl->OnFrameRender(fTime, fElapsedTime);
		}
	}
}

void CUICombo::progressEvent(uint32 uEvent, CUIControl* pControl)
{
	// Discard events triggered programatically if these types of events haven't been
	// enabled
	//if(! && !m_bNonUserEvents)
	//	return;
	for (uint32 i = 0; i < m_EventMapEntry.size(); i++)
	{
		const EVENTMAP_ENTRY& eventMapEntry = m_EventMapEntry[i];
		if (eventMapEntry.uEvent == uEvent && eventMapEntry.strID == pControl->GetID())
		{
			(this->*(eventMapEntry.pfn))(); // 类成员函数指针
			break;
		}
	}
}

bool CUICombo::postMsg(const std::string& strMsg)
{
	std::map<std::string,PEVENT>::iterator it = m_mapStrEvent.find(strMsg);
	if (it!=m_mapStrEvent.end())
	{
		(this->*(it->second))(); // 类成员函数指针
		return true;
	}
	return false;
}

bool CUICombo::postMsg(uint32 uEvent)
{
	std::map<uint32,PEVENT>::iterator it = m_mapEvent.find(uEvent);
	if (it!=m_mapEvent.end())
	{
		(this->*(it->second))(); // 类成员函数指针
		return true;
	}
	return false;
}

bool CUICombo::postMsg(WPARAM wParam, LPARAM lParam)
{
	return true;
}

bool CUICombo::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// If automation command-line switch is on, enable this dialog's keyboard input
	// upon any key press or mouse click.
//	if(DXUTGetAutomation() &&
//		(WM_LBUTTONDOWN == uMsg || WM_LBUTTONDBLCLK == uMsg || WM_KEYDOWN == uMsg))
//	{
//		GetUIMgr().EnableKeyboardInputForAllDialogs();
//	}

	switch(uMsg)
	{
	case WM_SIZE:
	case WM_MOVE: // window窗体移动
		{
			// Handle sizing and moving messages so that in case the mouse cursor is moved out
			// of an UI control because of the window adjustment, we can properly
			// unhighlight the highlighted control.
			POINT pt = { -1, -1 };
			OnMouseMove(pt);
			break;
		}

		// Keyboard messages
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			// Not yet handled, see if this matches a control's hotkey
			// Activate the hotkey if the focus doesn't belong to an
			// edit box.
			if(uMsg == WM_KEYDOWN && (!s_pControlFocus ||
				(s_pControlFocus->GetType() != UI_CONTROL_EDITBOX
				&& s_pControlFocus->GetType() != UI_CONTROL_IMEEDITBOX)))
			{
				for(uint32 i=0; i < m_Controls.size(); i++)
				{
					CUIControl* pControl = m_Controls[i];
					if(pControl->GetHotkey() == wParam)
					{
						pControl->OnHotkey();
						return true;
					}
				}
			}

			// Not yet handled, check for focus messages
			if(uMsg == WM_KEYDOWN)
			{
				// If keyboard input is not enabled, this message should be ignored
				if(!m_bKeyboardInput)
					return false;

				switch(wParam)
				{
				case VK_RIGHT:
				case VK_DOWN:
					if(s_pControlFocus != NULL)
					{
						return OnCycleFocus(true);
					}
					break;

				case VK_LEFT:
				case VK_UP:
					if(s_pControlFocus != NULL)
					{
						return OnCycleFocus(false);
					}
					break;

				case VK_TAB: 
					{
						bool bShiftDown = ((GetKeyState(VK_SHIFT) & 0x8000) != 0);
						return OnCycleFocus(!bShiftDown);
					}
				}
			}
			break;
		}

	}

	return false;
}

CUIControl* CUICombo::GetControlAtPoint(POINT pt)
{
	for(uint32 i=0; i < m_Controls.size(); i++)
	{
		CUIControl* pControl = m_Controls[i];
		if (pControl->IsFocus())
		{
			if (pControl->ContainsPoint(pt))
			{
				return pControl;
			}
		}
	}
	for(int i = m_Controls.size()-1; i >= 0; i--)
	{
		CUIControl* pControl = m_Controls[i];
		if(pControl==NULL)
		{
			continue;
		}
		if(pControl->ContainsPoint(pt) && pControl->GetEnabled() && pControl->IsVisible())
		{
			return pControl;
		}
	}
	return NULL;
}

bool CUICombo::GetControlEnabled(const std::string& strID)
{
	CUIControl* pControl = GetControl(strID);
	if(pControl == NULL)
		return false;
	return pControl->GetEnabled();
}

void CUICombo::SetControlEnabled(const std::string& strID, bool bEnabled)
{
	CUIControl* pControl = GetControl(strID);
	if(pControl == NULL)
		return;
	pControl->SetEnabled(bEnabled);
}

bool CUICombo::isControlVisible(const std::string& strID)
{
	CUIControl* pControl = GetControl(strID);
	if(pControl == NULL)
		return false;
	return pControl->IsVisible();
}

void CUICombo::setControlVisible(const std::string& strID, bool bEnabled)
{
	CUIControl* pControl = GetControl(strID);
	if(pControl == NULL)
		return;
	pControl->SetVisible(bEnabled);
}

void CUICombo::SetVisible(bool bVisible)
{
	CUIControl::SetVisible(bVisible);
	//if (false==bVisible)
	//{
	//	m_Style.Blend(CONTROL_STATE_HIDDEN, 1.0f, 0.0f);
	//	for(uint32 i=0; i < m_Controls.size(); i++)
	//	{
	//		CUIControl* pControl = m_Controls[i];
	//		pControl->m_Style.Blend(CONTROL_STATE_HIDDEN, 1.0f, 0.0f);
	//	}
	//}
}

void CUICombo::OnMouseMove(POINT point)
{
	// Figure out which control the mouse is over now
	CUIControl* pControl = GetControlAtPoint(point);

	if(pControl!=NULL)
	{
		pControl->OnMouseMove(point);
	}
	//else if(s_pControlFocus)
	//{
	//	s_pControlFocus->OnMouseMove(point);
	//}

	// If the mouse is still over the same control, nothing needs to be done
	if(pControl == s_pControlMouseOver)
		return;

	// Handle mouse leaving the old control
	if(s_pControlMouseOver)
		s_pControlMouseOver->OnMouseLeave();

	// Handle mouse entering the new control
	s_pControlMouseOver = pControl;
	if(pControl != NULL)
		s_pControlMouseOver->OnMouseEnter();
}

void CUICombo::OnMouseWheel(POINT point,short wheelDelta)
{
	// 控件
	CUIControl* pControl = GetControlAtPoint(point);
	if(pControl!=NULL)
	{
		pControl->OnMouseWheel(point,wheelDelta);
		return;
		//return true;
	}
	// return false;
}

void CUICombo::OnLButtonDblClk(POINT point)
{
	// 控件
	CUIControl* pControl = GetControlAtPoint(point);
	if(pControl!=NULL)
	{
		pControl->OnLButtonDblClk(point);
		return;
	}
}
void CUICombo::OnLButtonDown(POINT point)
{
	//// 设置为焦点对话框
	//SetFocus(true);
	// 控件
	CUIControl* pControl = GetControlAtPoint(point);
	if(pControl!=NULL)
	{
		pControl->OnLButtonDown(point);
		return;
	}
	SetFocus(true);
}
void CUICombo::OnLButtonUp(POINT point)
{
	//// 消除按住状态
	//if(IsPressed())
	//{
	//	SetPressed(false);
	//	//ReleaseCapture();
	//	return;
	//}
	// 控件
	CUIControl* pControl = GetControlAtPoint(point);
	if(pControl!=NULL)
	{
		pControl->OnLButtonUp(point);
		return;
	}
}
void CUICombo::OnRButtonDblClk(POINT point)
{
	// 控件
	CUIControl* pControl = GetControlAtPoint(point);
	if(pControl!=NULL)
	{
		pControl->OnRButtonDblClk(point);
		return;
	}
	SetFocus(true);
}
void CUICombo::OnRButtonDown(POINT point)
{
	//// 设置为焦点对话框
	//SetFocus(true);
	// 控件
	CUIControl* pControl = GetControlAtPoint(point);
	if(pControl!=NULL)
	{
		pControl->OnRButtonDown(point);
		return;
	}
	SetFocus(true);
}
void CUICombo::OnRButtonUp(POINT point)
{
	// 控件
	CUIControl* pControl = GetControlAtPoint(point);
	if(pControl!=NULL)
	{
		pControl->OnRButtonUp(point);
		return;
	}
}
void CUICombo::OnMButtonDblClk(POINT point)
{
	// 控件
	CUIControl* pControl = GetControlAtPoint(point);
	if(pControl!=NULL)
	{
		pControl->OnMButtonDblClk(point);
		return;
	}
	SetFocus(true);
}
void CUICombo::OnMButtonDown(POINT point)
{
	//// 设置为焦点对话框
	//SetFocus(true);
	// 控件
	CUIControl* pControl = GetControlAtPoint(point);
	if(pControl!=NULL)
	{
		pControl->OnMButtonDown(point);
		return;
	}
	SetFocus(true);
}
void CUICombo::OnMButtonUp(POINT point)
{
	// 控件
	CUIControl* pControl = GetControlAtPoint(point);
	if(pControl!=NULL)
	{
		pControl->OnMButtonUp(point);
		return;
	}
}

bool CUICombo::IsFocus()
{
	if (CUIControl::IsFocus())
	{
		return true;
	}
	
	for(uint32 i=0; i < m_Controls.size(); i++)
	{
		CUIControl* pControl = m_Controls[i];
		if (pControl->IsFocus())
		{
			return true;
		}
	}
	return false;
}

void CUICombo::OnFocusOut()
{
	CUIControl::OnFocusOut();
	if (GetParentDialog())
	{
		if (!GetParentDialog()->IsFocus())
		{
			GetParentDialog()->OnFocusOut();
		}
	}
}

void CUICombo::RegisterControlEvent(const std::string& strID, PEVENT pfn, uint32 uEvent)
{
	EVENTMAP_ENTRY eventMapEntry;
	eventMapEntry.strID		= strID;
	eventMapEntry.uEvent	= uEvent;
	eventMapEntry.pfn		= pfn;
	for (std::vector<EVENTMAP_ENTRY>::iterator it=m_EventMapEntry.begin();it!=m_EventMapEntry.end();it++)
	{
		if (it->strID==strID&&it->uEvent==uEvent)
		{
			std::wstring wstrInfo = FormatW(L"The same control events!\nDialog:%s; Control:%s; Event:%d;\nCheck the dialog functions::OnControlRegister(), to sure that not have the same events!",s2ws(m_strID).c_str(),s2ws(strID).c_str(),uEvent);
			MessageBoxW(NULL,wstrInfo.c_str(),L"Register Control Event Warn!",0);
		}
	}
	m_EventMapEntry.push_back(eventMapEntry);
}

void CUICombo::RegisterEvent(std::string strEvent, PEVENT pfn)
{
	if (m_mapStrEvent.find(strEvent)!=m_mapStrEvent.end())
	{
		std::wstring wstrInfo = FormatW(L"The same events!\nDialog:%s; Event:%s;\nCheck the dialog functions::OnControlRegister(), to sure that not have the same events!",s2ws(m_strID).c_str(),strEvent);
		MessageBoxW(NULL,wstrInfo.c_str(),L"Register Event Warn!",0);
	}
	m_mapStrEvent[strEvent]=pfn;
}

void CUICombo::RegisterEvent(uint32 uEvent, PEVENT pfn)
{
	if (m_mapEvent.find(uEvent)!=m_mapEvent.end())
	{
		std::wstring wstrInfo = FormatW(L"The same events!\nDialog:%s; Event:%d;\nCheck the dialog functions::OnControlRegister(), to sure that not have the same events!",s2ws(m_strID).c_str(),uEvent);
		MessageBoxW(NULL,wstrInfo.c_str(),L"Register Event Warn!",0);
	}
	m_mapEvent[uEvent]=pfn;
}

void CUICombo::RegisterControl(const std::string& strID, CUIControl& control)
{
	control.SetID(strID);
	AddControl(&control);
}

void CUICombo::AddControl(CUIControl* pControl)
{
	pControl->SetParent(this);
	m_Controls.push_back(pControl);
}

CUIControl* CUICombo::GetControl(const std::string& strID)
{
	for(uint32 i=0; i < m_Controls.size(); i++)
	{
		if(m_Controls[i]->GetID() == strID)
		{
			return m_Controls[i];
		}
	}
	return NULL;
}

CUIControl* CUICombo::GetControl(const std::string& strID, uint32 nControlType)
{
	for(uint32 i=0; i < m_Controls.size(); i++)
	{
		CUIControl* pControl = m_Controls[i];
		if(pControl->GetID() == strID && pControl->GetType() == nControlType)
		{
			return pControl;
		}
	}
	return NULL;
}

CUIControl* CUICombo::getFirstControl()
{
	if (m_Controls.size()>0)
	{
		return m_Controls[0];
	}
	return NULL;
}

CUIControl* CUICombo::getLastControl()
{
	if (m_Controls.size()>0)
	{
		return m_Controls[m_Controls.size()-1];
	}
	return NULL;
}

CUIControl* CUICombo::GetNextControl(CUIControl* pControl)
{
	for(std::vector<CUIControl*>::iterator it=m_Controls.begin(); it!=m_Controls.end(); it++)
	{
		if (*it==pControl)
		{
			if (it!=m_Controls.end())
			{
				return *++it;
			}
			else
			{
				return *m_Controls.begin();
			}
		}
	}
	return NULL;
}

CUIControl* CUICombo::GetPrevControl(CUIControl* pControl)
{
	for(std::vector<CUIControl*>::iterator it=m_Controls.begin(); it!=m_Controls.end(); it++)
	{
		if (*it==pControl)
		{
			if (it!=m_Controls.begin())
			{
				return *--it;
			}
			else
			{
				return *m_Controls.end();
			}
		}
	}
	return NULL;
}

void CUICombo::ClearRadioButtonGroup(uint32 nButtonGroup)
{
	// Find all radio buttons with the given group number
	for(uint32 i=0; i < m_Controls.size(); i++)
	{
		CUIControl* pControl = m_Controls[i];

		if(pControl->GetType() == UI_CONTROL_RADIOBUTTON)
		{
			CUIRadioButton* pRadioButton = (CUIRadioButton*) pControl;

			if(pRadioButton->GetButtonGroup() == nButtonGroup)
				pRadioButton->SetChecked(false);
		}
	}
}

void CUICombo::ClientToScreen(RECT& rc)
{
	CUIControl::ClientToScreen(rc);
}

void CUICombo::ScreenToClient(RECT& rc)
{
}

void CUICombo::FocusDefaultControl()
{
	// Check for default control in this dialog
	for(uint32 i=0; i < m_Controls.size(); i++)
	{
		CUIControl* pControl = m_Controls[i];
		if(pControl->IsDefault())
		{
			// Remove focus from the current control
			ClearFocus();

			// Give focus to the default control
			s_pControlFocus = pControl;
			s_pControlFocus->OnFocusIn();
			return;
		}
	}
}

void CUICombo::OnChildSize(const CRect<int>& rc)
{
	// change Controls' size
	for(uint32 i=0; i < m_Controls.size(); i++)
	{
		CUIControl* pControl = m_Controls[i];
		if (pControl)
		{
			pControl->OnSize(rc);
		}
	}
}

void CUICombo::OnSize(const CRect<int>& rc)
{
	CUIControl::OnSize(rc);
	OnChildSize(m_rcBoundingBox);
}

bool CUICombo::OnCycleFocus(bool bForward)
{
	//CUIControl *pControl = NULL;
	//CUICombo *pDialog = NULL; // pDialog and pLastDialog are used to track wrapping of
	//CUICombo *pLastDialog;    // focus from first control to last or vice versa.

	//if(s_pControlFocus == NULL)
	//{
	//	// If s_pControlFocus is NULL, we focus the first control of first dialog in
	//	// the case that bForward is true, and focus the last control of last dialog when
	//	// bForward is false.
	//	//
	//	if(bForward)
	//	{
	//		for(uint32 d=0; d<m_Dialogs.size(); ++d)
	//		{
	//			pDialog = pLastDialog = m_Dialogs[d];
	//			if(pDialog && pDialog->m_Controls.size() > 0)
	//			{
	//				pControl = pDialog->m_Controls[0];
	//				break;
	//			}
	//		}
	//	}
	//	else
	//	{
	//		for(uint32 d=m_Dialogs.size()-1; d>=0; --d)
	//		{
	//			pDialog = pLastDialog = m_Dialogs[d];
	//			if(pDialog && pDialog->m_Controls.size() > 0)
	//			{
	//				pControl = pDialog->m_Controls[pDialog->m_Controls.size() - 1];
	//				break;
	//			}
	//		}
	//	}
	//	if(!pDialog || !pControl)
	//	{
	//		// No dialog has been registered yet or no controls have been
	//		// added to the dialogs. Cannot proceed.
	//		return true;
	//	}
	//}
	//else if(s_pControlFocus->GetParentDialog() != this)
	//{
	//	// If a control belonging to another dialog has focus, let that other
	//	// dialog handle this event by returning false.
	//	//
	//	return false;
	//}
	//else
	//{
	//	// Focused control belongs to this dialog. Cycle to the
	//	// next/previous control.
	//	pLastDialog = s_pControlFocus->GetParentDialog();
	//	pControl = (bForward) ? GetNextControl(s_pControlFocus) : GetPrevControl(s_pControlFocus);
	//	pDialog = pControl->GetParentDialog();
	//}


	//for(int i=0; i < 0xffff; i++)
	//{
	//	// If we just wrapped from last control to first or vice versa,
	//	// set the focused control to NULL. This state, where no control
	//	// has focus, allows the camera to work.
	//	int nLastDialogIndex = -1;//m_Dialogs.IndexOf(pLastDialog);
	//	for (uint32 i=0; i<m_Dialogs.size(); ++i)
	//	{
	//		if(pLastDialog == m_Dialogs[i])
	//		{
	//			nLastDialogIndex = i;
	//			break;
	//		}
	//	}
	//	int nDialogIndex = -1;//m_Dialogs.IndexOf(pDialog);
	//	for (uint32 i=0; i<m_Dialogs.size(); ++i)
	//	{
	//		if(pDialog == m_Dialogs[i])
	//		{
	//			nDialogIndex = i;
	//			break;
	//		}
	//	}

	//	if((!bForward && nLastDialogIndex < nDialogIndex) ||
	//		(bForward && nDialogIndex < nLastDialogIndex))
	//	{
	//		if(s_pControlFocus)
	//			s_pControlFocus->OnFocusOut();
	//		s_pControlFocus = NULL;
	//		return true;
	//	}

	//	// If we've gone in a full circle then focus doesn't change
	//	if(pControl == s_pControlFocus)
	//		return true;

	//	// If the dialog accepts keybord input and the control can have focus then
	//	// move focus
	//	if(pControl->GetParentDialog()->IsKeyboardInputEnabled() && pControl->CanHaveFocus())
	//	{
	//		if(s_pControlFocus)
	//			s_pControlFocus->OnFocusOut();
	//		s_pControlFocus = pControl;
	//		s_pControlFocus->OnFocusIn();
	//		return true;
	//	}

	//	pLastDialog = pDialog;
	//	pControl = (bForward) ? GetNextControl(pControl) : GetPrevControl(pControl);
	//	pDialog = pControl->GetParentDialog();
	//}
	return false;
}
