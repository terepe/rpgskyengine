#include "UIControl.h"
#include "UICombo.h"
#include "tinyxml.h"

CUIControl* CUIControl::s_pControlFocus = NULL;
CUIControl* CUIControl::s_pControlPressed = NULL;
CUIControl* CUIControl::s_pControlMouseOver = NULL;

CUIStyle CUIControl::s_TipStyle;

HWND g_hWndUI;

HWND UIGetHWND()
{
	return g_hWndUI;
}

void UISetHWND(HWND hWnd)
{
	g_hWndUI = hWnd;
}

CUIControl::CUIControl()
{
	m_Type = UI_CONTROL_BUTTON;
	m_pUserData = NULL;

	m_bEnabled = true;
	m_bVisible = true;
	m_bMouseOver = false;
	m_bIsDefault = false;

	m_pParentDialog = NULL;

	m_rcOffset.set(0,0,0,0);
	m_rcScale.set(0,0,0,0);

	ZeroMemory(&m_rcBoundingBox, sizeof(m_rcBoundingBox));
	ZeroMemory(&m_rcOffset, sizeof(m_rcOffset));
}

CUIControl::~CUIControl()
{
}

void CUIControl::SetParent(CUICombo* pControl)
{
	m_pParentDialog=NULL;
	if (pControl)
	{
		if(pControl->isCombo())
		{
			m_pParentDialog = pControl;
		}
		else
		{
			std::wstring wstrInfo = FormatW(L"void CUIControl::SetParent(CUICombo* pControl)\nThe parent of this control is unknown.\nControl:%s; Parent Control:%s;",s2ws(GetID()).c_str(),s2ws(pControl->GetID()).c_str());
			MessageBoxW(NULL,wstrInfo.c_str(),L"Error",0);
		}
	}
}

void CUIControl::XMLParse(const TiXmlElement* pControlElement)
{
	// control name
	if (pControlElement->Attribute("id"))
	{
		std::string strID = pControlElement->Attribute("id");
		SetID(strID);
	}
	{
		if (pControlElement->Attribute("scale"))
		{
			m_rcScale.strToRect(pControlElement->Attribute("scale"));
		}

		if (pControlElement->Attribute("rect"))
		{
			m_rcOffset.strToRect(pControlElement->Attribute("rect"));
			m_rcOffset.right+=m_rcOffset.left;
			m_rcOffset.bottom+=m_rcOffset.top;
		}
		else if (pControlElement->Attribute("offset"))
		{
			m_rcOffset.strToRect(pControlElement->Attribute("offset"));
		}
	}
	// Tip
	if(pControlElement->Attribute("tip"))
	{
		const char* pText = pControlElement->Attribute("tip");
		int nLength = strlen(pText)+1;
		WCHAR* pTextBuffer = new WCHAR[nLength];
		MultiByteToWideChar(CP_UTF8,0,pText,-1,pTextBuffer,nLength);
		SetTip(pTextBuffer);
		delete pTextBuffer;
	}
	// hotkey
	if (pControlElement->Attribute("hotkey"))
	{
		std::string strHotkey = pControlElement->Attribute("hotkey");
		SetHotkey(strHotkey);
	}
	// control Is Default
	if (pControlElement->Attribute("IsDefault"))
	{
		bool bIsDefault = false;
		pControlElement->Attribute("IsDefault", &bIsDefault);
		SetDefault(bIsDefault);
	}
	// style
	if (pControlElement->Attribute("style"))
	{
		SetStyle(pControlElement->Attribute("style"));
	}
	else if (pControlElement->Attribute("type"))
	{
		SetStyle(pControlElement->Attribute("type"));
	}
	else
	{
		SetStyle("dialog");
	//	std::wstring wstrInfo = FormatW(L"Can not find the style of this control.\nControl:%s;",s2ws(GetID()).c_str());
	//	MessageBoxW(NULL,wstrInfo.c_str(),L"Register Control Warn!",0);
	}
	// text
	if (pControlElement->GetText()||pControlElement->Attribute("text"))
	{
		const char* pText = pControlElement->GetText();
		if(pText==NULL)
		{
			pText = pControlElement->Attribute("text");
		}
		int nLength = strlen(pText)+1;
		WCHAR* pTextBuffer = new WCHAR[nLength];
		MultiByteToWideChar(CP_UTF8,0,pText,-1,pTextBuffer,nLength);
		SetText(pTextBuffer);
		delete pTextBuffer;
	}
}

void CUIControl::OnMove(int x, int y)
{
	int w = m_rcBoundingBox.getWidth();
	int h = m_rcBoundingBox.getHeight();
	m_rcBoundingBox.set(x,y,x+w,y+h);
	UpdateRects();
}

void CUIControl::OnSize(const CRect<int>& rc)
{
	m_rcBoundingBox.left	= rc.left+rc.getWidth()*m_rcScale.left/100;
	m_rcBoundingBox.right	= rc.left+rc.getWidth()*m_rcScale.right/100;
	m_rcBoundingBox.top		= rc.top+rc.getHeight()*m_rcScale.top/100;
	m_rcBoundingBox.bottom	= rc.top+rc.getHeight()*m_rcScale.bottom/100;

	m_rcBoundingBox+= m_rcOffset;
	UpdateRects();
}

const CRect<int>& CUIControl::getOffset()
{
	return m_rcOffset;
}

const CRect<int>& CUIControl::getScale()
{
	return m_rcScale;
}

void CUIControl::setOffset(const CRect<int>& rc)
{
	m_rcOffset = rc;
}

void CUIControl::setScale(const CRect<int>& rc)
{
	m_rcScale = rc;
}

