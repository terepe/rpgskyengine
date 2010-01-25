#pragma once
#include "UICombo.h"
#include "UISLider.h"

class CUIComboColor : public CUICombo
{
public:
	CUIComboColor();
	~CUIComboColor();
public:
	virtual void OnControlRegister();
	Color32	getColor();
	void	setColor(Color32 color);
	//Vec4D	getColor();
	void	setColor(Vec4D color);
	void	OnSliderChanged();
private:
	CUISlider	m_SliderR;
	CUISlider	m_SliderG;
	CUISlider	m_SliderB;
};