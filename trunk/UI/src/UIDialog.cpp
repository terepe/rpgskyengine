#include "UIDialog.h"
#include "tinyxml.h"

CUIDialog::CUIDialog()
{
	m_nMouseOriginX = 0;
	m_nMouseOriginY = 0;
	m_Type = UI_CONTROL_DIALOG;

	m_bCaption = false;
	m_bCanMove = true;
	m_bMinimized = false;
	m_nCaptionHeight = 0;
}

CUIDialog::~CUIDialog()
{
	if (GetParentDialog())
	{
		((CUIDialog*)GetParentDialog())->UnregisterDialog(this);
	}

	for(uint32 i=0; i < m_Dialogs.size(); i++)
	{
		if (m_Dialogs[i]->GetParentDialog() == this)
		{
			m_Dialogs[i]->SetParent(NULL);
		}
	}

	for(uint32 i=0; i < m_AssignControls.size(); i++)
	{
		S_DEL(m_AssignControls[i]);
	}
}

bool CUIDialog::Create(CUICombo* pParent)
{
	SetParent(pParent);
	if (GetParentDialog()!=NULL)
	{
		((CUIDialog*)GetParentDialog())->RegisterDialog(this);
	}
	OnControlRegister();
	if (false == LoadXml(UIGetDialogRes(), m_strID))
	{
		return false;
	}
	if (false == OnInitDialog())
	{
		return false;
	}
	return true;
}

bool CUIDialog::Create(const std::string& strID, CUICombo* pParent)
{
	SetID(strID);
	return Create(pParent);
}

bool CUIDialog::InitDialog()
{
	if (OnInitDialog()==false)
	{
		return false;
	}
	for(uint32 i=0; i < m_Dialogs.size(); i++)
	{
		if (m_Dialogs[i]->InitDialog()==false)
		{
			return false;
		}
	}
	return true;
}

bool CUIDialog::OnInitDialog()
{
	return true;
}

#include "IniFile.h"
void CUIDialog::loadString(const std::string& strFilename)
{
	CUICombo::loadString(strFilename);
	std::string strCaption = IniGetStr(strFilename.c_str(),m_strID.c_str(),"CAPTION");
	if (!strCaption.empty())
	{
		SetCaptionText(s2ws(strCaption));
	}
	for(std::vector<CUIDialog*>::iterator it=m_Dialogs.begin(); it != m_Dialogs.end(); it++)
	{
		(*it)->loadString(strFilename);
	}
}

void CUIDialog::XMLParse(const TiXmlElement* pElement)
{
	// canmove
	if (pElement->Attribute("canmove"))
	{
		std::string strCanMove = pElement->Attribute("canmove");
		if ("false" == strCanMove)
		{
			SetCanMove(false);
		}
	}
	// caption
	{
		const TiXmlElement *pCaptionElement = pElement->FirstChildElement("caption");
		if (pCaptionElement)
		{
			if(pCaptionElement->GetText())
			{
				WCHAR strText[256]={0};
				MultiByteToWideChar(CP_UTF8,0,pCaptionElement->GetText(),-1,strText,256);
				SetCaptionText(strText);
			}

			int nHeight = 18;
			if (pCaptionElement->Attribute("height"))
			{
				pCaptionElement->Attribute("height",&nHeight);
			}
			SetCaptionHeight(nHeight);
			SetCaptionEnable(true);
		}
	}
	CUICombo::XMLParse(pElement);
}

void CUIDialog::SetStyle(const std::string& strStyleName)
{
	m_Style.SetStyle(strStyleName);
	m_StyleCaption.SetStyle(strStyleName+".caption");
}

void CUIDialog::UpdateRects()
{
	CUICombo::UpdateRects();
	if(m_bMinimized)
	{
		m_rcBoundingBox.bottom = m_rcBoundingBox.top+m_nCaptionHeight*2;
	}
	m_rcCaption = m_rcBoundingBox;
	m_rcCaption.bottom = m_rcCaption.top+m_nCaptionHeight;
}

