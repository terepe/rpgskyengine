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
	const CUIStyleData& styleData = getStyleData();
	styleData.blend(rc,iState,fElapsedTime,m_mapStyleDrawData);
}

void CUIStyle::SetStyle(const std::string& strName)
{
	m_strName = strName;
	CRect<float> rc;
	Blend(rc,CONTROL_STATE_HIDDEN,100);
}

const CUIStyleData& CUIStyle::getStyleData()
{
	return GetStyleMgr().getStyleData(m_strName);
}
void CUIStyle::draw(const CRect<float>& rc, const std::wstring& wstrText, CONTROL_STATE state, float fElapsedTime)
{
	Blend(rc, state, fElapsedTime);
	getStyleData().draw(wstrText,m_mapStyleDrawData);
}

void CUIStyle::draw(const CRect<int>& rc, const std::wstring& wstrText, CONTROL_STATE state, float fElapsedTime)
{
	Blend(rc.getRECT(), state, fElapsedTime);
	getStyleData().draw(wstrText,m_mapStyleDrawData);
}

void CUIStyleData::blend(const CRect<float>& rc, UINT iState, float fElapsedTime, std::map<int,StyleDrawData>& mapStyleDrawData)const
{
	for (size_t i=0; i<m_setStyleElement.size(); ++i)
	{
		m_setStyleElement[i]->blend(mapStyleDrawData[i], iState, fElapsedTime);
	}
	for(std::map<int,StyleDrawData>::iterator it=mapStyleDrawData.begin();it!=mapStyleDrawData.end();++it)
	{
		it->second.updateRect(rc);
	}
}

void CUIStyleData::draw(const std::wstring& wstrText, std::map<int,StyleDrawData>& mapStyleDrawData)const
{
	GetRenderSystem().SetDepthBufferFunc(false,false);

	for (size_t i=0; i<m_setStyleElement.size(); ++i)
	{
		m_setStyleElement[i]->draw(wstrText,mapStyleDrawData[i].rc,mapStyleDrawData[i].color.getColor());
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

CUIStyleData::~CUIStyleData()
{
	clear();
}

void CUIStyleData::clear()
{
	for (size_t i=0; i<m_setStyleElement.size(); ++i)
	{
		delete m_setStyleElement[i];
		m_setStyleElement[i] = NULL;
	}
	m_setStyleElement.clear();
}

void CUIStyleData::Refresh()
{
	//m_SpriteColor.Current = m_TextureColor.States[ CONTROL_STATE_HIDDEN ];
	//m_FontColor.Current = m_FontColor.States[ CONTROL_STATE_HIDDEN ];
}

void CUIStyleData::add(const std::vector<StyleElement*>& setStyleElement)
{
	m_setStyleElement.insert(m_setStyleElement.end(), setStyleElement.begin(), setStyleElement.end()); 
}

CUIStyleMgr::CUIStyleMgr()
{
}

void StyleElement::XMLParse(const TiXmlElement& element)
{
	{
		for (size_t i=0;i< CONTROL_STATE_MAX;++i)
		{
			setColor[i].set(0.0f,0.0f,0.0f,0.0f);
		}
		const TiXmlElement *pElement = element.FirstChildElement("color");
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
		const TiXmlElement *pElement = element.FirstChildElement("blend");
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
		const TiXmlElement *pElement = element.FirstChildElement("offset");
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
		const TiXmlElement *pElement = element.FirstChildElement("scale");
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

void StyleSprite::XMLParse(const TiXmlElement& element)
{
	//
	StyleElement::XMLParse(element);
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

void StyleText::XMLParse(const TiXmlElement& element)
{
	//
	StyleElement::XMLParse(element);
	uFormat = 0;
	if (element.Attribute("format"))
	{
		uFormat = StrToTextFormat(element.Attribute("format"));
	}
}

void StyleText::draw(const std::wstring& wstrText,const CRect<float>& rc,const Color32& color)const
{
	if(color.a==0)
	{
		return;
	}
	GetRenderSystem().SetTextureColorOP(0,TBOP_MODULATE);
	GetRenderSystem().SetTextureAlphaOP(0,TBOP_MODULATE);
	GetTextRender().drawText(wstrText,-1,rc,uFormat,color);
}

void StyleUBB::draw(const std::wstring& wstrText,const CRect<float>& rc,const Color32& color)const
{
	if(color.a==0)
	{
		return;
	}
	GetRenderSystem().SetTextureColorOP(0,TBOP_MODULATE);
	GetRenderSystem().SetTextureAlphaOP(0,TBOP_MODULATE);
	GetTextRender().DrawUBB(wstrText,rc.getRECT());
}

void StyleSprite::draw(const std::wstring& wstrText,const CRect<float>& rc,const Color32& color)const
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

void StyleBorder::draw(const std::wstring& wstrText,const CRect<float>& rc,const Color32& color)const
{
	if(color.a==0)
	{
		return;
	}
	GetRenderSystem().SetTextureColorOP(0,TBOP_SOURCE2);
	GetRenderSystem().SetTextureAlphaOP(0,TBOP_SOURCE2);
	GetGraphics().DrawRect(rc, color);
}

void StyleSquare::draw(const std::wstring& wstrText,const CRect<float>& rc,const Color32& color)const
{
	if(color.a==0)
	{
		return;
	}
	GetRenderSystem().SetTextureColorOP(0,TBOP_SOURCE2);
	GetRenderSystem().SetTextureAlphaOP(0,TBOP_SOURCE2);
	GetGraphics().FillRect(rc, color);
}

#include "IORead.h"
bool CUIStyleMgr::Create(const std::string& strFilename)
{
	m_strFilename = strFilename;
	m_mapStyleData.clear();
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
		if (pStyleElement->Attribute("name"))
		{
			std::vector<std::string> setTokenizer;
			Tokenizer(pStyleElement->Attribute("name"),setTokenizer);
			for (size_t i=0; i<setTokenizer.size(); ++i)
			{
				CUIStyleData& StyleData = m_mapStyleData[setTokenizer[i]];//.add(StyleData);
				{
					const TiXmlElement* pElement = pStyleElement->FirstChildElement();
					while (pElement)
					{
						StyleElement* pNewStyleElement = NULL;
						if (pElement->ValueStr() == "texture")
						{
							pNewStyleElement = new StyleSprite;
						}
						else if (pElement->ValueStr() == "border")
						{
							pNewStyleElement = new StyleBorder;
						}
						else if (pElement->ValueStr() == "square")
						{
							pNewStyleElement = new StyleSquare;
						}
						else if (pElement->ValueStr() == "font")
						{
							pNewStyleElement = new StyleText;
						}
						else if (pElement->ValueStr() == "ubb")
						{
							pNewStyleElement = new StyleUBB;
						}
						else
						{
							pNewStyleElement = new StyleElement;
						}
						pNewStyleElement->XMLParse(*pElement);
						StyleData.m_setStyleElement.push_back(pNewStyleElement);
						pElement = pElement->NextSiblingElement();
					}
				}
			}
		}
		// 查找下一个
		pStyleElement = pStyleElement->NextSiblingElement("element");
	}
	return true;
}

const CUIStyleData& CUIStyleMgr::getStyleData(const std::string& strName)
{
	if (m_mapStyleData.find(strName)!=m_mapStyleData.end())
	{
		return m_mapStyleData[strName];
	}
	static CUIStyleData s_StyleData;
	return s_StyleData;
}
