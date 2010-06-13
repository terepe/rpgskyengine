#pragma once

#include "UIControl.h"

//-----------------------------------------------------------------------------
// Í¼Æ¬¿Ø¼þ
//-----------------------------------------------------------------------------
class DLL_EXPORT CUIImage : public CUIControl
{
public:
	CUIImage();
	virtual void XMLParse(const TiXmlElement* pControlElement);
	virtual void OnFrameRender(double fTime, float fElapsedTime);

	std::string& GetFilename() { return m_strFilename; }
	DWORD GetColor(){ return m_dwColor; }

	void SetFilename(const std::string& strFilename);
	void SetColor(DWORD dwColor){ m_dwColor = dwColor; }


protected:
	RECT m_rcTex;
	UINT m_nTexID;
	DWORD m_dwColor;
	std::string m_strFilename;      // Window text  
};
