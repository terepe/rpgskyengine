#include "Graphics.h"
#include "RenderSystem.h"
#include "Vec4D.h"
#include "Vec3D.h"

#define MAX_VBSIZE 10000

int GetRectWidth(const RECT& rc)
{
	return abs(rc.right - rc.left);
}

int GetRectHeight(const RECT& rc)
{
	return abs(rc.bottom - rc.top);
}

CGraphics& GetGraphics()
{
	static CGraphics g_Graphics;
	return g_Graphics;
}

CGraphics::CGraphics(void)
{
	m_pVB = NULL;
	m_pIB = NULL;

	m_nVBOffset = 0;
	m_nVBBatchSize = 0;
	m_nCount = 0;

	m_vRectTex3D[0].t.x = 0;	m_vRectTex3D[0].t.y = 0;
	m_vRectTex3D[1].t.x = 0;	m_vRectTex3D[1].t.y = 1;
	m_vRectTex3D[2].t.x = 1;	m_vRectTex3D[2].t.y = 0;
	m_vRectTex3D[3].t.x = 1;	m_vRectTex3D[3].t.y = 1;

	m_pVB = GetRenderSystem().GetHardwareBufferMgr().CreateVertexBuffer(MAX_VBSIZE, sizeof(VERTEX_XYZ_DIF_TEX),
		CHardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);

	m_pIB = GetRenderSystem().GetHardwareBufferMgr().CreateIndexBuffer(6000);
	WORD* pIndex = (WORD*)m_pIB->lock(0, 6000*sizeof(WORD), CHardwareBuffer::HBL_NORMAL);
	for(int i = 0 ; i < 4000 ; i+=4)
	{
		*pIndex= i;
		pIndex++;
		*pIndex= i+1;
		pIndex++;
		*pIndex= i+2;
		pIndex++;
		*pIndex= i;
		pIndex++;
		*pIndex= i+2;
		pIndex++;
		*pIndex= i+3;
		pIndex++;
	}
	m_pIB->unlock();
}

CGraphics::~CGraphics(void)
{
	S_DEL(m_pVB);
	S_DEL(m_pIB);
}

void CGraphics::Clear()
{
	m_nVBOffset = 0;
}

void CGraphics::Begin(BGMODE mode, uint32 nSize)
{
	m_nCount = 0;
	m_uMode = mode;
	m_nVBBatchSize = nSize;
	if (m_nVBOffset + nSize > MAX_VBSIZE)
	{
		m_nVBOffset = 0;
	}
	m_pVertex = NULL;
	if (m_pVB)
	{
		m_pVertex = (VERTEX_XYZ_DIF_TEX*)m_pVB->lock(m_nVBOffset * sizeof(VERTEX_XYZ_DIF_TEX), nSize * sizeof(VERTEX_XYZ_DIF_TEX),
			m_nVBOffset?CHardwareBuffer::HBL_NO_OVERWRITE:CHardwareBuffer::HBL_DISCARD);
	}
	//assert(m_pVB);
	//assert(m_pVertex);
}

void CGraphics::Color(Color32 c)
{
	m_TempVertex.c = c;
}

void CGraphics::TexCoord2fv(const float *v)
{
	m_TempVertex.t = *(Vec2D*)v;
}

void CGraphics::TexCoord2f(const float u, const float v)
{
	m_TempVertex.t.x = u;
	m_TempVertex.t.y = v;
}

void CGraphics::Vertex2fv(const float *v)
{
	m_TempVertex.p = *(Vec3D*)v;
	m_pVertex[m_nCount] = m_TempVertex;
	m_nCount++;
	if (m_nCount>=10000)
	{
		m_nCount=0;
	}
}

void CGraphics::Vertex3fv(const float *v)
{
	m_TempVertex.p = *(Vec3D*)v;
	m_pVertex[m_nCount] = m_TempVertex;
	m_nCount++;
}

