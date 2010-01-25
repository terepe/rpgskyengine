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

static const char* gs_szUIControlType[] =
{
	"dialog",
	"combo",
	"button",
	"static",
	"image",
	"checkbox",
	"radiobutton",
	"combobox",
	"slider",
	"progress",
	"editbox",
	"imeeditbox",
	"listbox",
	"grid",
	"scrollbar",
	"display",
};

const char* GetUIControlTypeName(UI_CONTROL_TYPE eControlType)
{
	return gs_szUIControlType[eControlType];
}

UI_CONTROL_TYPE GetUIControlType(const std::string& strControlTypeName)
{
	for (int i=0; i<UI_CONTROL_MAX; i++)
	{
		if (strControlTypeName == gs_szUIControlType[i])
		{
			return (UI_CONTROL_TYPE)i;
		}
	}
	return UI_CONTROL_MAX;
}

uint32 StrToAlign(std::string& strAlign)
{
	uint32 uAlign = 0;
	if (strAlign.find("TOP") != std::string::npos)
	{
		uAlign |= ALIGN_TOP;
	}
	else if (strAlign.find("VCENTER") != std::string::npos)
	{
		uAlign |= ALIGN_VCENTER;
	}
	else if (strAlign.find("BOTTOM") != std::string::npos)
	{
		uAlign |= ALIGN_BOTTOM;
	}

	if (strAlign.find("LEFT") != std::string::npos)
	{
		uAlign |= ALIGN_LEFT;
	}
	else if (strAlign.find("UCENTER") != std::string::npos)
	{
		uAlign |= ALIGN_CENTER;
	}
	else if (strAlign.find("RIGHT") != std::string::npos)
	{
		uAlign |= ALIGN_RIGHT;
	}
	return uAlign;
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

	m_x = 0;
	m_y = 0;
	m_width = 0;
	m_height = 0;
	m_nPercentWidth = 0;
	m_nPercentHeight = 0;
	m_uAlign = 0;

	ZeroMemory(&m_rcBoundingBox, sizeof(m_rcBoundingBox));
	ZeroMemory(&rcOffset, sizeof(rcOffset));
}


CUIControl::~CUIControl()
{
}

void CUIControl::SetParent(CUICombo* pControl)
{
	m_pParentDialog=NULL;
	if (pControl)
	{
		if(UI_CONTROL_COMBO==pControl->m_Type||UI_CONTROL_DIALOG==pControl->m_Type)
		{
			m_pParentDialog = pControl;
		}
		else
		{
			MessageBoxW(NULL,L"The parent of this control is unknow ",L"Error",0);
		}
	}
}

