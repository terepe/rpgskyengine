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

CUIStyle::CUIStyle():
m_nVisible(0)
{}


template<typename T>
inline void updateStyle(const std::vector<T>& setSrc, std::map<int,StyleDrawData>& setDest,const CRect<float>& rc, UINT iState, float fElapsedTime)
{
	for (uint32 i=0; i<setSrc.size(); i++)
	{
		setSrc[i].blend(setDest[i], iState, fElapsedTime);
		setDest[i].updateRect(rc);
	}
}

void CUIStyle::Blend(const CRect<float>& rc, UINT iState, float fElapsedTime)
{
	m_nVisible = interpolate(1.0f - powf(0.8, 30 * fElapsedTime), m_nVisible, CONTROL_STATE_HIDDEN==iState?0:255);
	const CUICyclostyle& style =  GetCyclostyle();
	updateStyle(style.m_setDefault,m_mapDefault,rc,iState,fElapsedTime);
	updateStyle(style.m_SpriteStyle,m_mapSprite,rc,iState,fElapsedTime);
	updateStyle(style.m_setBorder,m_mapBorder,rc,iState,fElapsedTime);
	updateStyle(style.m_setSquare,m_mapSquare,rc,iState,fElapsedTime);
	updateStyle(style.m_FontStyle,m_mapFont,rc,iState,fElapsedTime);
}

void CUIStyle::SetStyle(const std::string& strName)
{
	m_strName = strName;
	CRect<float> rc;
	Blend(rc,CONTROL_STATE_HIDDEN,100);
}

const CUICyclostyle& CUIStyle::GetCyclostyle()
{
	return GetStyleMgr().GetCyclostyle(m_strName);
}
void CUIStyle::draw(const CRect<float>& rc, const std::wstring& wstrText, CONTROL_STATE state, float fElapsedTime)
{
	Blend(rc, state, fElapsedTime);
	Draw(wstrText);
}

void CUIStyle::draw(const CRect<int>& rc, const std::wstring& wstrText, CONTROL_STATE state, float fElapsedTime)
{
	Blend(rc.getRECT(), state, fElapsedTime);
	Draw(wstrText);
}

void CUIStyle::Draw(const std::wstring& wstrText)
{
	GetRenderSystem().SetDepthBufferFunc(false,false);
	for (size_t i=0; i<m_mapSprite.size(); i++)
	{
		GetCyclostyle().m_SpriteStyle[i].draw(m_mapSprite[i].rc,m_mapSprite[i].color.getColor());
	}
	GetRenderSystem().SetTextureColorOP(0,TBOP_SOURCE2);
	GetRenderSystem().SetTextureAlphaOP(0,TBOP_SOURCE2);
	for (size_t i=0; i<m_mapSquare.size(); ++i)
	{
		Color32 color = m_mapSquare[i].color.getColor();
		if(color.a!=0)
		{
			GetGraphics().FillRect(m_mapSquare[i].rc, color);
		}
	}
	for (size_t i=0; i<m_mapBorder.size(); ++i)
	{
		Color32 color = m_mapBorder[i].color.getColor();
		if(color.a!=0)
		{
			GetGraphics().DrawRect(m_mapBorder[i].rc, color);
		}
	}
	GetRenderSystem().SetTextureColorOP(0,TBOP_MODULATE);
	GetRenderSystem().SetTextureAlphaOP(0,TBOP_MODULATE);
	if (m_mapFont.size()>0)
	{
		GetCyclostyle().m_FontStyle[0].draw(wstrText,m_mapFont[0].rc,m_mapFont[0].color.getColor());
	}
}

bool CUIStyle::isVisible()
{
	return m_nVisible!=0;
}

void CUICyclostyle::Refresh()
{
	//m_SpriteColor.Current = m_TextureColor.States[ CONTROL_STATE_HIDDEN ];
	//m_FontColor.Current = m_FontColor.States[ CONTROL_STATE_HIDDEN ];
}

void CUICyclostyle::add(const CUICyclostyle& cyc)
{
	m_setDefault.insert(m_setDefault.end(), cyc.m_setDefault.begin(), cyc.m_setDefault.end()); 
	m_SpriteStyle.insert(m_SpriteStyle.end(), cyc.m_SpriteStyle.begin(), cyc.m_SpriteStyle.end());   
	m_setBorder.insert(m_setBorder.end(), cyc.m_setBorder.begin(), cyc.m_setBorder.end()); 
	m_setSquare.insert(m_setSquare.end(), cyc.m_setSquare.begin(), cyc.m_setSquare.end()); 
	m_FontStyle.insert(m_FontStyle.end(), cyc.m_FontStyle.begin(), cyc.m_FontStyle.end()); 
}

CUIStyleMgr::CUIStyleMgr()
{
}