void CGraphics::End()
{
	if (!m_pVB || m_nCount==0) return;
	m_pVB->unlock();
	CRenderSystem& R = GetRenderSystem();
	R.SetFVF(VERTEX_XYZ_DIF_TEX::FVF);
	R.SetStreamSource(0, m_pVB, 0, sizeof(VERTEX_XYZ_DIF_TEX));

	switch(m_uMode)
	{
		case BGMODE_POIN:
			R.DrawPrimitive(VROT_POINT_LIST, m_nVBOffset, m_nCount);
			break;
		case BGMODE_LINE:
			R.DrawPrimitive(VROT_LINE_LIST, m_nVBOffset, m_nCount/2);
			break;
		case BGMODE_LINESTRIP:
			break;
		case BGMODE_QUADS:
			R.SetIndices(m_pIB);
			R.DrawIndexedPrimitive(VROT_TRIANGLE_LIST, m_nVBOffset, 0, m_nCount, 0, m_nCount/2);
			break;
		case BGMODE_QUADSTRIP:
			R.DrawPrimitive(VROT_TRIANGLE_STRIP, m_nVBOffset, m_nCount-2);
			break;
		default:
			break;
	}

///	assert(m_nCount>0);

///	assert(m_nVBBatchSize>=m_nCount);
	m_nVBOffset += m_nCount;
	m_nCount = 0;
}

//void CGraphics::Begin(BGMODE mode, uint32 nSize)
//{
//	m_dwMode = mode;
//}
//
//void CGraphics::Vertex2fv(const float *v)
//{
//	m_TempVertex.p = *(Vec3D*)v;
//	m_Buffer[m_nCount] = m_TempVertex;
//	m_nCount++;
//}
//
//void CGraphics::Vertex3fv(const float *v)
//{
//	m_TempVertex.p = *(Vec3D*)v;
//	m_Buffer[m_nCount] = m_TempVertex;
//	m_nCount++;
//}

//void CGraphics::End()
//{
//	CRenderSystem& R = GetRenderSystem();
//	R.SetFVF(VERTEX_XYZ_DIF_TEX::FVF);

//		switch(m_dwMode) {
//		case BGMODE_POIN:
//			break;
//		case BGMODE_LINE:
//			R.DrawPrimitiveUP(VROT_LINE_LIST, m_nCount/2,m_Buffer + m_nOffset, sizeof(VERTEX_XYZ_DIF_TEX));
//			break;
//		case BGMODE_LINESTRIP:
//			break;
//		case BGMODE_QUADS:
//			R.DrawPrimitiveUP(VROT_TRIANGLE_LIST, m_nCount/3,m_Buffer + m_nOffset, sizeof(VERTEX_XYZ_DIF_TEX));
//			break;
//		case BGMODE_QUADSTRIP:
//			R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP,m_nCount-2,m_Buffer + m_nOffset, sizeof(VERTEX_XYZ_DIF_TEX));
//			break;
//		default:
//			break;
//		}
//	
//	m_nOffset += m_nCount;
//	m_nCount = 0;
//}

void CGraphics::Line2DBegin()
{
	m_set2DLine.clear();
}

void CGraphics::Line2DTo(const Vec2D& v)
{
	VERTEX_XYZW_DIF	temp;
	temp.p = Vec4D(v.x, v.y, 0, 1);
	temp.c = m_TempVertex.c;
	m_set2DLine.push_back(temp);
}

void CGraphics::Line2DEnd()
{
	if (m_set2DLine.size()>1)
	{
		CRenderSystem& R = GetRenderSystem();
		R.SetFVF(VERTEX_XYZW_DIF::FVF);

		R.DrawPrimitiveUP(VROT_LINE_STRIP, m_set2DLine.size()-1, &m_set2DLine[0], sizeof(VERTEX_XYZW_DIF));
	}
}

void CGraphics::Line3DBegin()
{
	m_set3DLine.clear();
}

void CGraphics::Line3DTo(const Vec3D& v)
{
	VERTEX_XYZ_DIF	temp;
	temp.p = v;
	temp.c = m_TempVertex.c;
	m_set3DLine.push_back(temp);
}

