#pragma once
#include "UIControl.h"

class DLL_EXPORT CUIProgress : public CUIControl
{
public:
	CUIProgress();
	virtual void	XMLParse(const TiXmlElement* pControlElement);
	virtual void	UpdateRects(); 
	virtual void	OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);
	bool			SetValue(int nValue);
	int				GetValue() const { return m_nValue; };
	void			GetRange(int &nMin, int &nMax) const { nMin = m_nMin; nMax = m_nMax; }
	void			SetRange(int nMin, int nMax);
protected:
	int		m_nValue;
	int		m_nMin;
	int		m_nMax;
	CRect<int>	m_rcButton;
};