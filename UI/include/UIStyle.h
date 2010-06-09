#pragma once
#include "Common.h"
#include "Manager.h"
#include "Color.h"
#include <vector>
#include <map>
#include <Windows.h>
#include "Vec4D.h"
#include "interpolation.h"
#include "Rect.h"

class TiXmlElement;

template<typename T>
inline void StrToRect(const char* str, T& rect)
{
	sscanf(str, "%d,%d,%d,%d", &rect.left, &rect.top, &rect.right, &rect.bottom);
}

template<>
inline void StrToRect<CRect<float>>(const char* str, CRect<float>& rect)
{
	sscanf(str, "%f,%f,%f,%f", &rect.left, &rect.top, &rect.right, &rect.bottom);
}

void StrToXY(const char* str, int& x, int& y);

enum CONTROL_STATE
{
	CONTROL_STATE_NORMAL,
	CONTROL_STATE_DISABLED,
	CONTROL_STATE_HIDDEN,
	CONTROL_STATE_FOCUS,
	CONTROL_STATE_MOUSEOVER,
	CONTROL_STATE_PRESSED,
	CONTROL_STATE_MAX,
};

// 纹理布局的三种类型
enum SPRITE_LAYOUT_TYPE
{
	SPRITE_LAYOUT_WRAP,
	SPRITE_LAYOUT_SIMPLE,
	SPRITE_LAYOUT_3X3GRID,
	SPRITE_LAYOUT_3X3GRID_WRAP,
	SPRITE_LAYOUT_DISPERSE_3X3GRID,
};

struct StyleDrawData
{
	StyleDrawData()
	{
		memset(this,0,sizeof(*this));
	}

	Vec4D	color;
	CRect<float> scale;
	CRect<float> offset;
	CRect<float> rc;
	void updateRect(CRect<float> rect)
	{
		rc.left		= rect.left+rect.getWidth()*scale.left;
		rc.right	= rect.left+rect.getWidth()*scale.right;
		rc.top		= rect.top+rect.getHeight()*scale.top;
		rc.bottom	= rect.top+rect.getHeight()*scale.bottom;
		rc+= offset;
	}
};

struct StyleElement
{
	StyleElement()
	{
		memset(this,0,sizeof(*this));
	}

	void blend(StyleDrawData& sdd,UINT iState,float fElapsedTime)const
	{
		sdd.color		= interpolate(1.0f - powf(setBlendRate[iState], 30 * fElapsedTime), sdd.color,	setColor[iState]);
		sdd.offset		= interpolate(1.0f - powf(setBlendRate[iState], 30 * fElapsedTime), sdd.offset, setOffset[iState]);
		sdd.scale		= interpolate(1.0f - powf(setBlendRate[iState], 30 * fElapsedTime), sdd.scale,	setScale[iState]);
	}
	void XMLParse(const TiXmlElement& element);
	virtual void draw(const std::wstring& wstrText,const CRect<float>& rc,const Color32& color)const{}

	float			setBlendRate[CONTROL_STATE_MAX];
	Vec4D			setColor[CONTROL_STATE_MAX];
	CRect<float>	setOffset[CONTROL_STATE_MAX];
	CRect<float>	setScale[CONTROL_STATE_MAX];
};

class StyleSprite: public StyleElement
{
public:
	virtual void XMLParse(const TiXmlElement& element);
	void		draw(const std::wstring& wstrText,const CRect<float>& rc,const Color32& color)const;
	int			m_nTexture;
	bool		m_bDecolor;
	int			m_nSpriteLayoutType;
	RECT		m_rcBorder;
	RECT		m_rcCenter;
};

struct  StyleBorder: public StyleElement
{
	void draw(const std::wstring& wstrText,const CRect<float>& rc,const Color32& color)const;
};

struct  StyleSquare: public StyleElement
{
	void draw(const std::wstring& wstrText,const CRect<float>& rc,const Color32& color)const;
};

class StyleText: public StyleElement
{
public:
	virtual void XMLParse(const TiXmlElement& element);
	void draw(const std::wstring& wstrText,const CRect<float>& rc,const Color32& color)const;
	uint32 uFormat;
};

class StyleUBB: public StyleElement
{
public:
	void draw(const std::wstring& wstrText,const CRect<float>& rc,const Color32& color)const;
};

class CUIStyleData
{
public:
	CUIStyleData(){}
	~CUIStyleData();
	void clear();
	void Refresh();
	void add(const std::vector<StyleElement*>& setStyleElement);

	void blend(const CRect<float>& rc, UINT iState, float fElapsedTime, std::map<int,StyleDrawData>& mapStyleDrawData)const;
	void draw(const std::wstring& wstrText, std::map<int,StyleDrawData>& mapStyleDrawData)const;

	std::vector<StyleElement*> m_setStyleElement;
};

class CUIStyle
{
public:
	CUIStyle();
	~CUIStyle(){};
	void Blend(const CRect<float>& rc, UINT iState, float fElapsedTime);
	void SetStyle(const std::string& strName);
	const CUIStyleData& getStyleData();
	void draw(const CRect<float>& rc, const std::wstring& wstrText, CONTROL_STATE state, float fElapsedTime);
	void draw(const CRect<int>& rc, const std::wstring& wstrText, CONTROL_STATE state, float fElapsedTime);
	void Draw(const std::wstring& wstrText);
	bool isVisible();
	CRect<float>& getTextRect();

	int m_nVisible;
	std::string	m_strName;
	std::map<int,StyleDrawData> m_mapStyleDrawData;
};

class CUIStyleMgr
{
	std::map<std::string, CUIStyleData> m_mapStyleData;
	std::string m_strFilename;
public:
	CUIStyleMgr();
	bool Create(const std::string& strFilename);
	const std::string& getFilename(){return m_strFilename;}
	const CUIStyleData& getStyleData(const std::string& strName);
};

CUIStyleMgr& GetStyleMgr();