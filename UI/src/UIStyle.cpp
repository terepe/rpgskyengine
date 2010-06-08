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

void CUIStyle::Blend(const CRect<float>& rc, UINT iState, float fElapsedTime)
{
	m_nVisible = interpolate(1.0f - powf(0.8, 30 * fElapsedTime), m_nVisible, CONTROL_STATE_HIDDEN==iState?0:255);
	const CUICyclostyle& style =  GetCyclostyle();
	style.blend(rc,iState,fElapsedTime,m_mapStyleDrawData);
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
	GetCyclostyle().draw(wstrText,m_mapStyleDrawData);
}

void CUIStyle::draw(const CRect<int>& rc, const std::wstring& wstrText, CONTROL_STATE state, float fElapsedTime)
{
	Blend(rc.getRECT(), state, fElapsedTime);
	GetCyclostyle().draw(wstrText,m_mapStyleDrawData);
}

void CUICyclostyle::blend(const CRect<float>& rc, UINT iState, float fElapsedTime, std::map<int,StyleDrawData>& mapStyleDrawData)const
{
	int nStyleDrawDataID = 0;
	for (size_t i=0; i<m_setDefault.size(); ++i)
	{
		m_setDefault[i].blend(mapStyleDrawData[nStyleDrawDataID], iState, fElapsedTime);
		nStyleDrawDataID++;
	}
	for (size_t i=0; i<m_SpriteStyle.size(); ++i)
	{
		m_SpriteStyle[i].blend(mapStyleDrawData[nStyleDrawDataID], iState, fElapsedTime);
		nStyleDrawDataID++;
	}
	for (size_t i=0; i<m_setSquare.size(); ++i)
	{
		m_setSquare[i].blend(mapStyleDrawData[nStyleDrawDataID], iState, fElapsedTime);
		nStyleDrawDataID++;
	}
	for (size_t i=0; i<m_setBorder.size(); ++i)
	{
		m_setBorder[i].blend(mapStyleDrawData[nStyleDrawDataID], iState, fElapsedTime);
		nStyleDrawDataID++;
	}
	for (size_t i=0; i<m_FontStyle.size(); ++i)
	{
		m_FontStyle[i].blend(mapStyleDrawData[nStyleDrawDataID], iState, fElapsedTime);
		nStyleDrawDataID++;
	}
	for (size_t i=0; i<m_UBBStyle.size(); ++i)
	{
		m_UBBStyle[i].blend(mapStyleDrawData[nStyleDrawDataID], iState, fElapsedTime);
		nStyleDrawDataID++;
	}

	for(std::map<int,StyleDrawData>::iterator it=mapStyleDrawData.begin();it!=mapStyleDrawData.end();++it)
	{
		it->second.updateRect(rc);
	}
}

void CUICyclostyle::draw(const std::wstring& wstrText, std::map<int,StyleDrawData>& mapStyleDrawData)const
{
	int nStyleDrawDataID = 0;
	GetRenderSystem().SetDepthBufferFunc(false,false);
	for (size_t i=0; i<m_setDefault.size(); ++i)
	{
		nStyleDrawDataID++;
	}
	for (size_t i=0; i<m_SpriteStyle.size(); ++i)
	{
		m_SpriteStyle[i].draw(mapStyleDrawData[nStyleDrawDataID].rc,mapStyleDrawData[nStyleDrawDataID].color.getColor());
		nStyleDrawDataID++;
	}
	GetRenderSystem().SetTextureColorOP(0,TBOP_SOURCE2);
	GetRenderSystem().SetTextureAlphaOP(0,TBOP_SOURCE2);
	for (size_t i=0; i<m_setSquare.size(); ++i)
	{
		Color32 color = mapStyleDrawData[nStyleDrawDataID].color.getColor();
		if(color.a!=0)
		{
			GetGraphics().FillRect(mapStyleDrawData[nStyleDrawDataID].rc, color);
		}
		nStyleDrawDataID++;
	}
	for (size_t i=0; i<m_setBorder.size(); ++i)
	{
		Color32 color = mapStyleDrawData[nStyleDrawDataID].color.getColor();
		if(color.a!=0)
		{
			GetGraphics().DrawRect(mapStyleDrawData[nStyleDrawDataID].rc, color);
		}
		nStyleDrawDataID++;
	}
	GetRenderSystem().SetTextureColorOP(0,TBOP_MODULATE);
	GetRenderSystem().SetTextureAlphaOP(0,TBOP_MODULATE);
	for (size_t i=0; i<m_FontStyle.size(); ++i)
	{
		m_FontStyle[0].draw(wstrText,mapStyleDrawData[nStyleDrawDataID].rc,mapStyleDrawData[nStyleDrawDataID].color.getColor());
		nStyleDrawDataID++;
	}
	for (size_t i=0; i<m_UBBStyle.size(); ++i)
	{
		m_UBBStyle[0].draw(wstrText,mapStyleDrawData[nStyleDrawDataID].rc,mapStyleDrawData[nStyleDrawDataID].color.getColor());
		nStyleDrawDataID++;
	}
}

bool CUIStyle::isVisible()
{
	return m_nVisible!=0;
}

CRect<float>& CUIStyle::getTextRect()
{
	return m_mapStyleDrawData[m_mapStyleDrawData.size()-1].rc;
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
	m_UBBStyle.insert(m_UBBStyle.end(), cyc.m_UBBStyle.begin(), cyc.m_UBBStyle.end()); 
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
				for (int i = 0; i < CONTROL_STATE_MAX; ++i)
				{
					setColor[i] = c;
				}
				setColor[CONTROL_STATE_HIDDEN].w = 0.0f;
			}
			for (int i = 0; i < CONTROL_STATE_MAX; ++i)
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

void CUIUBBCyclostyle::draw(const std::wstring& wstrText,const CRect<float>& rc,const Color32& color)const
{
	if(color.a==0)
	{
		return;
	}
	GetTextRender().DrawUBB(wstrText,rc.getRECT());
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
			TiXmlElement *pFontElement = pStyleElement->FirstChildElement("ubb");
			while (pFontElement)
			{
				CUIUBBCyclostyle UBBStyle;
				UBBStyle.XMLParse(*pFontElement);
				Cyclostyle.m_UBBStyle.push_back(UBBStyle);
				pFontElement = pFontElement->NextSiblingElement("ubb");
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
