#pragma once
#include "UICombo.h"
#include "UIButton.h"
#include "UIEditBox.h"

class CUIComboNumber : public CUICombo
{
public:
	CUIComboNumber();
	~CUIComboNumber();
public:
	virtual void OnControlRegister();
	virtual void SetText(const std::wstring& wstrText);
	void OnBtnUp();
	void OnBtnDown();
	void OnEditBoxChanged();
	float getFloat();
	float getExtentSize();
	void setFloat(float fValue);
	void setExtentSize(float fExtentSize);
private:
	CUIStatic	m_StaticName;
	CUIButton	m_BtnUp;
	CUIButton	m_BtnDown;
	CUIEditBox	m_EditBox;
	float		m_fExtentSize;
};