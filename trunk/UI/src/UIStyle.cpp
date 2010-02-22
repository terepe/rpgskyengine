#include "UIStyle.h"
#include "TextRender.h"
#include "tinyxml.h"
#include "UIGraph.h"
#include "FileSystem.h"

CUIStyleMgr& GetStyleMgr()
{
	static CUIStyleMgr g_UIStyleMgr;
	return g_UIStyleMgr;
}

const char* szLayoutCell[]=
{
	"upper_left",
	"upper_middle",
	"upper_right",
	"middle_left",
	"center",
	"middle_right",
	"lower_left",
	"lower_middle",
	"lower_right",
};

const char* szControlState[]=
{
	"normal",
	"disabled",
	"hidden",
	"focus",
	"mouseover",
	"pressed",
};

inline void StrToRect(const char* str, RECT& rect)
{
	sscanf(str, "%d,%d,%d,%d", &rect.left, &rect.top, &rect.right, &rect.bottom);
}

inline uint32 StrToTextFormat(const std::string& str)
{
	std::vector<std::string> setStr;
	Tokenizer(str,setStr,"|" );
	uint32 uFormat = 0;
	for (std::vector<std::string>::iterator it=setStr.begin();it!=setStr.end();it++)
	{
		if ("TOP"==*it)
		{
			uFormat |= DTL_TOP;
		}
		else if ("VCENTER"==*it)
		{
			uFormat |= DTL_VCENTER;
		}
		else if ("BOTTOM"==*it)
		{
			uFormat |= DTL_BOTTOM;
		}
		if ("LEFT"==*it)
		{
			uFormat |= DTL_LEFT;
		}
		else if ("UCENTER"==*it)
		{
			uFormat |= DTL_CENTER;
		}
		else if ("RIGHT"==*it)
		{
			uFormat |= DTL_RIGHT;
		}
	}
	return uFormat;
}

inline void StrToXY(const char* str, int& x, int& y)
{
	sscanf(str, "%d,%d", &x, &y);
}

void CUIStyle::Blend(UINT iState, float fElapsedTime, float fRate)
{
	const CUICyclostyle& style =  GetCyclostyle();
	for (uint32 i=0; i<style.m_SpriteStyle.size(); i++)
	{
		m_crSpriteColor[i] = style.m_SpriteStyle[i].Blend(m_crSpriteColor[i], iState, fElapsedTime, fRate);
	}
	for (uint32 i=0; i<style.m_setBorder.size(); i++)
	{
		m_mapBorder[i] = style.m_setBorder[i].Blend(m_mapBorder[i], iState, fElapsedTime, fRate);
	}
	for (uint32 i=0; i<style.m_setBackgroundColor.size(); i++)
	{
		m_mapBackgroundColor[i] = style.m_setBackgroundColor[i].Blend(m_mapBackgroundColor[i], iState, fElapsedTime, fRate);
	}
	for (uint32 i=0; i<style.m_FontStyle.size(); i++)
	{
		m_crFontColor[i] = style.m_FontStyle[i].Blend(m_crFontColor[i], iState, fElapsedTime, fRate);
	}
}

void CUIStyle::SetStyle(const std::string& strName)
{
	m_strName = strName;
}

const CUICyclostyle& CUIStyle::GetCyclostyle()
{
	return GetStyleMgr().GetCyclostyle(m_strName);
}
void CUIStyle::draw(const RECT& rc, const std::wstring& wstrText, CONTROL_STATE state, float fElapsedTime, float fRate)
{
	Blend(state, fElapsedTime, fRate);
	Draw(rc, wstrText);
}

inline void offsetRect(RECT& rc1, const RECT& rc2)
{
	rc1.left+=rc2.left;
	rc1.top+=rc2.top;
	rc1.right+=rc2.right;
	rc1.bottom+=rc2.bottom;
}