void CGraphics::Line3DEnd()
{
	if (m_set3DLine.size()>1)
	{
		CRenderSystem& R = GetRenderSystem();
		R.SetFVF(VERTEX_XYZ_DIF::FVF);

		R.DrawPrimitiveUP(VROT_LINE_STRIP, m_set3DLine.size()-1, &m_set3DLine[0], sizeof(VERTEX_XYZ_DIF));
	}
}

//////////////////////////////////////////////////////////////////////////
//2D Graph
//////////////////////////////////////////////////////////////////////////
void CGraphics::DrawLine(int x0, int y0, int x1, int y1, Color32 color)
{
	VERTEX_XYZW_DIF v[2]=
	{
		Vec4D((float) x0, (float) y0, 0.5f, 1.0f), color,
		Vec4D((float) x1, (float) y1, 0.5f, 1.0f), color,
	};
	GetRenderSystem().SetFVF(VERTEX_XYZW_DIF::FVF);
	GetRenderSystem().DrawPrimitiveUP(VROT_LINE_STRIP, 1, v, sizeof(VERTEX_XYZW_DIF));
}

void CGraphics::DrawRect(int x0, int y0, int x1, int y1, Color32 color)
{
	// border adjustment
	if(x0<x1)
		++x1;
	else if(x0>x1)
		++x0;
	if(y0<y1)
		++y1;
	else if(y0>y1)
		++y0;
	VERTEX_XYZW_DIF v[5]=
	{
		Vec4D((float) x0, (float) y0, 0.5f, 1.0f), color,
		Vec4D((float) x1, (float) y0, 0.5f, 1.0f), color,
		Vec4D((float) x1, (float) y1, 0.5f, 1.0f), color,
		Vec4D((float) x0, (float) y1, 0.5f, 1.0f), color,
		Vec4D((float) x0, (float) y0, 0.5f, 1.0f), color,
	};
	CRenderSystem& R = GetRenderSystem();
	R.SetFVF(VERTEX_XYZW_DIF::FVF);
	R.DrawPrimitiveUP(VROT_LINE_STRIP, 4, v, sizeof(VERTEX_XYZW_DIF));
}

void CGraphics::DrawRect(const RECT& rcDest, Color32 color)
{
	DrawRect(rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, color);
}

void CGraphics::FillRect(int x0, int y0, int x1, int y1, Color32 color)
{
	// border adjustment
	/*if(x0<x1)
		++x1;
	else if(x0>x1)
		++x0;
	if(y0<y1)
		++y1;
	else if(y0>y1)
		++y0;*/
	VERTEX_XYZW_DIF v[4]=
	{
		Vec4D((float) x0, (float) y0, 0.5f, 1.0f), color,
		Vec4D((float) x1, (float) y0, 0.5f, 1.0f), color,
		Vec4D((float) x1, (float) y1, 0.5f, 1.0f), color,
		Vec4D((float) x0, (float) y1, 0.5f, 1.0f), color,
	};
	CRenderSystem& R = GetRenderSystem();
	R.SetFVF(VERTEX_XYZW_DIF::FVF);
	R.DrawPrimitiveUP(VROT_TRIANGLE_FAN, 2, v, sizeof(VERTEX_XYZW_DIF));
}

void CGraphics::FillRect(int x0, int y0, int x1, int y1, Color32 color0, Color32 color1, Color32 color2, Color32 color3)
{
	// border adjustment
	/*if(x0<x1)
		++x1;
	else if(x0>x1)
		++x0;
	if(y0<y1)
		++y1;
	else if(y0>y1)
		++y0;*/
	VERTEX_XYZW_DIF v[4]=
	{
		Vec4D((float) x0, (float) y0, 0.5f, 1.0f), color0,
		Vec4D((float) x1, (float) y0, 0.5f, 1.0f), color1,
		Vec4D((float) x1, (float) y1, 0.5f, 1.0f), color2,
		Vec4D((float) x0, (float) y1, 0.5f, 1.0f), color3,
	};
	CRenderSystem& R = GetRenderSystem();
	R.SetFVF(VERTEX_XYZW_DIF::FVF);
	R.DrawPrimitiveUP(VROT_TRIANGLE_FAN, 2, v, sizeof(VERTEX_XYZW_DIF));
}