void StyleStateBlend::XMLParse(TiXmlElement& element)
{
	{
		for (size_t i=0;i< CONTROL_STATE_MAX;++i)
		{
			setColor[i].set(0.0f,0.0f,0.0f,0.0f);
		}
		TiXmlElement *pElement = element.FirstChildElement("color");
		if (pElement)
		{
			const char* pszText = pElement->GetText();
			if(pszText)
			{
				Color32 c;
				c = pszText;
				for (int i = 0; i < CONTROL_STATE_MAX; i++)
				{
					setColor[i] = c;
				}
				setColor[CONTROL_STATE_HIDDEN].w = 0.0f;
			}
			for (int i = 0; i < CONTROL_STATE_MAX; i++)
			{
				pszText =  pElement->Attribute(szControlState[i]);
				if (pszText)
				{
					Color32 c;
					c = pszText;
					setColor[i] = c;
				}
			}
		}
	}
	//
	{
		for (size_t i=0;i< CONTROL_STATE_MAX;++i)
		{
			setBlendRate[i]=0.8f;
		}
		TiXmlElement *pElement = element.FirstChildElement("blend");
		if (pElement)
		{
			const char* pszText = pElement->GetText();
			if(pszText)
			{
				for (size_t i=0;i< CONTROL_STATE_MAX;++i)
				{
					setBlendRate[i] = (float)atof(pszText);
				}
			}
			for (size_t i=0;i< CONTROL_STATE_MAX;++i)
			{
				pszText =  pElement->Attribute(szControlState[i]);
				if (pszText)
				{
					setBlendRate[i] = (float)atof(pszText);
				}
			}
		}
	}
	//
	{
		for (size_t i=0;i< CONTROL_STATE_MAX;++i)
		{
			setOffset[i].set(0.0f,0.0f,0.0f,0.0f);
		}
		TiXmlElement *pElement = element.FirstChildElement("offset");
		if (pElement)
		{
			const char* pszText = pElement->GetText();
			if(pszText)
			{
				CRect<float> rc;
				StrToRect(pszText, rc);
				for (size_t i=0;i< CONTROL_STATE_MAX;++i)
				{
					setOffset[i] = rc;
				}
			}
			for (size_t i=0;i< CONTROL_STATE_MAX;++i)
			{
				pszText =  pElement->Attribute(szControlState[i]);
				if (pszText)
				{
					StrToRect(pszText, setOffset[i]);
				}
			}
		}
	}
	// 
	{
		for (size_t i=0;i< CONTROL_STATE_MAX;++i)
		{
			setScale[i].set(0.0f,0.0f,1.0f,1.0f);
		}
		TiXmlElement *pElement = element.FirstChildElement("scale");
		if (pElement)
		{
			const char* pszText = pElement->GetText();
			if(pszText)
			{
				CRect<float> rc;
				StrToRect(pszText, rc);
				for (size_t i=0;i< CONTROL_STATE_MAX;++i)
				{
					setScale[i] = rc;
				}
			}
			for (size_t i=0;i< CONTROL_STATE_MAX;++i)
			{
				pszText =  pElement->Attribute(szControlState[i]);
				if (pszText)
				{
					StrToRect(pszText, setScale[i]);
				}
			}
		}
	}

}

void CUISpriteCyclostyle::XMLParse(TiXmlElement& element)
{
	//
	StyleStateBlend::XMLParse(element);
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

void CUITextCyclostyle::XMLParse(TiXmlElement& element)
{
	//
	StyleStateBlend::XMLParse(element);
	uFormat = 0;
	if (element.Attribute("format"))
	{
		uFormat = StrToTextFormat(element.Attribute("format"));
	}
}

void CUITextCyclostyle::draw(const std::wstring& wstrText,const CRect<float>& rc,const Color32& color)const
{
	if(color.a==0)
	{
		return;
	}
	GetTextRender().drawText(wstrText,-1,rc,uFormat,color);
}

void CUISpriteCyclostyle::draw(const CRect<float>& rc,const Color32& color)const
{
	if(color.a==0)
	{
		return;
	}
	if (m_bDecolor)
	{
		GetRenderSystem().SetTextureStageStateDecolor();
	}
	switch(m_nSpriteLayoutType)
	{
	case SPRITE_LAYOUT_WRAP:
		UIGraph::DrawSprite(rc,rc,m_nTexture,color);
		break;
	case SPRITE_LAYOUT_SIMPLE:
		UIGraph::DrawSprite(m_rcBorder,rc,m_nTexture,color);
		break;
	case SPRITE_LAYOUT_3X3GRID:
		UIGraph::DrawSprite3x3Grid(m_rcBorder,m_rcCenter,rc, m_nTexture,color);
		break;
	case SPRITE_LAYOUT_3X3GRID_WRAP:
		UIGraph::DrawSprite3x3GridWrap(m_rcBorder,m_rcCenter,rc,m_nTexture,color);
		break;
	case SPRITE_LAYOUT_DISPERSE_3X3GRID:
		break;
	default:
		break;
	}
	if (m_bDecolor)
	{
		GetRenderSystem().SetupRenderState();
	}
}

void StyleBorder::draw(const CRect<float>& rc,const Color32& color)const
{
	if(color.a==0)
	{
		return;
	}
	GetGraphics().DrawRect(rc, color);
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
		//
		{
			TiXmlElement* pElement = pStyleElement->FirstChildElement("default");
			while (pElement)
			{
				StyleBorder border;
				border.XMLParse(*pElement);
				Cyclostyle.m_setDefault.push_back(border);
				pElement = pElement->NextSiblingElement("default");
			}
		}
		// Sprite Info
		{
			TiXmlElement* pSpriteElement = pStyleElement->FirstChildElement("texture");
			while (pSpriteElement)
			{
				CUISpriteCyclostyle SpriteCyclostyle;
				SpriteCyclostyle.XMLParse(*pSpriteElement);
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
				border.XMLParse(*pElement);
				Cyclostyle.m_setBorder.push_back(border);
				pElement = pElement->NextSiblingElement("border");
			}
		}
		{
			TiXmlElement* pElement = pStyleElement->FirstChildElement("square");
			while (pElement)
			{
				StyleSquare backgroundColor;
				backgroundColor.XMLParse(*pElement);
				Cyclostyle.m_setSquare.push_back(backgroundColor);
				pElement = pElement->NextSiblingElement("square");
			}
		}

		// font Info
		{
			TiXmlElement *pFontElement = pStyleElement->FirstChildElement("font");
			while (pFontElement)
			{
				CUITextCyclostyle FontStyle;
				FontStyle.XMLParse(*pFontElement);
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