void CUIStyle::Draw(const RECT& rc, const std::wstring& wstrText)
{
	GetRenderSystem().SetDepthBufferFunc(false,false);
	for (size_t i=0; i<m_crSpriteColor.size(); i++)
	{
		Color32 color = m_crSpriteColor[i].getColor();
		if(color.a==0)
		{
			continue;
		}
		const CUISpriteCyclostyle& SpriteCyclostyle = GetCyclostyle().m_SpriteStyle[i];

		RECT rcDest =rc;
		SpriteCyclostyle.updataRect(rcDest);
		if (SpriteCyclostyle.m_bDecolor)
		{
			GetRenderSystem().SetTextureStageStateDecolor();
		}

		switch(SpriteCyclostyle.m_nSpriteLayoutType)
		{
		case SPRITE_LAYOUT_WRAP:
			{
				UIGraph::DrawSprite(rcDest, rcDest, SpriteCyclostyle.m_nTexture, color.c);
			}
			break;
		case SPRITE_LAYOUT_SIMPLE:
			{
				UIGraph::DrawSprite(SpriteCyclostyle.m_rcBorder,
					rcDest, SpriteCyclostyle.m_nTexture, color.c);
			}
			break;
		case SPRITE_LAYOUT_3X3GRID:
			{
				UIGraph::DrawSprite3x3Grid(SpriteCyclostyle.m_rcBorder,
					SpriteCyclostyle.m_rcCenter,
					rcDest, SpriteCyclostyle.m_nTexture, color.c);
			}
			break;
		case SPRITE_LAYOUT_3X3GRID_WRAP:
			{
				UIGraph::DrawSprite3x3GridWrap(SpriteCyclostyle.m_rcBorder,
					SpriteCyclostyle.m_rcCenter,
					rcDest, SpriteCyclostyle.m_nTexture, color.c);
			}
			break;
		case SPRITE_LAYOUT_DISPERSE_3X3GRID:
			break;
		default:
			break;
		}

		if (SpriteCyclostyle.m_bDecolor)
		{
			GetRenderSystem().SetupRenderState();
		}
	}
	GetRenderSystem().SetTextureColorOP(0,TBOP_SOURCE2);
	GetRenderSystem().SetTextureAlphaOP(0,TBOP_SOURCE2);
	for (size_t i=0; i<m_mapBackgroundColor.size(); ++i)
	{
		Color32 color = m_mapBackgroundColor[i].getColor();
		if(color.a!=0)
		{
			RECT rcDest =rc;
			const StyleBackgroundColor&  styleBackgroundColor = GetCyclostyle().m_setBackgroundColor[i];
			styleBackgroundColor.updataRect(rcDest);
			GetGraphics().FillRect(rcDest, color);
		}
	}
	for (size_t i=0; i<m_mapBorder.size(); ++i)
	{
		Color32 color = m_mapBorder[i].getColor();
		if(color.a!=0)
		{
			RECT rcDest =rc;
			GetCyclostyle().m_setBorder[i].updataRect(rcDest);
			GetGraphics().DrawRect(rcDest, color);
		}
	}
	GetRenderSystem().SetTextureColorOP(0,TBOP_MODULATE);
	GetRenderSystem().SetTextureAlphaOP(0,TBOP_MODULATE);
	if (m_crFontColor.size()>0)
	{
		Color32 color = m_crFontColor[0].getColor();
		if(color.a!=0)
		{
			RECT rcDest =rc;
			const CUITextCyclostyle&  textCyclostyle = GetCyclostyle().m_FontStyle[0];
			textCyclostyle.updataRect(rcDest);
			GetTextRender().drawText(wstrText,-1,rcDest,textCyclostyle.uFormat, color);
		}
	}
}

void CUICyclostyle::Refresh()
{
	//m_SpriteColor.Current = m_TextureColor.States[ CONTROL_STATE_HIDDEN ];
	//m_FontColor.Current = m_FontColor.States[ CONTROL_STATE_HIDDEN ];
}

void CUICyclostyle::add(const CUICyclostyle& cyc)
{
	m_SpriteStyle.insert(m_SpriteStyle.end(), cyc.m_SpriteStyle.begin(), cyc.m_SpriteStyle.end());   
	m_setBorder.insert(m_setBorder.end(), cyc.m_setBorder.begin(), cyc.m_setBorder.end()); 
	m_setBackgroundColor.insert(m_setBackgroundColor.end(), cyc.m_setBackgroundColor.begin(), cyc.m_setBackgroundColor.end()); 
	m_FontStyle.insert(m_FontStyle.end(), cyc.m_FontStyle.begin(), cyc.m_FontStyle.end()); 
}

CUIStyleMgr::CUIStyleMgr()
{
}