bool CUIDialog::ContainsPoint(POINT pt)
{
	if (CUICombo::ContainsPoint(pt))
	{
		return true;
	}
	CUIControl* pControl = GetControlAtPoint(pt);
	return pControl!=NULL;
}

void CUIDialog::OnFrameMove(double fTime, float fElapsedTime)
{
	if(!m_bMinimized&&m_bVisible)
	{
		for(uint32 i=0; i < m_Controls.size(); i++)
		{
			if (m_Controls[i])
			{
				m_Controls[i]->OnFrameMove(fTime, fElapsedTime);
			}
		}
		//
		for (uint32 i=0; i<m_Dialogs.size(); ++i)
		{
			m_Dialogs[i]->OnFrameMove(fTime, fElapsedTime);
		}
	}
}

void CUIDialog::OnFrameRender(double fTime, float fElapsedTime)
{
	// For invisible dialog, out now.
	if((m_bMinimized && !m_bCaption))
		return;

	// If the dialog is minimized, skip rendering
	// its controls.
	if(!m_bMinimized)
	{
		CUICombo::OnFrameRender(fTime, fElapsedTime);
	}

	if(m_bCaption)
	{
		std::wstring wstrOutput;
		wstrOutput = m_wstrCaption;
		if(m_bMinimized)
		{
			wstrOutput += L"Minimized";
		}
		m_StyleCaption.draw(m_rcCaption, wstrOutput,GetState(), fElapsedTime);
	}

	//
	for (uint32 i=0; i<m_Dialogs.size(); ++i)
	{
		CUIDialog* pDialog = m_Dialogs[i];
		if (pDialog->IsVisible()||pDialog->isStyleVisible())
		{
			pDialog->OnFrameRender(fTime, fElapsedTime);
		}
	}
	return;
}

bool CUIDialog::postMsg(const std::string& strMsg)
{
	for (int i=m_Dialogs.size()-1; i>=0; --i)
	{
		if (m_Dialogs[i]->IsVisible())
		{
			if(m_Dialogs[i]->postMsg(strMsg))
			{
				return true;
			}
		}
	}
	return CUICombo::postMsg(strMsg);
}

bool CUIDialog::postMsg(uint32 uEvent)
{
	for (int i=m_Dialogs.size()-1; i>=0; --i)
	{
		if (m_Dialogs[i]->IsVisible())
		{
			if(m_Dialogs[i]->postMsg(uEvent))
			{
				return true;
			}
		}
	}
	return CUICombo::postMsg(uEvent);
}

bool CUIDialog::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	for (int i=m_Dialogs.size()-1; i>=0; --i)
	{
		if (m_Dialogs[i]->IsVisible())
		{
			if(m_Dialogs[i]->MsgProc(hWnd, uMsg, wParam, lParam))
			{
				return true;
			}
		}
	}

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

CUIDialog* CUIDialog::GetChildDialogAtPoint(POINT pt)
{
	for (int i=m_Dialogs.size()-1; i>=0; --i)
	{
		CUIDialog* pDialog = m_Dialogs[i];
		if(pDialog==NULL)
		{
			continue;
		}
		if(pDialog->ContainsPoint(pt)&&pDialog->GetEnabled() && pDialog->IsVisible())
		{
			return pDialog;
		}
	}
	return NULL;
}

void CUIDialog::SetVisible(bool bVisible)
{
	CUICombo::SetVisible(bVisible);
	if (bVisible)
	{
		toTop();
	}
}

void CUIDialog::SetFocus(bool bFocus)
{
	CUICombo::SetFocus(bFocus);
	if (bFocus)
	{
		toTop();
	}
}

void CUIDialog::OnMouseMove(POINT point)
{
	if (IsPressed())
	{
		OnMove(point.x - m_nMouseOriginX,point.y - m_nMouseOriginY);
		return;
	}
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnMouseMove(point);
		return;
	}
	// 
	CUICombo::OnMouseMove(point);
}