void CGraphics::FillRect(const RECT& rcDest, Color32 color)
{
	FillRect(rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, color);
}

void CGraphics::FillRect(const RECT& rcDest, Color32 color, Color32 color0, Color32 color1, Color32 color2, Color32 color3)
{
	FillRect(rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, color0, color1, color2, color3);
}

//////////////////////////////////////////////////////////////////////////

void CGraphics::DrawQuad(const RECT& rcSrc, const RECT& rcDest, int nWidth, int nHeight, Color32 color)
{
	CRenderSystem& R = GetRenderSystem();
	float u0 = (float)(rcSrc.left+0.5f)		/ (float)nWidth;
	float v0 = (float)(rcSrc.top+0.5f)		/ (float)nHeight;
	float u1 = (float)(rcSrc.right)	/ (float)nWidth;
	float v1 = (float)(rcSrc.bottom)	/ (float)nHeight;
	VERTEX_XYZW_DIF_TEX v[4]=
	{
		Vec4D((float) rcDest.left,	(float) rcDest.top,		0.5f, 1.0f), color, Vec2D(u0, v0),
		Vec4D((float) rcDest.right,	(float) rcDest.top,		0.5f, 1.0f), color, Vec2D(u1, v0),
		Vec4D((float) rcDest.right,	(float) rcDest.bottom,	0.5f, 1.0f), color, Vec2D(u1, v1),
		Vec4D((float) rcDest.left,	(float) rcDest.bottom,	0.5f, 1.0f), color, Vec2D(u0, v1),
	};
	R.SetFVF(VERTEX_XYZW_DIF_TEX::FVF);
	R.DrawPrimitiveUP(VROT_TRIANGLE_FAN, 2, v, sizeof(VERTEX_XYZW_DIF_TEX));
}

void CGraphics::DrawTex(int destX, int destY, int nTexID, Color32 color)
{
	CTexture* pTex = GetRenderSystem().GetTextureMgr().getItem(nTexID);
	if (pTex)
	{
		int nTexWidth = pTex->GetWidth();
		int nTexHeight = pTex->GetHeight();
		RECT rcSrc = {0,0,nTexWidth,nTexHeight};
		RECT rcDest = {destX,destX,nTexWidth+destX,nTexHeight+destX};
		GetRenderSystem().SetTexture(0, nTexID);
		DrawQuad(rcSrc, rcDest, nTexWidth, nTexHeight, color);
	}
}

void CGraphics::DrawTex(const RECT& rcDest, int nTexID, Color32 color)
{
	CTexture* pTex = GetRenderSystem().GetTextureMgr().getItem(nTexID);
	if (pTex)
	{
		int nTexWidth = pTex->GetWidth();
		int nTexHeight = pTex->GetHeight();
		RECT rcSrc = {0,0,nTexWidth,nTexHeight};
		GetRenderSystem().SetTexture(0, nTexID);
		DrawQuad(rcSrc, rcDest, nTexWidth, nTexHeight, color);
	}
}

void CGraphics::DrawTex(const RECT& rcSrc, const RECT& rcDest, int nTexID, Color32 color)
{
	CRenderSystem& R = GetRenderSystem();
	CTexture* pTex = GetRenderSystem().GetTextureMgr().getItem(nTexID);
	if (pTex)
	{
		R.SetTexture(0, nTexID);
		DrawQuad(rcSrc, rcDest, pTex->GetWidth(), pTex->GetHeight(), color);
	}
}

void CGraphics::DrawTex(const RECT& rcSrc, int destX, int destY, int nTexID, Color32 color)
{
	RECT rcDest = rcSrc;
	rcDest.right += destX;
	rcDest.left += destX;
	rcDest.top += destY;
	rcDest.bottom += destY;
	DrawTex(rcSrc, rcDest, nTexID, color);
}