void ControlBlendColor::XMLStateColor(TiXmlElement& element)
{
	const char* pszColor = element.GetText();
	if(pszColor)
	{
		Color32 c;
		c = pszColor;
		for (int i = 0; i < CONTROL_STATE_MAX; i++)
		{
			ColorOfStates[i] = c;
		}
	}

	for (int i = 0; i < CONTROL_STATE_MAX; i++)
	{
		pszColor =  element.Attribute(szControlState[i]);
		if (pszColor)
		{
			Color32 c;
			c = pszColor;
			ColorOfStates[i] = c;
		}
	}
}

void BaseCyclostyle::XML(TiXmlElement& element)
{
	if (element.Attribute("format"))
	{
		uFormat = StrToTextFormat(element.Attribute("format"));
	}
	if (element.Attribute("offset"))
	{
		StrToRect(element.Attribute("offset"), rcOffset);
	}
	TiXmlElement *pColorElement = element.FirstChildElement("color");
	if (pColorElement)
	{
		ControlBlendColor::XMLStateColor(*pColorElement);
	}
}

void BaseCyclostyle::updataRect(RECT& rc)const
{
	if (uFormat&DTL_LEFT)
	{
		rc.right = rc.left;
	}
	else if (uFormat&DTL_CENTER)
	{
		rc.left = (rc.right+rc.left)/2;
		rc.right = rc.left;
	}
	else if (uFormat&DTL_RIGHT)
	{
		rc.left = rc.right;
	}
	if (uFormat&DTL_TOP)
	{
		rc.bottom = rc.top;
	}
	else if (uFormat&DTL_VCENTER)
	{
		rc.top = (rc.bottom+rc.top)/2;
		rc.bottom = rc.top;
	}
	else if (uFormat&DTL_BOTTOM)
	{
		rc.top = rc.bottom;
	}
	offsetRect(rc,rcOffset);
}

void CUISpriteCyclostyle::updataRect(RECT& rc)const
{
	if (uFormat&DTL_TOP)
	{
		rc.bottom = rc.top+(m_rcBorder.bottom-m_rcBorder.top);
	}
	else if (uFormat&DTL_VCENTER)
	{
		rc.top = (rc.bottom+rc.top-(m_rcBorder.bottom-m_rcBorder.top))/2;
		rc.bottom = rc.top+(m_rcBorder.bottom-m_rcBorder.top);
	}
	else if (uFormat&DTL_BOTTOM)
	{
		rc.top = rc.bottom-(m_rcBorder.bottom-m_rcBorder.top);
	}
	if (uFormat&DTL_LEFT)
	{
		rc.right = rc.left+(m_rcBorder.right-m_rcBorder.left);
	}
	else if (uFormat&DTL_CENTER)
	{
		rc.left = (rc.right+rc.left-(m_rcBorder.right-m_rcBorder.left))/2;
		rc.right = rc.left+(m_rcBorder.right-m_rcBorder.left);
	}
	else if (uFormat&DTL_RIGHT)
	{
		rc.left = rc.right-(m_rcBorder.right-m_rcBorder.left);
	}
	offsetRect(rc,rcOffset);
}

void CUITextCyclostyle::updataRect(RECT& rc)const
{
	offsetRect(rc,rcOffset);
}

