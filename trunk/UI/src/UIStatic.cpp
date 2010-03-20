#include "UIStatic.h"
#include "UIDialog.h"
#include "FileSystem.h"
#include "tinyxml.h"

CUIStatic::CUIStatic()
{
	m_Type = UI_CONTROL_STATIC;
}

void CUIStatic::OnFrameRender(double fTime, float fElapsedTime)
{
	if (m_width==0||m_height==0)
	{
		CRect<float> rect = m_rcBoundingBox;
		UIGraph::CalcTextRect(m_wstrText,rect);
		m_rcBoundingBox=rect.getRECT();
	}
	m_Style.draw(m_rcBoundingBox,m_wstrText,GetState(), fElapsedTime);
}

void CUIStatic::SetText(const std::wstring& wstrText)
{
	m_wstrText = wstrText;
}

void CUIStatic::SetText(int32 nValue)
{
	m_wstrText = i2ws(nValue);
}		