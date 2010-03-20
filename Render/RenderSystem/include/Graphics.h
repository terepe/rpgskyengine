#pragma once
#include "RenderSystem.h"
#include "Frustum.h"
#include <Windows.h>
#include "Rect.h"

enum eDrawFlag
{
	eDraw_NORMAL		=	1<<1,
	eDraw_ADD			=	1<<2,
	eDraw_SUB			=	1<<3,
	eDraw_UseTex		=	1<<4,
	eDraw_AlphaBlend	=	1<<5,
};

typedef enum _BGMODE {
	BGMODE_POIN				= 1,
	BGMODE_LINE				= 2,
	BGMODE_LINESTRIP		= 3,
	BGMODE_QUADS			= 4,
	BGMODE_QUADSTRIP		= 5,
	BGMODE_TRIANGLEFAN		= 6,
	BGMODE_FORCE_DWORD		= 0x7fffffff, /* force 32-bit size enum */
} BGMODE;

class DLL_EXPORT CGraphics
{
public:
	CGraphics(void);
	~CGraphics(void);
public:
	uint32 m_uMode;
	VERTEX_XYZ_DIF_TEX	m_Buffer[10000];
public:
	void Clear();
	//
	void Begin		(BGMODE mode, uint32 nSize);
	//
	void Color	(Color32 c);
	void TexCoord2fv(const float *v);
	void TexCoord2f	(const float u, const float v);
	void Vertex2fv	(const float *v);
	void Vertex3fv	(const float *v);
	//
	void End();
	/// 2D几何绘制
	// 直线
	void DrawLine	(int x0, int y0, int x1, int y1, Color32 color);
	// 矩形框
	void DrawRect	(int x0, int y0, int x1, int y1, Color32 color);
	void DrawRect	(const CRect<float>& rcDest, Color32 color);
	// 矩形填充
	void FillRect	(int x0, int y0, int x1, int y1, Color32 color);
	void FillRect	(int x0, int y0, int x1, int y1, Color32 color0, Color32 color1, Color32 color2, Color32 color3);
	void FillRect	(const CRect<float>& rcDest, Color32 color);
	void FillRect	(const CRect<float>& rcDest, Color32 color, Color32 color0, Color32 color1, Color32 color2, Color32 color3);
	// 椭圆形
	void DrawCircle	(int x0, int y0, int x1, int y1, Color32 color);
	// 椭圆形填充
	void FillCircle	(int x0, int y0, int x1, int y1, Color32 color);

	//
	void DrawQuad	(const CRect<float>& rcSrc, const CRect<float>& rcDest, int nWidth, int nHeight, Color32 color);
	void DrawTex	(int destX, int destY, int nTexID, Color32 color);
	void DrawTex	(const CRect<float>& rcDest, int nTexID, Color32 color);
	void DrawTex	(const CRect<float>& rcSrc, const CRect<float>& rcDest, int nTexID, Color32 color);
	void DrawTex	(const CRect<float>& rcSrc, int destX, int destY, int nTexID, Color32 color);

	//
	void DrawTexWrap(const CRect<float>& rcSrc, const CRect<float>& rcDest, int nTexID, Color32 color);
	//
	void Draw3x3Grid(const CRect<float>& rcSrc, const CRect<float>& rcCenterSrc, const CRect<float>& rcDest, int nTexID, Color32 color);
	//
	void Draw3x3GridWrap(const CRect<float>& rcSrc, const CRect<float>& rcCenterSrc, const CRect<float>& rcDest, int nTexID, Color32 color);

	// 绘制2D线
	void Line2DBegin();
	void Line2DTo	(const Vec2D& v);
	void Line2DEnd	();
	// 绘制3D线
	void Line3DBegin();
	void Line3DTo	(const Vec3D& v);
	void Line3DEnd	();

	// 绘制3D直线
	void DrawLine3D	(const Vec3D& v0,const Vec3D& v1, Color32 color);
	// Quadrilateral
	void drawQuad	(const Vec3D& v0,const Vec3D& v1,const Vec3D& v2,const Vec3D& v3, Color32 color);
	void fillQuad	(const Vec3D& v0,const Vec3D& v1,const Vec3D& v2,const Vec3D& v3, Color32 color);
	// 绘制3D矩形
	void DrawRect3D	(const Vec3D& v0,const Vec3D& v1, Color32 color);
	void FillRect3D	(const Vec3D& v0,const Vec3D& v1, Color32 color);
	// 绘制3D椭圆形
	void DrawCircle3D(const Vec3D& v0,const Vec3D& v1, Color32 color);
	void FillCircle3D(const Vec3D& v0,const Vec3D& v1, Color32 color);
	// 绘制包围盒
	void drawBBox	(const BBox& bbox, Color32 color);

	/// 纹理
	// 绘制2D纹理
	void DrawTex2D	(const Vec2D& v0,const Vec2D& v1, float fZ, Color32 color, int nTexID);
	// 绘制3D纹理
	void DrawTex3D	(const Vec3D& v0,const Vec3D& v1, Color32 color, int nTexID);
protected:
	CHardwareVertexBuffer*		m_pVB;			// 顶点缓冲
	CHardwareIndexBuffer*		m_pIB;			// 索引缓冲
	//std::vector(VERTEX_XYZW_DIF_TEX) m_UIVB;
	int					m_nCount;
	int					m_nVBOffset;
	int					m_nVBBatchSize;
	VERTEX_XYZ_DIF_TEX	m_TempVertex;// 顶点缓冲 lock的时候得到的值
	VERTEX_XYZ_DIF_TEX*	m_pVertex;

	std::vector<VERTEX_XYZW_DIF>	m_set2DLine;
	std::vector<VERTEX_XYZ_DIF>		m_set3DLine;

	VERTEX_XYZW_DIF_TEX	m_vRectTex2D[4];
	VERTEX_XYZ_DIF_TEX	m_vRectTex3D[4];

	int					m_nPassCount;
};

DLL_EXPORT CGraphics& GetGraphics();