void CGraphics::DrawTexWrap(const RECT& rcSrc, const RECT& rcDest, int nTexID, Color32 color)
{
	CTexture* pTex = GetRenderSystem().GetTextureMgr().getItem(nTexID);
	if (pTex)
	{
		int nTileWidth = GetRectWidth(rcSrc);
		int nTileHeight = GetRectHeight(rcSrc);
		int nWidth = GetRectWidth(rcDest)/nTileWidth;
		int nHeight = GetRectHeight(rcDest)/nTileHeight;

		int nTileCount = int(nWidth) * int(nHeight);
		int nVertexCount = nTileCount*4;
		int nIndexCount = nTileCount*6;

		float u0 = (rcSrc.left+0.5f)	/ (float)pTex->GetWidth();
		float v0 = (rcSrc.top+0.5f)		/ (float)pTex->GetHeight();
		float u1 = (rcSrc.right+0.5f)	/ (float)pTex->GetWidth();
		float v1 = (rcSrc.bottom+0.5f)	/ (float)pTex->GetHeight();
		// VB
		VERTEX_XYZW_DIF_TEX* vb = new VERTEX_XYZW_DIF_TEX[nVertexCount];
		for (int y = 0; y < nHeight; y++)
		{
			for (int x = 0; x < nWidth; x++)
			{
				VERTEX_XYZW_DIF_TEX v[ 4 ] =
				{
					Vec4D((float) rcDest.left+nTileWidth*x,		(float) rcDest.top+nTileHeight*y,		0.5f, 1.0f), color, Vec2D(u0, v0),
					Vec4D((float) rcDest.left+nTileWidth*(x+1),	(float) rcDest.top+nTileHeight*y,		0.5f, 1.0f), color, Vec2D(u1, v0),
					Vec4D((float) rcDest.left+nTileWidth*(x+1),	(float) rcDest.top+nTileHeight*(y+1),	0.5f, 1.0f), color, Vec2D(u1, v1),
					Vec4D((float) rcDest.left+nTileWidth*x,		(float) rcDest.top+nTileHeight*(y+1),	0.5f, 1.0f), color, Vec2D(u0, v1),
				};
				memcpy(&vb[ (y * nWidth + x)*4 ], v, sizeof(VERTEX_XYZW_DIF_TEX)*4);
			}
		}
		// IB
		WORD* ib = new WORD[nIndexCount];
		for (int i = 0; i < nTileCount; i++)
		{
			ib[i*6]= i*4;
			ib[i*6+1]= i*4+1;
			ib[i*6+2]= i*4+2;
			ib[i*6+3]= i*4;
			ib[i*6+4]= i*4+2;
			ib[i*6+5]= i*4+3;
		}

		CRenderSystem& R = GetRenderSystem();
		R.SetTexture(0, nTexID);
		R.SetFVF(VERTEX_XYZW_DIF_TEX::FVF);
		R.DrawIndexedPrimitiveUP(VROT_TRIANGLE_LIST, 0, nVertexCount, nTileCount*2, ib, vb, sizeof(VERTEX_XYZW_DIF_TEX));
		// ɾ��
		delete[] vb;
		delete[] ib;
	}
}

