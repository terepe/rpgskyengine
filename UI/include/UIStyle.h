#pragma once
#include "Common.h"
#include "Manager.h"
#include "Color.h"
#include <vector>
#include <map>
#include <Windows.h>
#include "Vec4D.h"
#include "interpolation.h"

class TiXmlElement;

void StrToRect(const char* str, RECT& rect);
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
	Vec4D	vColor;
	RECT	rc;
};

struct ControlBlendColor
{
	ControlBlendColor()
	{
// 		memset(&ColorOfStates,0,sizof)
// 		for(int i=0; i < CONTROL_STATE_MAX; i++)
// 		{
// 			ColorOfStates[i] = 0x0;
// 		}
	}

	Vec4D Blend(Vec4D crCurrent, UINT iState, float fElapsedTime, float fRate) const
	{
		return interpolate(1.0f - powf(fRate, 30 * fElapsedTime),crCurrent,ColorOfStates[ iState ]);
		//return Color32::lerp(1.0f - powf(fRate, 30 * fElapsedTime), crCurrent, States[ iState ]);
	}

	void XMLStateColor(TiXmlElement& element);

	Vec4D	ColorOfStates[CONTROL_STATE_MAX];
};

struct BaseCyclostyle: public ControlBlendColor
{
	BaseCyclostyle()
	{
		uFormat = 0;
		SetRect(&rcOffset,0,0,0,0);
	}
	RECT rcOffset;
	uint32 uFormat;// The format argument to DrawText or Texture
	virtual void XML(TiXmlElement& element);
	virtual void updataRect(RECT& rc)const;
};

class CUISpriteCyclostyle: public BaseCyclostyle
{
public:
	virtual void XML(TiXmlElement& element);
	virtual void updataRect(RECT& rc)const;
	void		draw(const RECT& rc,const Color32& color)const;
	int			m_nTexture;
	bool		m_bDecolor;
	int			m_nSpriteLayoutType;
	RECT		m_rcBorder;
	RECT		m_rcCenter;
};

class CUITextCyclostyle: public BaseCyclostyle
{
public:
	virtual void updataRect(RECT& rc)const;
	void draw(const std::wstring& wstrText,const RECT& rc,const Color32& color)const;
};

struct  StyleBorder: public BaseCyclostyle
{
	void draw(const RECT& rc,const Color32& color)const;
};

struct  StyleBackgroundColor: public BaseCyclostyle
{
	void draw(const RECT& rc,const Color32& color)const;
};

class DLL_EXPORT CUICyclostyle
{
public:
	CUICyclostyle(){}
	void Refresh();
	void add(const CUICyclostyle& cyc);
	std::vector<CUISpriteCyclostyle> m_SpriteStyle;
	std::vector<StyleBorder> m_setBorder;
	std::vector<StyleBackgroundColor> m_setBackgroundColor;
	std::vector<CUITextCyclostyle> m_FontStyle;
};

class DLL_EXPORT CUIStyle
{
public:


	CUIStyle(){};
	~CUIStyle(){};
	void Blend(const RECT& rc, UINT iState, float fElapsedTime, float fRate = 0.7f);
	void SetStyle(const std::string& strName);
	const CUICyclostyle& GetCyclostyle();
	void draw(const RECT& rc, const std::wstring& wstrText, CONTROL_STATE state, float fElapsedTime, float fRate = 0.7f);
	void Draw(const std::wstring& wstrText);

	std::string	m_strName;
	std::map<int,StyleDrawData> m_mapSprite;
	std::map<int,StyleDrawData> m_mapBorder;
	std::map<int,StyleDrawData> m_mapBackground;
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