void CUIControl::XMLParse(TiXmlElement* pControlElement)
{
	// control name
	if (pControlElement->Attribute("id"))
	{
		std::string strID = pControlElement->Attribute("id");
		SetID(strID);
	}
	{
		if (pControlElement->Attribute("offset"))
		{
			StrToRect(pControlElement->Attribute("offset"),rcOffset);
		}
		if (pControlElement->Attribute("rect"))
		{
			RECT rc;
			StrToRect(pControlElement->Attribute("rect"),rc);

			SetLocation(rc.left, rc.top);
			SetSize(rc.right,rc.bottom,false,false);
		}
		else
		{
			// control pos
			if (pControlElement->Attribute("x")||pControlElement->Attribute("y"))
			{
				int nx=0,ny=0;
				pControlElement->Attribute("x",&nx);
				pControlElement->Attribute("y",&ny);
				SetLocation(nx, ny);
			}

			// control size
			if (pControlElement->Attribute("width")||pControlElement->Attribute("height"))
			{
				int nWidth=0,nHeight=0;
				bool bPercentWidth=false,bPercentHeight=false;
				pControlElement->Attribute("width",&nWidth);
				pControlElement->Attribute("height",&nHeight);
				if (pControlElement->Attribute("width"))
				{
					std::string strWidth = pControlElement->Attribute("width");
					if (strWidth.find("%") != std::string::npos)
					{
						bPercentWidth = true;
					}
				}
				if (pControlElement->Attribute("height"))
				{
					std::string strHeight = pControlElement->Attribute("height");
					if (strHeight.find("%") != std::string::npos)
					{
						bPercentHeight = true;
					}
				}
				SetSize(nWidth,nHeight,bPercentWidth,bPercentHeight);
			}

			// control align
			if (pControlElement->Attribute("align"))
			{
				std::string strAlign = pControlElement->Attribute("align");
				uint32 uAlign = StrToAlign(strAlign);
				SetAlign(uAlign);
			}
		}

	}
	// Tip
	if(pControlElement->Attribute("tip"))
	{
		WCHAR strText[256]={0};
		MultiByteToWideChar(CP_UTF8,0,pControlElement->Attribute("tip"),-1,strText,256);
		SetTip(strText);
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
	else
	{
		SetStyle(GetUIControlTypeName(m_Type));
	}
	// text
	if (pControlElement->Attribute("text"))
	{
		WCHAR strText[256]={0};
		MultiByteToWideChar(CP_UTF8,0,pControlElement->Attribute("text"),-1,strText,256);
		SetText(strText);
	}
	else if(pControlElement->GetText())
	{
		WCHAR strText[256]={0};
		MultiByteToWideChar(CP_UTF8,0,pControlElement->GetText(),-1,strText,256);
		SetText(strText);
	}
}

void CUIControl::OnMove(int x, int y)
{
	int w = m_rcBoundingBox.right-m_rcBoundingBox.left;
	int h = m_rcBoundingBox.bottom-m_rcBoundingBox.top;
	SetRect(&m_rcBoundingBox,x,y,x+w,y+h);
	UpdateRects();
}

void CUIControl::SetLocation(int x, int y)
{
	m_x = x; m_y = y;
	UpdateRects();
}

void CUIControl::OnSize(const RECT& rc)
{
	int nX = m_x;
	int nY = m_y;
	int nWidth=m_width;
	int nHeight=m_height;
	if (m_nPercentWidth != 0)
	{
		nWidth = (rc.right-rc.left) * m_nPercentWidth/100;
	}
	if (m_nPercentHeight != 0)
	{
		nHeight = (rc.bottom-rc.top) * m_nPercentHeight/100;
	}

	if (m_uAlign & ALIGN_CENTER)
	{
		nX = (rc.right-rc.left - nWidth)/2;
	}
	else if (m_uAlign & ALIGN_RIGHT)
	{
		if (nWidth)
		{
			nX = rc.right-rc.left - nWidth;
		}
		else
		{
			nWidth = rc.right-rc.left - m_x;
		}
	}

	if (m_uAlign & ALIGN_VCENTER)
	{
		nY = (rc.bottom-rc.top - nHeight)/2;
	}
	else if (m_uAlign & ALIGN_BOTTOM)
	{
		if (nHeight)
		{
			nY = rc.bottom-rc.top - nHeight;
		}
		else
		{
			nHeight = rc.bottom-rc.top - nY;
		}
	}
	m_rcBoundingBox.left	= rc.left+nX;
	m_rcBoundingBox.right	= m_rcBoundingBox.left+nWidth;
	m_rcBoundingBox.top		= rc.top+nY;
	m_rcBoundingBox.bottom	= m_rcBoundingBox.top+nHeight;

	m_rcBoundingBox.left	+= rcOffset.left;
	m_rcBoundingBox.right	+= rcOffset.right;
	m_rcBoundingBox.top		+= rcOffset.top;
	m_rcBoundingBox.bottom	+= rcOffset.bottom;
	UpdateRects();
}

void CUIControl::SetSize(int nWidth, int nHeight, bool bPercentWidth, bool bPercentHeight)
{
	if (bPercentWidth)
	{
		m_nPercentWidth = nWidth;
	}
	else
	{
		m_width = nWidth;
	}
	if (bPercentHeight)
	{
		m_nPercentHeight = nHeight;
	}
	else
	{
		m_height = nHeight;
	}
}

void CUIControl::SetAlign(uint32 uAlign)
{
	m_uAlign = uAlign;
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


// void CUIControl::SetTextColor(D3DCOLOR Color)
// {
// 	CUIStyle* pStyle = GetStyle(0);
// 
// 	if(pStyle)
// 		pStyle->m_FontColor.States[CONTROL_STATE_NORMAL] = Color;
// }

// CUIStyle* CUIControl::GetStyle(UINT iElement)
// {
// // 	if (0 <= iElement && m_UIStyleIndices.size() > iElement)
// // 	{
// // 		return GetStyleMgr().GetStyleByID(m_UIStyleIndices[iElement]);
// // 	}
// 	return NULL;
// }


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
	return PtInRect(&m_rcBoundingBox, pt)==TRUE;
}

void CUIControl::SendEvent(uint32 uEvent, CUIControl* pControl)
{
	GetParentDialog()->SendEvent(uEvent, pControl);
}

void CUIControl::drawTip(const RECT& rc, double fTime, float fElapsedTime)
{
	if (m_wstrTip.length()>0)
	{
		RECT rect={0,0,0,0};
		UIGraph::CalcTextRect(m_wstrTip, rect);// 计算文本框大小
		rect.right	+=5;
		rect.bottom	+=5;

		if (rc.right<m_rcBoundingBox.left+rect.right)
		{
			rect.left	= rc.right-rect.right;
			rect.right	= rc.right;
		}
		else if (rc.left>m_rcBoundingBox.left)
		{
			rect.left	= rc.left;
			rect.right	= rc.left+rect.right;
		}
		else
		{
			rect.left	= m_rcBoundingBox.left;
			rect.right	= m_rcBoundingBox.left+rect.right;
		}

		if (rc.bottom<m_rcBoundingBox.bottom+rect.bottom)
		{
			if (m_rcBoundingBox.top<rect.bottom)
			{
				rect.top	= rc.bottom-rect.bottom;
				rect.bottom	= rc.bottom;
			}
			else
			{
				rect.top	= m_rcBoundingBox.top-rect.bottom;
				rect.bottom	= m_rcBoundingBox.top;
			}
		}
		else
		{
			rect.top	= m_rcBoundingBox.bottom;
			rect.bottom	= m_rcBoundingBox.bottom+rect.bottom;
		}

		CUIControl::s_TipStyle.draw(rect,m_wstrTip,CONTROL_STATE_NORMAL, fElapsedTime);
	}
	else
	{
		CUIControl::s_TipStyle.Blend(CONTROL_STATE_HIDDEN,fElapsedTime);
	}
}

void CUIControl::SetFocus(bool bFocus)
{
	if (GetParentDialog())
	{
		if (bFocus)
		{
			GetParentDialog()->RequestFocus(this);
		}
		else
		{
			if(!GetParentDialog()->IsKeyboardInputEnabled())
				m_pParentDialog->ClearFocus();
		}
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
	rc.left		= int((rc.left-m_width/2)*fScale+m_x+m_width/2);
	rc.right	= int((rc.right-m_width/2)*fScale+m_x+m_width/2);
	rc.top		= int((rc.top-m_height/2)*fScale+m_y+m_height/2);
	rc.bottom	= int((rc.bottom-m_height/2)*fScale+m_y+m_height/2);
}

void CUIControl::ScreenToClient(RECT& rc)
{
}

void CUIControl::UpdateRects()
{
}