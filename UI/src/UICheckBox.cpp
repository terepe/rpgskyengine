#include "UICheckBox.h"
#include "tinyxml.h"

CUICheckBox::CUICheckBox()
{
	m_Type = UI_CONTROL_CHECKBOX;
	m_bChecked = false;
}

void CUICheckBox::XMLParse(TiXmlElement* pControlElement)
{
	CUIButton::XMLParse(pControlElement);
	// Checked
	if (pControlElement->Attribute("checked"))
	{
		std::string strChecked = pControlElement->Attribute("checked");
		if ("true" == strChecked)
		{
			SetChecked(true);
		}
	}
}

void CUICheckBox::SetStyle(const std::string& strStyleName)
{
	m_Style.SetStyle(strStyleName);
	m_StyleChecked.SetStyle(strStyleName+".checked");
}

void CUICheckBox::OnPressUp()
{
	m_bChecked = !m_bChecked; 
	SendEvent(EVENT_CHECKBOX_CHANGED, this); 
}

void CUICheckBox::OnFrameRender(double fTime, float fElapsedTime)
{
	CONTROL_STATE iState = GetState();
	float fBlendRate = (iState == CONTROL_STATE_PRESSED) ? 0.0f : 0.8f;
	m_Style.draw(m_rcBoundingBox,m_wstrText,m_bChecked?CONTROL_STATE_HIDDEN:iState,fElapsedTime,fBlendRate);
	m_StyleChecked.draw(m_rcBoundingBox,m_wstrText,m_bChecked?iState:CONTROL_STATE_HIDDEN,fElapsedTime,fBlendRate);
}