void CGraphics::Draw3x3Grid(const RECT& rcSrc, const RECT& rcCenterSrc, const RECT& rcDest, int nTexID, Color32 color)
{
	CTexture* pTex = GetRenderSystem().GetTextureMgr().getItem(nTexID);
	if (pTex)
	{
		RECT rcCenterDest =
		{
			rcDest.left + (rcCenterSrc.left - rcSrc.left),
			rcDest.top + (rcCenterSrc.top - rcSrc.top),
			rcDest.right + (rcCenterSrc.right - rcSrc.right),
			rcDest.bottom + (rcCenterSrc.bottom - rcSrc.bottom),
		};

		VERTEX_XYZW_DIF_TEX vertex[4*4];
		for (int i = 0; i<16; i++)
		{
			vertex[i].p.z = 0.5f;
			vertex[i].p.w = 1.0f;
			vertex[i].c = color;
		}
		//////////////////////////////////////////////////////////////////////////
		float X[4] =
		{
			(float)rcDest.left,
			(float)rcCenterDest.left,
			(float)rcCenterDest.right,
			(float)rcDest.right,
		};
		float U[4] =
		{
			(float)(rcSrc.left+0.5f)/pTex->GetWidth(),
			(float)(rcCenterSrc.left+0.5f)/pTex->GetWidth(),
			(float)(rcCenterSrc.right+0.5f)/pTex->GetWidth(),
			(float)(rcSrc.right+0.5f)/pTex->GetWidth(),
		};
		for (int i = 0; i<4; i++)
		{
			for (int n = 0; n < 4; n ++)
			{
				int index = i*4+n;
				vertex[index].p.x = X[n];
				vertex[index].t.x = U[n];
			}
		}
		//////////////////////////////////////////////////////////////////////////
		float Y[4] =
		{
			(float)rcDest.top,
			(float)rcCenterDest.top,
			(float)rcCenterDest.bottom,
			(float)rcDest.bottom,
		};
		float V[4] =
		{
			(float)(rcSrc.top+0.5f)/pTex->GetHeight(),
			(float)(rcCenterSrc.top+0.5f)/pTex->GetHeight(),
			(float)(rcCenterSrc.bottom+0.5f)/pTex->GetHeight(),
			(float)(rcSrc.bottom+0.5f)/pTex->GetHeight(),
		};
		for (int i = 0; i<4; i++)
		{
			for (int n = 0; n < 4; n ++)
			{
				int index = i+n*4;
				vertex[index].p.y = Y[n];
				vertex[index].t.y = V[n];
			}
		}
		//////////////////////////////////////////////////////////////////////////
		const static uint16 index[3*3*6] = { 0,0+1,0+5,		0,0+5,0+4,
									 1,1+1,1+5,		1,1+5,1+4,
									 2,2+1,2+5,		2,2+5,2+4,
									 4,4+1,4+5,		4,4+5,4+4,
									 5,5+1,5+5,		5,5+5,5+4,
									 6,6+1,6+5,		6,6+5,6+4,
									 8,8+1,8+5,		8,8+5,8+4,
									 9,9+1,9+5,		9,9+5,9+4,
									 10,10+1,10+5,	10,10+5,10+4};

		CRenderSystem& R = GetRenderSystem();
		R.SetTexture(0, nTexID);
		R.SetFVF(VERTEX_XYZW_DIF_TEX::FVF);
		R.DrawIndexedPrimitiveUP(VROT_TRIANGLE_LIST, 0, 4*4, 3*3*2, index, vertex, sizeof(VERTEX_XYZW_DIF_TEX));
	}
}

void CGraphics::Draw3x3GridWrap(const RECT& rcSrc, const RECT& rcCenterSrc, const RECT& rcDest, int nTexID, Color32 color)
{
	RECT rcCenterDest =
	{
		rcDest.left + (rcCenterSrc.left - rcSrc.left),
		rcDest.top + (rcCenterSrc.top - rcSrc.top),
		rcDest.right + (rcCenterSrc.right - rcSrc.right),
		rcDest.bottom + (rcCenterSrc.bottom - rcSrc.bottom),
	};

	int SrcX[4] =
	{
		rcSrc.left,
		rcCenterSrc.left,
		rcCenterSrc.right,
		rcSrc.right,
	};
	int SrcY[4] =
	{
		rcSrc.top,
		rcCenterSrc.top,
		rcCenterSrc.bottom,
		rcSrc.bottom,
	};
	int DestX[4] =
	{
		rcDest.left,
		rcCenterDest.left,
		rcCenterDest.right,
		rcDest.right,
	};
	int DestY[4] =
	{
		rcDest.top,
		rcCenterDest.top,
		rcCenterDest.bottom,
		rcDest.bottom,
	};
	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			RECT rcGridSrc = {SrcX[0+x],SrcY[0+y],SrcX[1+x],SrcY[1+y]};
			RECT rcGridDest = {DestX[0+x],DestY[0+y],DestX[1+x],DestY[1+y]};
			DrawTexWrap(rcGridSrc, rcGridDest, nTexID, color);
		}
	}
}
///////////////////////////////////////////////////////////////////////////
//3D Graph
//////////////////////////////////////////////////////////////////////////
void CGraphics::DrawLine3D(const Vec3D& v0,const Vec3D& v1, Color32 color)
{
	VERTEX_XYZ_DIF v[2];

	v[0].p = v0;
	v[1].p = v1;
	v[0].c = color;
	v[1].c = color;

	CRenderSystem& R = GetRenderSystem();

	R.SetFVF(VERTEX_XYZ_DIF::FVF);
	R.DrawPrimitiveUP(VROT_LINE_STRIP, 1, v, sizeof(VERTEX_XYZ_DIF));
}

