#pragma once
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<assert.h>
#include	<math.h>

#ifdef _BCB
#define sqrtf sqrt
#define	__max max
#endif

#include	<windows.h>
#include	<wingdi.h>

#include	"c3_phy.h"
#include    "basefunc.h"
#include	"TextRender.h"

// color define
//const int _COLOR_WHITE		=0xffffffff;
//const int _COLOR_GRAY		=0xff7f7f7f;
//const int _COLOR_DARKGRAY	=0xff3f3f3f;
//const int _COLOR_BLACK		=0xff000000;
//const int _COLOR_RED		=0xffff0000;
//const int _COLOR_GREEN		=0xff00ff00;
//const int _COLOR_BLUE		=0xff0000ff;
//const int _COLOR_YELLOW		=0xffffff00;
//const int _COLOR_TRANS		=0xffff00ff;

const char _V_ARMET[]		="v_head";
const char _V_MISC[]		="v_misc";
const char _V_L_WEAPON[]	="v_l_weapon";
const char _V_R_WEAPON[]	="v_r_weapon";
const char _V_L_SHIELD[]	="v_l_shield";
const char _V_R_SHIELD[]	="v_r_shield";
const char _V_L_SHOE[]		="v_l_shoe";
const char _V_R_SHOE[]		="v_r_shoe";
const char _V_BODY[]		="v_body";
const char _V_PET[]			="v_pet";
const char _V_BACK[]		="v_back";

const char _V_HEAD[]		="v_armet";
const char _V_L_ARM[]		="v_l_arm";
const char _V_R_ARM[]		="v_r_arm";
const char _V_L_LEG[]		="v_l_leg";
const char _V_R_LEG[]		="v_r_leg";
const char _V_MANTLE[]		="v_mantle";



// video mode enum 
//enum {	modeNone=0, 
//modeSprite, 
//modeLine, 
//modeElement, 
//modeMesh,
//modeShape,
//modePtcl,
//modeText
//};

// struct
#define LINE_VERTEX ( FVF_XYZRHW | FVF_DIFFUSE )
struct LineVertex
{
	float		x, y, z;		// 坐标
	float		rhw;
	DWORD	color;
};

#include <deque>                             


//------------------------------------------------------
class C3DTexture
{
public:
	C3DTexture();
	virtual ~C3DTexture();

	BOOL	Create	(const char* pszFileName);
	void	Destroy	(void);

	int		GetTexture	(void)	{return m_nTexture;}
private:
	int		m_nTexture;
};


//------------------------------------------------------
#define		MOTION_MAX	16
class C3DMotion
{
public:
	C3DMotion ();
	virtual ~C3DMotion ();

	bool IsValid ( void );

	bool Create(const std::string& strFilename);
	bool Destroy ( void );

	DWORD	GetFrameAmount();

public:
	DWORD		m_dwMotionNum;
	C3Motion	*m_motion[MOTION_MAX];
};



//------------------------------------------------------
#define		PHY_MAX		16
class C3DObj
{
public:
	C3DObj ();
	virtual ~C3DObj ();

	// 实例是否有效
	bool IsValid ();
	// 创建一个模型 *.c3
	bool Create(const std::string& strFilename);
	// 释放
	void Destroy ();

	// 颜色，范围从 0.0f - 1.0f
	void SetARGB ( float alpha, float red, float green, float blue );

	// 通过物体名字得到物体索引
	int GetIndexByName ( char *lpName );

	// 设置动作
	void SetMotion ( C3DMotion* pMotion );

	// 下一帧 nStep 步幅 负直为倒播
	void NextFrame ( int nStep );

	// 设置当前帧数
	void SetFrame (DWORD dwFrame);

	// 绘制
	void Draw ( int type, float lightx, float lighty, float lightz, float sa, float sr, float sg, float sb );
	void DrawAlpha ( int type, float lightx, float lighty, float lightz, float sa, float sr, float sg, float sb, float height );
	// 换贴图
	void ChangeTexture ( C3DTexture* pTexture, char *objname=NULL );

	// 准备3D对象绘制
	static void Prepare ( void );

	void ClearMatrix ( void );

public:
	DWORD		m_dwPhyNum;
	C3Phy		*m_phy[PHY_MAX];

	float		m_x[PHY_MAX];
	float		m_y[PHY_MAX];
	float		m_z[PHY_MAX];
};