void CUISpriteCyclostyle::XML(TiXmlElement& element)
{
	//
	BaseCyclostyle::XML(element);
	if (element.Attribute("filename"))
	{
		m_nTexture =  GetRenderSystem().GetTextureMgr().RegisterTexture(element.Attribute("filename"));
		if (m_nTexture==0)
		{
			m_nTexture =  GetRenderSystem().GetTextureMgr().RegisterTexture(GetParentPath(GetStyleMgr().getFilename())+element.Attribute("filename"));
		}
	}
	if (element.Attribute("rect"))
	{
		const char* strRect = element.Attribute("rect");
		RECT rc;
		StrToRect(strRect, rc);
		rc.right	+= rc.left;
		rc.bottom	+= rc.top;
		m_rcBorder=rc;
		if (element.Attribute("center_rect"))
		{
			m_nSpriteLayoutType = SPRITE_LAYOUT_3X3GRID;//SPRITE_LAYOUT_3X3GRID_WRAP
			const char* strCenterRect = element.Attribute("center_rect");
			RECT rcrCenter;
			StrToRect(strCenterRect, rcrCenter);
			rcrCenter.left		+= rc.left;
			rcrCenter.top		+= rc.top;
			rcrCenter.right		+= rcrCenter.left;
			rcrCenter.bottom	+= rcrCenter.top;
			m_rcCenter=rcrCenter;
		}
		else
		{
			m_nSpriteLayoutType = SPRITE_LAYOUT_SIMPLE;
		}
	}
	else
	{
		m_nSpriteLayoutType = SPRITE_LAYOUT_WRAP;
	}

	m_bDecolor = false;
	if (element.Attribute("decolor"))
	{
		if (std::string("true")==element.Attribute("decolor"))
		{
			m_bDecolor = true;
		}
	}
}
#include "IORead.h"
bool CUIStyleMgr::Create(const std::string& strFilename)
{
	m_strFilename = strFilename;
	m_CyclostyleList.clear();
	TiXmlDocument myDocument;
	{
		IOReadBase* pRead = IOReadBase::autoOpen(strFilename);
		if (pRead)
		{
			size_t uFilesize = pRead->GetSize();
			char* pBuf = new char[uFilesize+1];
			if (pBuf)
			{
				pBuf[uFilesize] = 0;
				pRead->Read(pBuf, uFilesize);
				myDocument.LoadFormMemory(pBuf, uFilesize, TIXML_ENCODING_UTF8);
				delete[] pBuf;
			}
			IOReadBase::autoClose(pRead);
		}
	}
	if (myDocument.Error())
	{
		return false;
	}
	//获得根元素，即root。
	TiXmlElement *pRootElement = myDocument.RootElement();
	if (pRootElement==NULL)
	{
		return false;
	}
	//获得第一个Style节点。
	TiXmlElement *pStyleElement = pRootElement->FirstChildElement("element");
	while (pStyleElement)
	{
		// Style name
		CUICyclostyle Cyclostyle;

		// Sprite Info
		{
			TiXmlElement* pSpriteElement = pStyleElement->FirstChildElement("texture");
			while (pSpriteElement)
			{
				CUISpriteCyclostyle SpriteCyclostyle;
				SpriteCyclostyle.XML(*pSpriteElement);
				Cyclostyle.m_SpriteStyle.push_back(SpriteCyclostyle);
				pSpriteElement = pSpriteElement->NextSiblingElement("texture");
			}
		}

		//
		{
			TiXmlElement* pElement = pStyleElement->FirstChildElement("border");
			while (pElement)
			{
				StyleBorder border;
				border.XML(*pElement);
				Cyclostyle.m_setBorder.push_back(border);
				pElement = pElement->NextSiblingElement("border");
			}
		}
		{
			TiXmlElement* pElement = pStyleElement->FirstChildElement("background-color");
			while (pElement)
			{
				StyleBackgroundColor backgroundColor;
				backgroundColor.XML(*pElement);
				Cyclostyle.m_setBackgroundColor.push_back(backgroundColor);
				pElement = pElement->NextSiblingElement("background-color");
			}
		}

		// font Info
		{
			TiXmlElement *pFontElement = pStyleElement->FirstChildElement("font");
			while (pFontElement)
			{
				CUITextCyclostyle FontStyle;
				FontStyle.XML(*pFontElement);
				Cyclostyle.m_FontStyle.push_back(FontStyle);
				pFontElement = pFontElement->NextSiblingElement("font");
			}
		}
		{
			if (pStyleElement->Attribute("name"))
			{
				std::vector<std::string> setTokenizer;
				Tokenizer(pStyleElement->Attribute("name"),setTokenizer);
				for (size_t i=0; i<setTokenizer.size(); ++i)
				{
					m_CyclostyleList[setTokenizer[i]].add(Cyclostyle);
				}
			}
		}

		// 查找下一个
		pStyleElement = pStyleElement->NextSiblingElement("element");
	}
	return true;
}

const CUICyclostyle& CUIStyleMgr::GetCyclostyle(const std::string& strName)
{
	if (m_CyclostyleList.find(strName)!=m_CyclostyleList.end())
	{
		return m_CyclostyleList[strName];
	}
	static CUICyclostyle s_Cyclostyle;
	return s_Cyclostyle;
}
