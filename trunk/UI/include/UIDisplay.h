#pragma once
#include "UIControl.h"

class DLL_EXPORT CUIDisplay: public CUIControl//, public CDisplay
{
public:
	CUIDisplay();
	virtual void	XMLParse(const TiXmlElement* pControlElement);
	virtual void	OnFrameRender(double fTime, float fElapsedTime);
	CRect<int>		getViewport();
};