void CGraphics::drawQuad(const Vec3D& v0,const Vec3D& v1,const Vec3D& v2,const Vec3D& v3, Color32 color)
{
	VERTEX_XYZ_DIF v[5];
	v[0].p = v0;
	v[1].p = v1;
	v[2].p = v2;
	v[3].p = v3;
	v[4].p = v0;
	v[0].c = color;
	v[1].c = color;
	v[2].c = color;
	v[3].c = color;
	v[4].c = color;

	CRenderSystem& R = GetRenderSystem();
	R.SetFVF(VERTEX_XYZ_DIF::FVF);
	R.DrawPrimitiveUP(VROT_LINE_STRIP, 4, v, sizeof(VERTEX_XYZ_DIF));
}

void CGraphics::fillQuad(const Vec3D& v0,const Vec3D& v1,const Vec3D& v2,const Vec3D& v3, Color32 color)
{
	VERTEX_XYZ_DIF v[4];
	v[0].p = v0;
	v[1].p = v1;
	v[2].p = v2;
	v[3].p = v3;
	v[0].c = color;
	v[1].c = color;
	v[2].c = color;
	v[3].c = color;
	CRenderSystem& R = GetRenderSystem();
	R.SetFVF(VERTEX_XYZ_DIF::FVF);
	R.DrawPrimitiveUP(VROT_TRIANGLE_FAN, 2, v, sizeof(VERTEX_XYZ_DIF));
}

void CGraphics::DrawRect3D(const Vec3D& v0,const Vec3D& v1, Color32 color)
{
	VERTEX_XYZ_DIF v[4];
	v[0].p = v0;
	v[1].p = Vec3D(v1.x, v0.y, v0.z);
	v[2].p = v1;
	v[3].p = Vec3D(v0.x, v1.y, v1.z);
	v[4].p = v[0].p;
	v[0].c = color;
	v[1].c = color;
	v[2].c = color;
	v[3].c = color;
	v[4].c = color;

	CRenderSystem& R = GetRenderSystem();
	R.SetFVF(VERTEX_XYZ_DIF::FVF);
	R.DrawPrimitiveUP(VROT_LINE_STRIP, 4, v, sizeof(VERTEX_XYZ_DIF));
}

void CGraphics::FillRect3D(const Vec3D& v0,const Vec3D& v1, Color32 color)
{
	VERTEX_XYZ_DIF v[4];
	v[0].p = v0;
	v[1].p = Vec3D(v1.x, v0.y, v0.z);
	v[2].p = Vec3D(v0.x, v1.y, v1.z);
	v[3].p = v1;
	v[0].c = color;
	v[1].c = color;
	v[2].c = color;
	v[3].c = color;
	CRenderSystem& R = GetRenderSystem();
	R.SetFVF(VERTEX_XYZ_DIF::FVF);
	R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, v, sizeof(VERTEX_XYZ_DIF));
}