void CUIControl::SetHotkey(std::string& strHotkey)
{
	if (strHotkey.size() == 1)
	{
		if (0 <= strHotkey[0] && 9 >= strHotkey[0])
		{
			m_nHotkey = strHotkey[0];
		}
		else if ('A' <= strHotkey[0] && 'Z' >= strHotkey[0])
		{
			m_nHotkey = strHotkey[0];
		}
		else if ('a' <= strHotkey[0] && 'z' >= strHotkey[0])
		{
			m_nHotkey = strHotkey[0]+'A'-'a';
		}
	}
}

void CUIControl::SetStyle(const std::string& strStyleName)
{
	m_Style.SetStyle(strStyleName);
}

void CUIControl::Refresh()
{
	m_bMouseOver = false;
	//IsFocus()
	//m_bHasFocus = false;
}

bool CUIControl::ContainsPoint(POINT pt)
{
	return m_rcBoundingBox.ptInRect(pt);
}

void CUIControl::SendEvent(uint32 uEvent)
{
	if (GetParentDialog())
	{
		std::string strEvent=GetID()+Format("_%d",uEvent);
		GetParentDialog()->progressEvent(strEvent);
	}
}

void CUIControl::drawTip(const CRect<int>& rc, double fTime, float fElapsedTime)
{
	if (m_wstrTip.length()<=0)
	{
		return;
	}
	CRect<float> rect(0.0f,0.0f,1000.0f,1000.0f);
	UIGraph::CalcTextRect(m_wstrTip, rect);// 计算文本框大小

	const StyleElement* pFontStyleElement = CUIControl::s_TipStyle.getStyleData().getFontStyleElement();
	if(pFontStyleElement)
	{
		rect -= pFontStyleElement->setOffset[CONTROL_STATE_NORMAL];
	}
	int nTipWidth = rect.getWidth();
	int nTipHeight = rect.getHeight();

	if (rc.right<m_rcBoundingBox.left+nTipWidth)
	{
		rect.left	= rc.right-nTipWidth;
		rect.right	= rc.right;
	}
	else if (rc.left>m_rcBoundingBox.left)
	{
		rect.right	= rc.left+nTipWidth;
		rect.left	= rc.left;
	}
	else
	{
		rect.right	= m_rcBoundingBox.left+nTipWidth;
		rect.left	= m_rcBoundingBox.left;
	}

	if (rc.bottom<m_rcBoundingBox.bottom+nTipHeight)
	{
		if (m_rcBoundingBox.top<nTipHeight)
		{
			rect.top	= rc.bottom-nTipHeight;
			rect.bottom	= rc.bottom;
		}
		else
		{
			rect.top	= m_rcBoundingBox.top-nTipHeight;
			rect.bottom	= m_rcBoundingBox.top;
		}
	}
	else
	{
		rect.top	= m_rcBoundingBox.bottom;
		rect.bottom	= m_rcBoundingBox.bottom+nTipHeight;
	}

	CUIControl::s_TipStyle.draw(rect,m_wstrTip,CONTROL_STATE_NORMAL, fElapsedTime);
}

void CUIControl::SetFocus(bool bFocus)
{
	if (bFocus)
	{
		if(s_pControlFocus==this)
			return;
		if(!CanHaveFocus())
			return;
		CUIControl* pOldControlFocus=s_pControlFocus;
		s_pControlFocus=this;
		if(pOldControlFocus&&!pOldControlFocus->IsFocus())
		{
			pOldControlFocus->OnFocusOut();
		}
		OnFocusIn();
	}
	else if(GetParentDialog()&&!GetParentDialog()->IsKeyboardInputEnabled())
	{
		ClearFocus();
	}
}

void CUIControl::ClearFocus()
{
	clearFocus();
	ReleaseCapture();
}

void CUIControl::clearFocus()
{
	if(s_pControlFocus)
	{
		CUIControl* pOldControlFocus=s_pControlFocus;
		s_pControlFocus=NULL;
		pOldControlFocus->OnFocusOut();
	}
}

void CUIControl::ClientToScreen(RECT& rc)
{
	if (m_pParentDialog)
	{
		m_pParentDialog->ClientToScreen(rc);
	}
	float fScale = 1;//m_Style.m_crSpriteColor[DIALOF_STYLE_SPRITE_INDEX_BACKGROUND].a / 255.0f;
	//int nCenterX = m_x+m_width/2;
	//int nCenterY = m_y+m_height/2;
// 	rc.left		= int((rc.left-m_width/2)*fScale+m_x+m_width/2);
// 	rc.right	= int((rc.right-m_width/2)*fScale+m_x+m_width/2);
// 	rc.top		= int((rc.top-m_height/2)*fScale+m_y+m_height/2);
// 	rc.bottom	= int((rc.bottom-m_height/2)*fScale+m_y+m_height/2);
}

void CUIControl::ScreenToClient(RECT& rc)
{
}

void CUIControl::UpdateRects()
{
}

CONTROL_STATE CUIControl::GetState()
{
	CONTROL_STATE iState = CONTROL_STATE_NORMAL;

	CUICombo* pParent=GetParentDialog(); 
	if(pParent&&pParent->GetState()==CONTROL_STATE_HIDDEN)
	{
		iState = CONTROL_STATE_HIDDEN;
	}
	else if(m_bVisible == false)
	{
		iState = CONTROL_STATE_HIDDEN;
	}
	else if(m_bEnabled == false)
	{
		iState = CONTROL_STATE_DISABLED;
	}
	else if(IsPressed())
	{
		iState = CONTROL_STATE_PRESSED;
	}
	else if(m_bMouseOver)
	{
		iState = CONTROL_STATE_MOUSEOVER;
	}
	else if(IsFocus())
	{
		iState = CONTROL_STATE_FOCUS;
	}
	return iState;
}