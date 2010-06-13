#pragma once

#include "UICheckBox.h"

//-----------------------------------------------------------------------------
// RadioButton control
//-----------------------------------------------------------------------------
class DLL_EXPORT CUIRadioButton : public CUICheckBox
{
public:
	CUIRadioButton();
	virtual void XMLParse(const TiXmlElement* pControlElement);
	virtual void OnPressUp();
	void SetChecked(bool bChecked) {m_bChecked = bChecked;}
	void SetButtonGroup(uint32 uButtonGroup) { m_uButtonGroup = uButtonGroup; }
	uint32 GetButtonGroup() { return m_uButtonGroup; }
protected:
	uint32 m_uButtonGroup;
};