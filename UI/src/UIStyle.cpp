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

#include "Timer.h"
void StyleDrawData::updateRect(CRect<float> rect)
{
	rc.left		= rect.left+rect.getWidth()*scale.left;
	rc.right	= rect.left+rect.getWidth()*scale.right;
	rc.top		= rect.top+rect.getHeight()*scale.top;
	rc.bottom	= rect.top+rect.getHeight()*scale.bottom;
	rc+= offset;
}

CUIStyle::CUIStyle():
m_nVisible(0)
{}

void CUIStyle::Blend(const CRect<float>& rc, UINT iState, float fElapsedTime)
{
	float fRate = 1.0f - powf(0.8f, 30 * fElapsedTime);

	m_nVisible = interpolate(fRate, m_nVisible, CONTROL_STATE_HIDDEN==iState?0:255);
	const CUIStyleData& styleData = getStyleData();

	fRate = 1.0f - powf(styleData.setBlendRate[iState], 30 * fElapsedTime);
	vRotate		= interpolate(fRate, vRotate, styleData.setRotate[iState]);

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

void CUIStyle::draw(const Matrix& mTransform, const CRect<float>& rc, const std::wstring& wstrText, CONTROL_STATE state, float fElapsedTime)
{
	CRect<float> rcNew;
	rcNew.left =0;// -0.5f*rc.getWidth();
	rcNew.right = rc.getWidth();//0.5f*rc.getWidth();
	rcNew.top =0;// -0.5f*rc.getHeight();
	rcNew.bottom = rc.getHeight();//0.5f*rc.getHeight();
	Blend(rcNew, state, fElapsedTime);


	CRenderSystem& R = GetRenderSystem();
	//R.SetDepthBufferFunc(false,false);
	R.SetDepthBufferFunc(true,true);
	//////////////////////////////////////////////////////////////////////////
	CRect<int> rcViewport;
	R.getViewport(rcViewport);
	Matrix mView;
	//mView.MatrixLookAtLH(Vec3D(0,0,0),Vec3D(0,0,1.0f),Vec3D(0,1.0f,0));
	mView.unit();
	R.setViewMatrix(mView);
	Matrix mProjection;
	mProjection.MatrixPerspectiveFovLH(PI/2,(float)rcViewport.right/(float)rcViewport.bottom,0.1f,100);
	R.setProjectionMatrix(mProjection);
	//////////////////////////////////////////////////////////////////////////
	Matrix mRotate;
	mRotate.rotate(vRotate);
	mWorld = mTransform*Matrix::newTranslation(Vec3D(rc.left,rc.top,0))*mRotate;
	GetRenderSystem().setWorldMatrix(mWorld);
	//////////////////////////////////////////////////////////////////////////

	getStyleData().draw(wstrText,m_mapStyleDrawData);
}

void CUIStyle::draw(const Matrix& mTransform, const CRect<int>& rc, const std::wstring& wstrText, CONTROL_STATE state, float fElapsedTime)
{
	CRect<float> rcFloat = rc.getRECT();;
	draw(mTransform, rcFloat, wstrText, state, fElapsedTime);
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

CUIStyleData::CUIStyleData():m_pFontStyleElement(NULL)
{
	for (size_t i=0;i< CONTROL_STATE_MAX;++i)
	{
		setBlendRate[i]=0.8f;
	}
	for (size_t i=0;i< CONTROL_STATE_MAX;++i)
	{
		setRotate[i].set(0.0f,0.0f,0.0f);
	}
}

CUIStyleData::~CUIStyleData()
{
	clear();
}

void CUIStyleData::clear()
{
	m_pFontStyleElement = NULL;
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

void CUIStyleData::XMLParse(const TiXmlElement& xml)
{
	const TiXmlElement* pElement = xml.FirstChildElement();
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
			m_pFontStyleElement = pNewStyleElement;
		}
		else if (pElement->ValueStr() == "ubb")
		{
			pNewStyleElement = new StyleUBB;
			m_pFontStyleElement = pNewStyleElement;
		}
		else
		{
			pNewStyleElement = new StyleBorder;
		}
		pNewStyleElement->XMLParse(*pElement);
		m_setStyleElement.push_back(pNewStyleElement);
		pElement = pElement->NextSiblingElement();
	}
	//
	{
		const TiXmlElement *pElement = xml.FirstChildElement("blend");
		if (pElement)
		{
			const char* pszText = pElement->GetText();
			if(pszText)
			{
				float fBlend = (float)atof(pszText);
				for (size_t i=0;i< CONTROL_STATE_MAX;++i)
				{
					setBlendRate[i] = fBlend;
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
	//{
	//	for (size_t i=0;i< CONTROL_STATE_MAX;++i)
	//	{
	//		setOffset[i].set(0.0f,0.0f,0.0f,0.0f);
	//	}
	//	const TiXmlElement *pElement = xml.FirstChildElement("offset");
	//	if (pElement)
	//	{
	//		const char* pszText = pElement->GetText();
	//		if(pszText)
	//		{
	//			CRect<float> rc;
	//			rc.strToRect(pszText);
	//			for (size_t i=0;i< CONTROL_STATE_MAX;++i)
	//			{
	//				setOffset[i] = rc;
	//			}
	//		}
	//		for (size_t i=0;i< CONTROL_STATE_MAX;++i)
	//		{
	//			pszText =  pElement->Attribute(szControlState[i]);
	//			if (pszText)
	//			{
	//				setOffset[i].strToRect(pszText);
	//			}
	//		}
	//	}
	//}
	// 
	{
		const TiXmlElement *pElement = xml.FirstChildElement("rotate");
		if (pElement)
		{
			const char* pszText = pElement->GetText();
			if(pszText)
			{
				Vec3D v;
				v.setByString(pszText);
				v*=PI/180.0f;
				for (size_t i=0;i< CONTROL_STATE_MAX;++i)
				{
					setRotate[i] = v;
				}
			}
			for (size_t i=0;i< CONTROL_STATE_MAX;++i)
			{
				pszText =  pElement->Attribute(szControlState[i]);
				if (pszText)
				{
					setRotate[i].setByString(pszText);
					setRotate[i]*=PI/180.0f;
				}
			}
		}
	}
}

const StyleElement* CUIStyleData::getFontStyleElement()const
{
	return m_pFontStyleElement;
}

CUIStyleMgr::CUIStyleMgr()
{
}

void StyleElement::blend(StyleDrawData& sdd,UINT iState,float fElapsedTime)const
{
	float fRate = 1.0f - powf(setBlendRate[iState], 30 * fElapsedTime);
	sdd.color		= interpolate(fRate, sdd.color,		setColor[iState]);
	sdd.offset		= interpolate(fRate, sdd.offset,	setOffset[iState]);
	sdd.scale		= interpolate(fRate, sdd.scale,		setScale[iState]);
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
				float fBlend = (float)atof(pszText);
				for (size_t i=0;i< CONTROL_STATE_MAX;++i)
				{
					setBlendRate[i] = fBlend;
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
				rc.setByString(pszText);
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
					setOffset[i].setByString(pszText);
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
				rc.setByString(pszText);
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
					setScale[i].setByString(pszText);
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
		m_rcBorder.setByString(strRect);
		m_rcBorder.right	+= m_rcBorder.left;
		m_rcBorder.bottom	+= m_rcBorder.top;
		if (element.Attribute("center_rect"))
		{
			m_nSpriteLayoutType = SPRITE_LAYOUT_3X3GRID;
			const char* strCenterRect = element.Attribute("center_rect");
			m_rcCenter.setByString(strCenterRect);
			m_rcCenter.left		+= m_rcBorder.left;
			m_rcCenter.top		+= m_rcBorder.top;
			m_rcCenter.right	+= m_rcCenter.left;
			m_rcCenter.bottom	+= m_rcCenter.top;
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

void StyleElement::draw(const std::wstring& wstrText,const CRect<float>& rc,const Color32& color)const
{
}

void StyleText::draw(const std::wstring& wstrText,const CRect<float>& rc,const Color32& color)const
{
	if(color.a==0)
	{
		return;
	}
	GetTextRender().drawText(wstrText,-1,rc,uFormat,color);
}

void StyleUBB::draw(const std::wstring& wstrText,const CRect<float>& rc,const Color32& color)const
{
	if(color.a==0)
	{
		return;
	}
	GetRenderSystem().SetTextureFactor(color);
	GetRenderSystem().SetTextureColorOP(1,TBOP_MODULATE,TBS_CURRENT,TBS_TFACTOR);
	GetRenderSystem().SetTextureAlphaOP(1,TBOP_MODULATE,TBS_CURRENT,TBS_TFACTOR);
	GetTextRender().DrawUBB(wstrText,rc.getRECT());
	GetRenderSystem().SetTextureColorOP(1,TBOP_DISABLE);
	GetRenderSystem().SetTextureAlphaOP(1,TBOP_DISABLE);
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
		UIGraph::DrawSprite3x3Grid(m_rcBorder,m_rcCenter,rc,m_nTexture,color);
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
	GetRenderSystem().SetTextureColorOP(0,TBOP_MODULATE);
	GetRenderSystem().SetTextureAlphaOP(0,TBOP_MODULATE);
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
	GetRenderSystem().SetTextureColorOP(0,TBOP_MODULATE);
	GetRenderSystem().SetTextureAlphaOP(0,TBOP_MODULATE);
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
	const TiXmlElement *pRootElement = myDocument.RootElement();
	if (pRootElement==NULL)
	{
		return false;
	}
	//获得第一个Style节点。
	const TiXmlElement *pStyleElement = pRootElement->FirstChildElement("element");
	while (pStyleElement)
	{
		// Style name
		if (pStyleElement->Attribute("name"))
		{
			std::vector<std::string> setTokenizer;
			Tokenizer(pStyleElement->Attribute("name"),setTokenizer);
			for (size_t i=0; i<setTokenizer.size(); ++i)
			{
				CUIStyleData& styleData = m_mapStyleData[setTokenizer[i]];//.add(StyleData);
				styleData.XMLParse(*pStyleElement);
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