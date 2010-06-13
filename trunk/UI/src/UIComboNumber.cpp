#include "UIComboNumber.h"
#include "tinyxml.h"

CUIComboNumber::CUIComboNumber():
m_fExtentSize(0.1f)
{
}

CUIComboNumber::~CUIComboNumber()
{	
}

void CUIComboNumber::XMLParse(const TiXmlElement* pControlElement)
{
	CUICombo::XMLParse(pControlElement);
	if (pControlElement->Attribute("extent"))
	{
		pControlElement->Attribute("extent",&m_fExtentSize);
	}
}

void CUIComboNumber::OnControlRegister()
{
	CUICombo::OnControlRegister();

	RegisterControl("IDC_BTN_UP",m_BtnUp);
	RegisterControl("IDC_BTN_DOWN",m_BtnDown);
	RegisterControl("IDC_EDIT_BOX",m_EditBox);

	RegisterControlEvent("IDC_BTN_UP",		(PEVENT)&CUIComboNumber::OnBtnUp);
	RegisterControlEvent("IDC_BTN_DOWN",	(PEVENT)&CUIComboNumber::OnBtnDown);
	RegisterControlEvent("IDC_EDIT_BOX",	(PEVENT)&CUIComboNumber::OnEditBoxChanged);
}

void CUIComboNumber::SetText(const std::wstring& wstrText)
{
	m_EditBox.SetText(wstrText);
}

float CUIComboNumber::getFloat()
{
	return m_EditBox.GetFloat();
}

float CUIComboNumber::getExtentSize()
{
	return m_fExtentSize;
}

void CUIComboNumber::setFloat(float fValue)
{
	m_EditBox.SetFloat(fValue); 
}

void CUIComboNumber::setExtentSize(float fExtentSize)
{
	m_fExtentSize = fExtentSize;
}

void CUIComboNumber::OnBtnUp()
{
	m_EditBox.SetFloat(m_EditBox.GetFloat()+m_fExtentSize);
	SendEvent(EVENT_DEFAULT);
}

void CUIComboNumber::OnBtnDown()
{
	m_EditBox.SetFloat(m_EditBox.GetFloat()-m_fExtentSize);
	SendEvent(EVENT_DEFAULT);
}

void CUIComboNumber::OnEditBoxChanged()
{
	SendEvent(EVENT_DEFAULT);
}