void CUIDialog::OnMouseWheel(POINT point,short wheelDelta)
{
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnMouseWheel(point,wheelDelta);
		return;
	}
	CUICombo::OnMouseWheel(point,wheelDelta);
}

void CUIDialog::OnLButtonDblClk(POINT point)
{
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnLButtonDblClk(point);
		return;
	}
	CUICombo::OnLButtonDblClk(point);
	// 标题
	if(m_bCaption&&m_rcCaption.ptInRect(point))
	{
		ReleaseCapture();
		//m_bMinimized = !m_bMinimized;
		UpdateRects();
		return;
	}
}
void CUIDialog::OnLButtonDown(POINT point)
{
	// 设置为焦点对话框
	toTop();
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnLButtonDown(point);
		return;
	}
	CUICombo::OnLButtonDown(point);
	// 标题
	if(m_bCaption&&m_bCanMove&&m_rcCaption.ptInRect(point))
	{
		m_nMouseOriginX = point.x-m_rcBoundingBox.left;
		m_nMouseOriginY = point.y-m_rcBoundingBox.top;
		SetPressed(true);
		//SetCapture(UIGetHWND());//会使对话框超出window窗口
		return;
	}
}
void CUIDialog::OnLButtonUp(POINT point)
{
	// 消除按住状态
	if(IsPressed())
	{
		SetPressed(false);
		//ReleaseCapture();
		return;
	}
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnLButtonUp(point);
		return;
	}
	CUICombo::OnLButtonUp(point);
}
void CUIDialog::OnRButtonDblClk(POINT point)
{
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnRButtonDblClk(point);
		return;
	}
	CUICombo::OnRButtonDblClk(point);
}
void CUIDialog::OnRButtonDown(POINT point)
{
	// 设置为焦点对话框
	toTop();
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnRButtonDown(point);
		return;
	}
	CUICombo::OnRButtonDown(point);
}
void CUIDialog::OnRButtonUp(POINT point)
{
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnRButtonUp(point);
		return;
	}
	CUICombo::OnRButtonUp(point);
}
void CUIDialog::OnMButtonDblClk(POINT point)
{
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnMButtonDblClk(point);
		return;
	}
	CUICombo::OnMButtonDblClk(point);
}
void CUIDialog::OnMButtonDown(POINT point)
{
	// 设置为焦点对话框
	toTop();
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnMButtonDown(point);
		return;
	}
	CUICombo::OnMButtonDown(point);
}
void CUIDialog::OnMButtonUp(POINT point)
{
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnMButtonUp(point);
		return;
	}
	CUICombo::OnMButtonUp(point);
}

void CUIDialog::OnMove(int x, int y)
{
	CUICombo::OnMove(x, y);
	CRect<int> rcChild = m_rcBoundingBox;
	rcChild.top+=m_nCaptionHeight;
	OnChildSize(rcChild);
}

void CUIDialog::ClientToScreen(RECT& rc)
{
	CUICombo::ClientToScreen(rc);
	rc.top		+= m_bCaption?m_nCaptionHeight:0;
	rc.bottom	+= m_bCaption?m_nCaptionHeight:0;
}

void CUIDialog::ScreenToClient(RECT& rc)
{
}