void CGraphics::drawBBox(const BBox& bbox, Color32 color)
{
	VERTEX_XYZ_DIF vtx[8] =
	{
		Vec3D(bbox.vMin.x, bbox.vMax.y, bbox.vMax.z), color,
		Vec3D(bbox.vMax.x, bbox.vMax.y, bbox.vMax.z), color,
		Vec3D(bbox.vMax.x, bbox.vMax.y, bbox.vMin.z), color,
		Vec3D(bbox.vMin.x, bbox.vMax.y, bbox.vMin.z), color,
		Vec3D(bbox.vMin.x, bbox.vMin.y, bbox.vMax.z), color,
		Vec3D(bbox.vMax.x, bbox.vMin.y, bbox.vMax.z), color,
		Vec3D(bbox.vMax.x, bbox.vMin.y, bbox.vMin.z), color,
		Vec3D(bbox.vMin.x, bbox.vMin.y, bbox.vMin.z), color,
	};

	//Index3w		idx[12] = 
	//{ 
	//	//{ 0, 1, 2 }, { 0, 2, 3 },	/// �Ϸ�
	//	//{ 4, 6, 5 }, { 4, 7, 6 },	/// �·�
	//	//{ 0, 3, 7 }, { 0, 7, 4 },	/// ��
	//	//{ 1, 5, 6 }, { 1, 6, 2 },	/// �ҷ�
	//	//{ 3, 2, 6 }, { 3, 6, 7 },	/// ǰ��
	//	//{ 0, 4, 5 }, { 0, 5, 1 }	/// ��
	//}
	uint16		idx[24] = 
	{ 
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7

	};
	CRenderSystem& R = GetRenderSystem();
	R.SetTextureColorOP(0,TBOP_SOURCE2, TBS_TEXTURE, TBS_DIFFUSE);
	R.SetTextureColorOP(1,TBOP_DISABLE);
	R.SetFVF(VERTEX_XYZ_DIF::FVF);
	R.DrawIndexedPrimitiveUP(VROT_LINE_LIST, 0, 8, 12, idx, vtx, sizeof(VERTEX_XYZ_DIF));
}

void CGraphics::DrawTex2D(const Vec2D& v0,const Vec2D& v1, float fZ, Color32 color, int nTexID)
{
	VERTEX_XYZW_DIF_TEX v[4];
	v[0].p = Vec4D(v0.x, v0.y, fZ, 1);
	v[1].p = Vec4D(v1.x, v0.y, fZ, 1);
	v[2].p = Vec4D(v0.x, v1.y, fZ, 1);
	v[3].p = Vec4D(v1.x, v1.y, fZ, 1);
	v[0].t.x = 0;	v[0].t.y = 0;
	v[1].t.x = 1;	v[1].t.y = 0;
	v[2].t.x = 0;	v[2].t.y = 1;
	v[3].t.x = 1;	v[3].t.y = 1;
	v[0].c = color;
	v[1].c = color;
	v[2].c = color;
	v[3].c = color;
	CRenderSystem& R = GetRenderSystem();
	R.SetTexture(0, nTexID);
	R.SetFVF(VERTEX_XYZW_DIF_TEX::FVF);

	R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, v, sizeof(VERTEX_XYZW_DIF_TEX));
}

void CGraphics::DrawTex3D(const Vec3D& v0,const Vec3D& v1, Color32 color, int nTexID)
{
	m_vRectTex3D[0].p = v0;
	m_vRectTex3D[1].p = Vec3D(v1.x, v0.y, v0.z);
	m_vRectTex3D[2].p = Vec3D(v0.x, v1.y, v1.z);
	m_vRectTex3D[3].p = v1;

	m_vRectTex3D[0].c = color;
	m_vRectTex3D[1].c = color;
	m_vRectTex3D[2].c = color;
	m_vRectTex3D[3].c = color;
	CRenderSystem& R = GetRenderSystem();
	R.SetTexture(0, nTexID);
	R.SetFVF(VERTEX_XYZ_DIF_TEX::FVF);

	R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_vRectTex3D, sizeof(VERTEX_XYZ_DIF_TEX));
}