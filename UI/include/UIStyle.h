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

// �������ֵ���������
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

	Vec4D	vColor;
	CRect<float> offset;
	CRect<float> rc;
};

struct ControlBlendColor
{
	ControlBlendColor()
	{
		memset(this,0,sizeof(*this));
	}

	void blend(Vec4D& crCurrent, CRect<float>& offset, UINT iState, float fElapsedTime)const
	{
		crCurrent	= interpolate(1.0f - powf(m_BlendRates[iState], 30 * fElapsedTime),crCurrent,ColorOfStates[iState]);
		offset		= interpolate(1.0f - powf(m_BlendRates[iState], 30 * fElapsedTime),offset,setOffset[iState]);
		//return Color32::lerp(1.0f - powf(fRate, 30 * fElapsedTime), crCurrent, States[ iState ]);
	}
	void XMLState(TiXmlElement& element);

	Vec4D	ColorOfStates[CONTROL_STATE_MAX];
	CRect<float>	setOffset[CONTROL_STATE_MAX];
	float	m_BlendRates[CONTROL_STATE_MAX];
};

struct BaseCyclostyle: public ControlBlendColor
{
	BaseCyclostyle()
	{
		uFormat = 0;
		memset(&rcOffset,0,sizeof(rcOffset));
	}
	CRect<float> rcOffset;
	uint32 uFormat;// The format argument to DrawText or Texture
	virtual void XML(TiXmlElement& element);
	virtual void updataRect(CRect<float>& rc)const;
};

class CUISpriteCyclostyle: public BaseCyclostyle
{
public:
	virtual void XML(TiXmlElement& element);
	virtual void updataRect(CRect<float>& rc)const;
	void		draw(const CRect<float>& rc,const Color32& color)const;
	int			m_nTexture;
	bool		m_bDecolor;
	int			m_nSpriteLayoutType;
	RECT		m_rcBorder;
	RECT		m_rcCenter;
};

class CUITextCyclostyle: public BaseCyclostyle
{
public:
	virtual void updataRect(CRect<float>& rc)const;
	void draw(const std::wstring& wstrText,const CRect<float>& rc,const Color32& color)const;
};

struct  StyleBorder: public BaseCyclostyle
{
	void draw(const CRect<float>& rc,const Color32& color)const;
};

struct  StyleSquare: public BaseCyclostyle
{
	void draw(const CRect<float>& rc,const Color32& color)const;
};

class DLL_EXPORT CUICyclostyle
{
public:
	CUICyclostyle(){}
	void Refresh();
	void add(const CUICyclostyle& cyc);
	std::vector<CUISpriteCyclostyle> m_SpriteStyle;
	std::vector<StyleBorder> m_setBorder;
	std::vector<StyleSquare> m_setSquare;
	std::vector<CUITextCyclostyle> m_FontStyle;
};

class DLL_EXPORT CUIStyle
{
public:
	CUIStyle();
	~CUIStyle(){};
	void Blend(const CRect<float>& rc, UINT iState, float fElapsedTime);
	void SetStyle(const std::string& strName);
	const CUICyclostyle& GetCyclostyle();
	void draw(const CRect<float>& rc, const std::wstring& wstrText, CONTROL_STATE state, float fElapsedTime);
	void Draw(const std::wstring& wstrText);
	bool isHidden();

	bool m_bHidden;
	std::string	m_strName;
	std::map<int,StyleDrawData> m_mapSprite;
	std::map<int,StyleDrawData> m_mapBorder;
	std::map<int,StyleDrawData> m_mapSquare;
	std::map<int,StyleDrawData> m_mapFont;
};

class CUIStyleMgr
{
	std::map<std::string, CUICyclostyle> m_CyclostyleList;
	std::string m_strFilename;
public:
	CUIStyleMgr();
	bool Create(const std::string& strFilename);
	const std::string& getFilename(){return m_strFilename;}
	const CUICyclostyle& GetCyclostyle(const std::string& strName);
};

CUIStyleMgr& GetStyleMgr();