bool CUIDialog::OnCycleFocus(bool bForward)
{
	CUIControl *pControl = NULL;
	CUICombo *pDialog = NULL; // pDialog and pLastDialog are used to track wrapping of
	CUICombo *pLastDialog;    // focus from first control to last or vice versa.

	if(s_pControlFocus == NULL)
	{
		// If s_pControlFocus is NULL, we focus the first control of first dialog in
		// the case that bForward is true, and focus the last control of last dialog when
		// bForward is false.
		//
		if(bForward)
		{
			for(uint32 d=0; d<m_Dialogs.size(); ++d)
			{
				pDialog = pLastDialog = m_Dialogs[d];
				if(pDialog)
				{
					pControl = pDialog->getFirstControl();
					if (pControl)
					{
						break;
					}
				}
			}
		}
		else
		{
			for(uint32 d=m_Dialogs.size()-1; d>=0; --d)
			{
				pDialog = pLastDialog = m_Dialogs[d];
				if(pDialog)
				{
					pControl = pDialog->getLastControl();
					if (pControl)
					{
						break;
					}
				}
			}
		}
		if(!pDialog || !pControl)
		{
			// No dialog has been registered yet or no controls have been
			// added to the dialogs. Cannot proceed.
			return true;
		}
	}
	else if(s_pControlFocus->GetParentDialog() != this)
	{
		// If a control belonging to another dialog has focus, let that other
		// dialog handle this event by returning false.
		//
		return false;
	}
	else
	{
		// Focused control belongs to this dialog. Cycle to the
		// next/previous control.
		pLastDialog = s_pControlFocus->GetParentDialog();
		pControl = (bForward) ? GetNextControl(s_pControlFocus) : GetPrevControl(s_pControlFocus);
		pDialog = pControl->GetParentDialog();
	}


	for(int i=0; i < 0xffff; i++)
	{
		// If we just wrapped from last control to first or vice versa,
		// set the focused control to NULL. This state, where no control
		// has focus, allows the camera to work.
		int nLastDialogIndex = -1;//m_Dialogs.IndexOf(pLastDialog);
		for (uint32 i=0; i<m_Dialogs.size(); ++i)
		{
			if(pLastDialog == m_Dialogs[i])
			{
				nLastDialogIndex = i;
				break;
			}
		}
		int nDialogIndex = -1;//m_Dialogs.IndexOf(pDialog);
		for (uint32 i=0; i<m_Dialogs.size(); ++i)
		{
			if(pDialog == m_Dialogs[i])
			{
				nDialogIndex = i;
				break;
			}
		}

		if((!bForward && nLastDialogIndex < nDialogIndex) ||
			(bForward && nDialogIndex < nLastDialogIndex))
		{
			clearFocus();
			return true;
		}

		// If we've gone in a full circle then focus doesn't change
		if(pControl == s_pControlFocus)
			return true;

		// If the dialog accepts keybord input and the control can have focus then
		// move focus
		if(pControl->GetParentDialog()->IsKeyboardInputEnabled() && pControl->CanHaveFocus())
		{
			pControl->SetFocus(true);
			return true;
		}

		pLastDialog = pDialog;
		pControl = (bForward) ? GetNextControl(pControl) : GetPrevControl(pControl);
		pDialog = pControl->GetParentDialog();
	}
	return false;
}

void CUIDialog::OnChildSize(const CRect<int>& rc)
{
	for (uint32 i=0; i<m_Dialogs.size(); ++i)
	{
		m_Dialogs[i]->OnSize(rc);
	}
	CUICombo::OnChildSize(rc);
}

void CUIDialog::OnSize(const CRect<int>& rc)
{
	CUICombo::OnSize(rc);
	CRect<int> rcChild = m_rcBoundingBox;
	rcChild.top+=m_nCaptionHeight;
	OnChildSize(rcChild);
}

bool CUIDialog::RegisterDialog(CUIDialog *pDialog)
{
	for(uint32 i = 0; i < m_Dialogs.size(); ++i)
	{
		if(m_Dialogs[i] == pDialog)
		{
			return true;
		}
	}
	m_Dialogs.push_back(pDialog);
	return true;
}

bool CUIDialog::UnregisterDialog(CUIDialog *pDialog)
{
	for(std::vector<CUIDialog*>::iterator it=m_Dialogs.begin(); it!=m_Dialogs.end(); it++)
	{
		if((*it) == pDialog)
		{
			m_Dialogs.erase(it);
			return true;
		}
	}
	return false;
}

bool CUIDialog::setTopDialog(const CUIDialog *pDialog)
{
	if (!UnregisterDialog(this))
	{
		return false;
	}
	return RegisterDialog(this);
}

bool CUIDialog::toTop()
{
	if (!GetParentDialog())
	{
		return false;
	}
	return ((CUIDialog*)GetParentDialog())->setTopDialog(this);
}