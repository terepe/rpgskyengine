#include "TextRender.h"
#include "RenderSystem.h"
#include "Vec4D.h"
#include "Vec3D.h"

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

//#include "FileSystem.h"
#include "IORead.h"
// 最接近的2的倍数
inline int next_p2 (int a)
{
	int rval=1;
	while(rval<a) rval<<=1;
	return rval;
}

CTextRender& GetTextRender()
{
	static CTextRender g_TextRender;
	return g_TextRender;
}
#define DRAW_TEXT_MAX	1024*8
static WORD s_DrawTextIB[DRAW_TEXT_MAX*6];

CTextRender::CTextRender()
{
	for (int i = 0; i < DRAW_TEXT_MAX; i++)
	{
		s_DrawTextIB[i*6]= i*4;
		s_DrawTextIB[i*6+1]= i*4+1;
		s_DrawTextIB[i*6+2]= i*4+2;
		s_DrawTextIB[i*6+3]= i*4;
		s_DrawTextIB[i*6+4]= i*4+2;
		s_DrawTextIB[i*6+5]= i*4+3;
	}
	m_dwColor			= 0xFFFFFFFF;
	m_bUpdated			= true;
	m_nTextTexWidth		= 1024;
	m_nTextTexHeight	= 1024;
	m_pTextTexture		= NULL;

	//Init("data\\Fonts\\DFPShaoNvW5-GB.ttf",24);
	//Init("data\\Fonts\\simfang.ttf",12);
	//Init("data\\Fonts\\ARIALUNI.TTF",22);
	//Init("data\\Fonts\\fzl2jw.ttf",12);

//	D3DLOCKED_RECT lockedRect;
	//m_pTextTexture->LockRect(0, &lockedRect, NULL, D3DLOCK_DISCARD);
	//byte* imageData = (byte*)lockedRect.pBits;
	//ZeroMemory(imageData,m_nTextTexWidth*m_nTextTexHeight);
	//m_pTextTexture->UnlockRect(0);
	//////////////////////////////////////////////////////////////////////////
	// read font info from regedit
	std::string strFontDir;
	{
		char szWindowsDir[MAX_PATH];
		GetWindowsDirectoryA(szWindowsDir,MAX_PATH);
		strFontDir = szWindowsDir;
		strFontDir+="\\Fonts\\";
	}
	HKEY hKey;
	if (ERROR_SUCCESS==RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"software\\microsoft\\windows nt\\currentversion\\fonts\\", 0, KEY_READ, &hKey))
	{
		wchar_t szValue[MAX_PATH];
		wchar_t szString[MAX_PATH];
		DWORD dwType=REG_SZ;
		DWORD dwValueSize=MAX_PATH;
		DWORD dwStringSize=MAX_PATH;
		DWORD dwEnumLoop=0;
		while (ERROR_NO_MORE_ITEMS!=RegEnumValueW(hKey,dwEnumLoop,szValue,&dwValueSize,NULL,&dwType,(BYTE*)szString,&dwStringSize))
		{
			std::string strFontFilename = strFontDir+ws2s(szString);
			std::wstring wstrFontName = szValue;
			// get the font simple name.
			{
				size_t pos = wstrFontName.find(L"(");
				if (pos!=std::wstring::npos&&pos>0)
				{
					wstrFontName = wstrFontName.substr(0,pos-1);
				}
			}
			//if (GetExtension(strFontFilename)==".ttf")
			{
				std::vector<std::wstring> out;
				TokenizerW(wstrFontName,out,L"&");
				for (std::vector<std::wstring>::const_iterator it=out.begin();it!=out.end();it++)
				{
					std::wstring wstrName = *it;
					wstrName = wstrName.substr(wstrName.find_first_not_of(L" "), std::wstring::npos);
					wstrName = wstrName.substr(0, wstrName.find_last_not_of(L" ")+1);
					registerFont(wstrName,strFontFilename);
				}
			}
			dwEnumLoop++;
			dwValueSize=MAX_PATH;
			dwStringSize=MAX_PATH;
		}
		RegCloseKey(hKey);
	}
	if (ERROR_SUCCESS==RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"software\\microsoft\\windows nt\\currentversion\\gre_initialize\\", 0, KEY_READ, &hKey))
	{
		wchar_t szString[MAX_PATH];
		DWORD dwType=REG_SZ;
		DWORD dwStringSize=MAX_PATH;
		if (ERROR_SUCCESS == RegQueryValueExW(hKey, L"guifont.Facename", NULL, &dwType, (BYTE*)&szString, &dwStringSize))
		{
			m_wstrDefaultFont = szString;
		}
		RegCloseKey(hKey);
	}
}

CTextRender::~CTextRender()
{
	Free();
}

int CTextRender::GetCharHeight() const
{
	return m_nH+2;
}

void CTextRender::registerFont(const std::wstring& wstrFontName, const std::string& strFilename)
{
	m_mapFontFilename[wstrFontName]=strFilename;
}

void CTextRender::Init(const std::wstring& wstrFontName, uint32 uSize) 
{
	std::string	strFilename;
	if (m_mapFontFilename.find(wstrFontName)==m_mapFontFilename.end())
	{
		strFilename = m_mapFontFilename[m_wstrDefaultFont];
	}
	else
	{
		strFilename = m_mapFontFilename[wstrFontName];
	}

	m_nH=uSize; 
	m_nLineHeight = int(m_nH*1.5f);

	//初始化FreeType库.. 
	if (FT_Init_FreeType((FT_Library*)&m_library))
		throw std::runtime_error("FT_Init_FreeType failed");

	//加载一个字体,取默认的Face,一般为Regualer 
	IOReadBase* pRead = IOReadBase::autoOpen(strFilename);
	if (pRead)
	{
		char* pBuf = new char[pRead->GetSize()];
		if (pBuf)
		{
			pRead->Read(pBuf, pRead->GetSize());
			if (FT_New_Memory_Face((FT_Library)m_library, (FT_Byte *)pBuf,pRead->GetSize(), 0, (FT_Face*)&m_Face)) 
				throw std::runtime_error("FT_New_Face failed (there is probably a problem with your font file)"); 
		}
		IOReadBase::autoClose(pRead);
	}


	FT_Select_Charmap((FT_Face)m_Face, FT_ENCODING_UNICODE);

	//大小要乘64.这是规定。照做就可以了。 
	//FT_Set_Char_Size((FT_Face)m_Face,uSize<< 6, uSize << 6, 96, 96);
	FT_Set_Pixel_Sizes((FT_Face)m_Face, uSize, uSize);
	//FT_Matrix matrix; /* transformation matrix */ 
	//FT_UInt glyph_index; 
	//FT_Vector pen; 

	////给它设置个旋转矩阵 
	//float angle = -20/180.* 3.14; 
	//matrix.xx = (FT_Fixed)(cos(angle) * 0x10000L); 
	//matrix.xy = (FT_Fixed)(-sin(angle) * 0x10000L); 
	//matrix.yx = (FT_Fixed)(sin(angle) * 0x10000L); 
	//matrix.yy = (FT_Fixed)(cos(angle) * 0x10000L); 


	//FT_Set_Transform((FT_Face)m_Face, &matrix, &pen); 
}

TexCharInfo* CTextRender::GetCharInfo(wchar_t ch)
{
	std::map<wchar_t,TexCharInfo>::iterator it = m_CharsInfo.find(ch);
	if (it != m_CharsInfo.end())
	{
		if (it->second.nLeft != -1)
		{
			return &it->second;
		}
	}
	else
	{
		TexCharInfo charInfo;
		charInfo.nLeft = -1;
	//	m_CharsInfo.inserter()
		m_CharsInfo[ch] = charInfo;

		// 需要更新
		m_bUpdated = false;
	}
	return NULL;
}

const int nBorderWidth = 0;
void CTextRender::OnFrameMove()
{
	if (!m_bUpdated)
	{
		Color32* imageData = new Color32[m_nTextTexWidth*m_nTextTexHeight];
		if (nBorderWidth>0)
		{
			ZeroMemory(imageData,m_nTextTexWidth*m_nTextTexHeight*4);
		}
		else
		{
			memset(imageData,0xFF,m_nTextTexWidth*m_nTextTexHeight*4);
		}

		int nLeft	= 0;
		int nTop	= 0;
		int nHeight = 0;
		for (std::map<wchar_t,TexCharInfo>::iterator it=m_CharsInfo.begin(); it!=m_CharsInfo.end(); ++it)
		{
			// 取字模数据
			//FT_Load_Char((FT_Face)m_Face, it->first, FT_LOAD_RENDER
			//	|FT_LOAD_FORCE_AUTOHINT
			//	|(true?FT_LOAD_TARGET_NORMAL : FT_LOAD_MONOCHROME|FT_LOAD_TARGET_MONO)); 
			FT_Load_Char((FT_Face)m_Face, it->first, FT_LOAD_DEFAULT);

			//得到字模
			FT_Glyph glyph = NULL;
			if(FT_Get_Glyph( ((FT_Face)m_Face)->glyph, &glyph ))
				break;

			//转化成位图
			FT_Glyph_To_Bitmap( &glyph, FT_RENDER_MODE_NORMAL, 0, 1 );
			FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

			//取道位图数据
			FT_Bitmap& bitmap=bitmap_glyph->bitmap;


			//FT_GlyphSlot slot = ((FT_Face)m_Face)->glyph; 

			if (nLeft + bitmap.width > m_nTextTexWidth)
			{
				nLeft = 0;
				nTop += nHeight+1;
				nHeight = 0;
			}
			// 超出纹理容量了
			if (nTop + bitmap.rows > m_nTextTexHeight)
			{
				m_CharsInfo.erase(it);
				continue;
			}
			// TexCharInfo
			TexCharInfo& charInfo = it->second;
			charInfo.nLeft		= nLeft;
			charInfo.nTop		= nTop;
			charInfo.nAdvX		= ((FT_Face)m_Face)->glyph->advance.x / 64;
			charInfo.nAdvY		= ((FT_Face)m_Face)->glyph->advance.y / 64;
			//m_FT_Face->size->metrics.y_ppem; //m_FT_Face->glyph->metrics.horiBearingY / 64.0f;

			charInfo.nWidth		= bitmap.width+nBorderWidth*2;
			charInfo.nHeight	= bitmap.rows+nBorderWidth*2;
			charInfo.nOffsetX	= bitmap_glyph->left;
			charInfo.nOffsetY	= bitmap_glyph->top;

			charInfo.fU0 = float(charInfo.nLeft+0.5f)	/ (float)m_nTextTexWidth;
			charInfo.fV0 = float(charInfo.nTop+0.5f)	/ (float)m_nTextTexHeight;
			charInfo.fU1 = float(charInfo.nLeft+charInfo.nWidth+0.5f) / (float)m_nTextTexWidth;
			charInfo.fV1 = float(charInfo.nTop+charInfo.nHeight+0.5f) / (float)m_nTextTexHeight;


			//把位图数据拷贝自己定义的画字纹理里。 
			if (nBorderWidth>0)
			{
				// shadow
				for(int y=0; y < bitmap.rows; y++)
				{ 
					for(int x=0; x < bitmap.width; x++) 
					{
						byte byColor = bitmap.buffer[x + bitmap.width*y];
						for (int offsetY=0; offsetY<nBorderWidth*2+1; ++offsetY)
						{
							for (int offsetX=0; offsetX<nBorderWidth*2+1; ++offsetX)
							{
								Color32& c = imageData[ (y + charInfo.nTop+offsetY) * m_nTextTexWidth + x + charInfo.nLeft+offsetX ];
								//c.a =min(255,c.a+byColor);
								c.a =max(c.a,byColor);
							}
						}
					} 
				}
				for(int y=0; y < bitmap.rows; y++)
				{ 
					for(int x=0; x < bitmap.width; x++) 
					{
						byte byColor = bitmap.buffer[x + bitmap.width*y];
						uint32 uColor = (byColor<<16)+(byColor<<8)+byColor;
						imageData[ (y + charInfo.nTop+nBorderWidth) * m_nTextTexWidth + x + charInfo.nLeft+nBorderWidth ].c |= uColor;
					} 
				}
			}
			else
			{
				// shadow
				for(int y=0; y < bitmap.rows; y++)
				{ 
					for(int x=0; x < bitmap.width; x++) 
					{
						byte byColor = bitmap.buffer[x + bitmap.width*y];
						Color32& c = imageData[ (y + charInfo.nTop) * m_nTextTexWidth + x + charInfo.nLeft ];
						c.a =byColor;
					} 
				}
			}

			//
			nLeft += charInfo.nWidth+1;
			if (nHeight < charInfo.nHeight)
			{
				nHeight = charInfo.nHeight;
			}
			//
			FT_Done_Glyph( glyph );
		}
		if(m_pTextTexture==NULL)
		{
			m_pTextTexture = GetRenderSystem().GetTextureMgr().CreateTexture(m_nTextTexWidth, m_nTextTexHeight);
				//GetRenderSystem().GetTextureMgr().CreateDynamicTexture(m_nTextTexWidth, m_nTextTexHeight);
		}
		if (m_pTextTexture)
		{
			m_pTextTexture->FillLevel(imageData,m_nTextTexWidth*m_nTextTexHeight*4,m_nTextTexWidth,m_nTextTexHeight,0);
		}
		S_DEL(imageData);
		//m_pTextTexture->UnlockRect(0);
		m_bUpdated = true;

		//
		m_bHasUpdata = true;

	}
}

//void CTextRender::DrawAUnicode(byte* pImage, int &x, int &y, wchar_t ch)
//{ 
//	/*if(FT_Load_Glyph((FT_Face)m_Face, FT_Get_Char_Index((FT_Face)m_Face, ch), FT_LOAD_DEFAULT)) 
//		throw std::runtime_error("FT_Load_Glyph failed"); 
//
//	//得到字模 
//	FT_Glyph glyph; 
//	if(FT_Get_Glyph(m_Face->glyph, &glyph)) 
//		throw std::runtime_error("FT_Get_Glyph failed"); 
//
//	//转化成位图 
//	FT_Render_Glyph(m_Face->glyph, FT_RENDER_MODE_NORMAL); 
//	FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1); 
//	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph; 
//
//	//取道位图数据 
//	FT_Bitmap& bitmap=bitmap_glyph->bitmap; */
//	
//
//	FT_Load_Char(m_Face, ch, FT_LOAD_RENDER); 
//	FT_Bitmap& bitmap=m_Face->glyph->bitmap;
//
//	//把位图数据拷贝自己定义的数据区里.这样旧可以画到需要的东西上面了。 
//	int width = bitmap.width;//next_p2(bitmap.width); 
//	int height = bitmap.rows;// next_p2(bitmap.rows); 
//
//	for(int j=0; j < height; j++)
//	{ 
//		for(int i=0; i < width; i++) 
//		{ 
//			byte color = (i>=bitmap.width || j>=bitmap.rows) ?
//				0 : bitmap.buffer[i + bitmap.width*j];
//			//color = ARGB(color,color,color,color);
//			pImage[i+x+(10 - m_Face->glyph->bitmap_top+j)*1024] = color;
//		} 
//	}
//	//glyph->bitmap_left
//	//glyph->bitmap_top
//
//	/*
//	// 下面是画文字的调用 
//	FT_GlyphSlot slot = mFace->glyph; 
//
//	for (int n = 0; n  < nCount; n++) 
//	{ 
//	// set transformation 
//	FT_Set_Transform(mFace, &mMatrix, &pen); 
//
//	// load glyph image into the slot (erase previous one) 
//	FT_Load_Char(mFace, lpszText[n], FT_LOAD_RENDER); 
//
//	// 
//	// now, draw to our target texture 
//	// 
//	short xOffset = slot->bitmap_left; 
//	short yOffset = texHeight - slot->bitmap_top; 
//
//	DrawCharToTexture(texture, &slot->bitmap, xOffset, yOffset); 
//
//	/* increment pen position */ 
//	//pen.x += slot->advance.x; 
//	//pen.y += slot->advance.y; 
//
//	x += width+1;
//	y = max(y,height);
//} 

void CTextRender::Free() 
{
	S_DEL(m_pTextTexture);
	FT_Done_Face((FT_Face)m_Face); 
	FT_Done_FreeType((FT_Library)m_library); 
}

void CTextRender::DrawTextVB(int nVertexCount, void* pVB)
{
	CRenderSystem& R = GetRenderSystem();
	R.SetTexture(0, m_pTextTexture);
	R.SetFVF(VERTEX_XYZW_DIF_TEX::FVF);
	R.DrawIndexedPrimitiveUP(VROT_TRIANGLE_LIST, 0, nVertexCount, nVertexCount/2, &s_DrawTextIB, pVB, sizeof(VERTEX_XYZW_DIF_TEX));
	R.SetTexture(0, (CTexture*)NULL);
}

void CTextRender::drawText(const std::wstring& strText,int cchText,const CRect<float>& rcDest,UINT format,Color32 dwColor,CRect<float>* prcRet)
{
	bool bVisible = !(format & DTL_CALCRECT);
	int nX = rcDest.left;
	std::vector<VERTEX_XYZW_DIF_TEX> vb;
	size_t uTextLength = strText.length();
	if (cchText > 0)
	{
		if (uTextLength > cchText)
		{
			uTextLength = cchText;
		}
	}
	for(size_t n = 0 ; n < uTextLength;++n) 
	{ 
		TexCharInfo* charInfo = GetCharInfo(strText[n]);
		if (charInfo)
		{
			nX+=charInfo->nOffsetX;
			if (bVisible && charInfo->nWidth > 0)
			{
				int nX1 = nX+charInfo->nWidth;
				float fSecondU = charInfo->fU1;
				// 超出矩阵范围 截断
				if (0 != rcDest.right && nX1 > rcDest.right)
				{
					fSecondU = charInfo->fU1 - (charInfo->fU1 - charInfo->fU0) * (float) (nX1 - rcDest.right) / (float) charInfo->nWidth;
					nX1 = rcDest.right;
				}
				int nY0 = rcDest.top+m_nH-charInfo->nOffsetY;
				int nY1 = nY0+charInfo->nHeight;
				//if (dwShadowColor.a)
				//{
				//	VERTEX_XYZW_DIF_TEX v11[4]=
				//	{
				//		Vec4D((float)nX+1,	(float)nY0+1, 0.5f, 1.0f), dwShadowColor, Vec2D(charInfo->fU0, charInfo->fV0),
				//		Vec4D((float)nX1+1,	(float)nY0+1, 0.5f, 1.0f), dwShadowColor, Vec2D(fSecondU, charInfo->fV0),
				//		Vec4D((float)nX1+1,	(float)nY1+1, 0.5f, 1.0f), dwShadowColor, Vec2D(fSecondU, charInfo->fV1),
				//		Vec4D((float)nX+1,	(float)nY1+1, 0.5f, 1.0f), dwShadowColor, Vec2D(charInfo->fU0, charInfo->fV1),
				//	};
				//	vb.push_back(v11[0]);vb.push_back(v11[1]);vb.push_back(v11[2]);vb.push_back(v11[3]);
				//}

				VERTEX_XYZW_DIF_TEX v[4]=
				{
					Vec4D((float)nX,	(float)nY0, 0.5f, 1.0f), dwColor, Vec2D(charInfo->fU0, charInfo->fV0),
					Vec4D((float)nX1,	(float)nY0, 0.5f, 1.0f), dwColor, Vec2D(fSecondU, charInfo->fV0),
					Vec4D((float)nX1,	(float)nY1, 0.5f, 1.0f), dwColor, Vec2D(fSecondU, charInfo->fV1),
					Vec4D((float)nX,	(float)nY1, 0.5f, 1.0f), dwColor, Vec2D(charInfo->fU0, charInfo->fV1),
				};
				vb.push_back(v[0]);vb.push_back(v[1]);vb.push_back(v[2]);vb.push_back(v[3]);
			}
			nX += charInfo->nAdvX-charInfo->nOffsetX;
			// 超出矩阵范围 返回
			if (format & DTL_CALCRECT)
			{
			}
			else if (0 != rcDest.right && nX > rcDest.right)
			{
				break;
			}
		}
	}
	if (format > 0)
	{
		// 只需计算矩阵
		if (prcRet)
		{
			*prcRet=rcDest;
			//rcDest.left = 0;
			prcRet->right = nX;
			//rcDest.top = 0;
			prcRet->bottom = rcDest.top+m_nH;
		}
		else
		{
			int nOffsetX = 0;
			int nOffsetY = 0;
			if (format & DTL_CENTER)
			{
				nOffsetX = (rcDest.right - nX)/2;
			}
			else if (format & DTL_RIGHT)
			{
				nOffsetX = rcDest.right - nX;
			}

			if (format & DTL_VCENTER)
			{
				nOffsetY = (rcDest.bottom - rcDest.top - m_nH)/2;
			}
			else if (format & DTL_BOTTOM)
			{
				nOffsetY = rcDest.bottom - rcDest.top - m_nH;
			}
			// update
			for (std::vector<VERTEX_XYZW_DIF_TEX>::iterator it = vb.begin(); it != vb.end(); ++it)
			{
				it->p.x += nOffsetX;
				it->p.y += nOffsetY;
			}
		}

	}
	if (bVisible)
	{
		// IB
		int nVertexCount = vb.size();
		if (nVertexCount)
		{
			DrawTextVB(nVertexCount,&vb[0]);
		}		
	}
}

void CTextRender::drawText(const std::wstring& strText, int x, int y, Color32 dwColor)
{
	RECT rc = {x, y, 0, 0};
	drawText(strText, -1, rc, DT_NOCLIP, dwColor);
}

void CTextRender::drawText(const std::string& strText)
{
	//GetTextRender().drawText(s2ws(strText));
}

void CTextRender::drawText(const std::wstring& strText)
{
	drawText(strText, m_nX, m_nY, m_dwColor);
	m_nY += m_nLineHeight;
}

void CTextRender::BuildUBB(CUBB* pUBB, const std::wstring& strText, const RECT& rc)
{
	// 初始化
	pUBB->Init(rc, m_nH);
	size_t uTextLength = strText.length();
	for(size_t n = 0 ; n < uTextLength;++n) 
	{
		// 标签判断中
		if (strText[n] == L'[')
		{
			size_t pos = strText.find(L']',n);
			std::wstring  wstrTag = strText.substr(n+1,pos-n-1);
			n=pos;
			pUBB->ParseTag(wstrTag);
		}
		else // 处理字
		{
			TexCharInfo* charInfo = GetCharInfo(strText[n]);
			if (charInfo)
			{
				pUBB->AddChar(charInfo);
			}
		}
	}
	// 针对最后一行文本
	pUBB->UpdateTextLine();
}

void CTextRender::DrawUBB(CUBB* pUBB)
{
	int nVertexCount = pUBB->m_VB.size();
	if (nVertexCount)
	{
		DrawTextVB(nVertexCount,&pUBB->m_VB[0]);
	}
}

void CTextRender::DrawUBB(const std::wstring& strText, const RECT& rc)
{
	CUBB ubb;
	BuildUBB(&ubb, strText, rc);
	DrawUBB(&ubb);
}

CUBB::CUBB()
{
}

CUBB::~CUBB()
{
}

void CUBB::Init(const RECT& rc, int nFontSize)
{
	m_nFontSize = nFontSize;

	m_nBold = 0;
	m_nItalic = 0;
	m_bUnderline = false;
	m_nAlignType = ALIGN_TYPE_LEFT;
	m_bSup = false;
	m_bSub = false;
	m_bFlipH = false;
	m_bFlipV = false;
	m_dwColor = 0xFFFFFFFF;
	m_nSize = m_nFontSize;
	m_TagTypeRecords[TAG_TYPE_BOLD].push_back(m_nBold);
	m_TagTypeRecords[TAG_TYPE_ITALIC].push_back(m_nItalic);
	m_TagTypeRecords[TAG_TYPE_UNDERLINE].push_back(m_bUnderline);
	m_TagTypeRecords[TAG_TYPE_BR].push_back(0);
	m_TagTypeRecords[TAG_TYPE_ALIGN].push_back(m_nAlignType);
	m_TagTypeRecords[TAG_TYPE_URL].push_back(0);
	m_TagTypeRecords[TAG_TYPE_SUP].push_back(m_bSup);
	m_TagTypeRecords[TAG_TYPE_SUB].push_back(m_bSub);
	m_TagTypeRecords[TAG_TYPE_FLIPH].push_back(m_bFlipH);
	m_TagTypeRecords[TAG_TYPE_FLIPV].push_back(m_bFlipV);
	m_TagTypeRecords[TAG_TYPE_COLOR].push_back(m_dwColor);
	m_TagTypeRecords[TAG_TYPE_SIZE].push_back(m_nSize);

	m_nShowLeft = rc.left;
	m_nShowTop = rc.top;
	m_nShowWidth = rc.right - rc.left;
	m_nShowHeight = rc.bottom - rc.top;

	m_nTextHeight = 0;

	m_nLineWidth = 0;
	m_nLineHeight = 0;

	m_nLineBegin = 0;
	m_nLineEnd = 0;

	m_VB.clear();
	m_IB.clear();
}

void CUBB::ParseTag(const std::wstring& wstrTag)
{
	if (wstrTag.length()==0)
	{
		return;
	}
	//return;
	int n = 0;
	bool bEnd =false;
	// 是否为截至符
	if (wstrTag[0] == L'/')
	{
		n++;
		bEnd =true;
	}
	std::wstring wstrTagName;// 标签名字
	std::vector<std::wstring> setParameters;

	size_t pos = wstrTag.find('=');
	if (std::wstring::npos==pos)
	{
		wstrTagName = wstrTag.substr(n);
	}
	else
	{
		wstrTagName = wstrTag.substr(n,pos-n);
		std::wstring wstrParameters = wstrTag.substr(pos+1);
		TokenizerW(wstrParameters,setParameters);
	}

	int nTagType = -1;
	// 标签解析
	for (int i=0; i<TAG_TYPE_MAX; ++i)
	{
		if (gs_UBB_TAG_TYPE_NAME[i]==wstrTagName)
		{
			nTagType = i;
			break;
		}
	}
	if (nTagType !=-1)
	{
		if (bEnd) // 回退
		{
			int nRecordCount = m_TagTypeRecords[nTagType].size()-1;
			if (nRecordCount > 0) // 防止为空
			{
				m_TagTypeRecords[nTagType].resize(nRecordCount);
				nRecordCount--;
				switch(nTagType)
				{
				case TAG_TYPE_BOLD:
					m_nBold = m_TagTypeRecords[nTagType][nRecordCount];
					break;
				case TAG_TYPE_ITALIC:
					m_nItalic = m_TagTypeRecords[nTagType][nRecordCount];
					break;
				case TAG_TYPE_UNDERLINE:
					break;
				case TAG_TYPE_BR:
					// 不做处理
					break;
				case TAG_TYPE_ALIGN:
					// 进入换行移位啦！
					UpdateTextLine();
					m_nAlignType = m_TagTypeRecords[nTagType][nRecordCount];
					break;
				case TAG_TYPE_URL:
					break;
				case TAG_TYPE_SUP:
					break;
				case TAG_TYPE_SUB:
					break;
				case TAG_TYPE_FLIPH:
					m_bFlipH = m_TagTypeRecords[nTagType][nRecordCount]!=0;
					break;
				case TAG_TYPE_FLIPV:
					m_bFlipV = m_TagTypeRecords[nTagType][nRecordCount]!=0;
					break;
				case TAG_TYPE_IMG:
					break;
				case TAG_TYPE_FLY:
					break;
				case TAG_TYPE_MOVE:
					break;
				case TAG_TYPE_GLOW:
					break;
				case TAG_TYPE_SHADOW:
					break;
				case TAG_TYPE_COLOR:
					m_dwColor = m_TagTypeRecords[nTagType][nRecordCount];
					break;
				case TAG_TYPE_SIZE:
					m_nSize = m_TagTypeRecords[nTagType][nRecordCount];
					break;
				case TAG_TYPE_FACE:
					break;
				case TAG_TYPE_DIR:
					break;
				case TAG_TYPE_RM:
					break;
				default:
					break;
				}
			}
		}
		else
		{
			switch(nTagType)
			{
			case TAG_TYPE_BOLD:
				{
					if (setParameters.size()==0)
					{
						m_nBold = 1;
					}
					else if (setParameters.size()>=1)
					{
						m_nBold = _wtoi(setParameters[0].c_str());
					}
					m_TagTypeRecords[nTagType].push_back(m_nBold);
				}
				break;
			case TAG_TYPE_ITALIC:
				{
					if (setParameters.size()==0)
					{
						m_nItalic = 3;
					}
					else if (setParameters.size()>=1)
					{
						m_nItalic = _wtoi(setParameters[0].c_str());
					}
					m_TagTypeRecords[nTagType].push_back(m_nItalic);
				}
				break;
			case TAG_TYPE_UNDERLINE:
				break;
			case TAG_TYPE_BR:
				{
					// 进入换行移位啦！
					UpdateTextLine();
				}
				break;
			case TAG_TYPE_ALIGN:
				{
					// 进入换行移位啦！
					UpdateTextLine();
					if (setParameters.size()>=1)
					{
						if (setParameters[0]==L"center")
						{
							m_nAlignType = ALIGN_TYPE_CENTER;
						}
						else if (setParameters[0]==L"left")
						{
							m_nAlignType = ALIGN_TYPE_LEFT;
						}
						else if (setParameters[0]==L"right")
						{
							m_nAlignType = ALIGN_TYPE_RIGHT;
						}
					}
					m_TagTypeRecords[nTagType].push_back(m_nAlignType);
				}
				break;
			case TAG_TYPE_URL:
				break;
			case TAG_TYPE_SUP:
				break;
			case TAG_TYPE_SUB:
				break;
			case TAG_TYPE_FLIPH:
				{
					m_bFlipH = true;
					m_TagTypeRecords[nTagType].push_back(m_bFlipH);

				}
				break;
			case TAG_TYPE_FLIPV:
				{
					m_bFlipV = true;
					m_TagTypeRecords[nTagType].push_back(m_bFlipV);
				}
				break;
			case TAG_TYPE_IMG:
				break;
			case TAG_TYPE_FLY:
				break;
			case TAG_TYPE_MOVE:
				break;
			case TAG_TYPE_GLOW:
				break;
			case TAG_TYPE_SHADOW:
				break;
			case TAG_TYPE_COLOR:
				{
					if (setParameters.size()==1)
					{
					}
					else if (setParameters.size()==3)
					{
						byte r = max(min(_wtoi(setParameters[0].c_str()), 255), 0);
						byte g = max(min(_wtoi(setParameters[1].c_str()), 255), 0);
						byte b = max(min(_wtoi(setParameters[2].c_str()), 255), 0);
						m_dwColor = (255<<24)+(r<<16)+(g<<8)+b;
					}
					else if (setParameters.size()==4)
					{
						byte a = max(min(_wtoi(setParameters[0].c_str()), 255), 0);
						byte r = max(min(_wtoi(setParameters[1].c_str()), 255), 0);
						byte g = max(min(_wtoi(setParameters[2].c_str()), 255), 0);
						byte b = max(min(_wtoi(setParameters[3].c_str()), 255), 0);
						m_dwColor = (a<<24)+(r<<16)+(g<<8)+b;
					}
					m_TagTypeRecords[nTagType].push_back(m_dwColor);
				}
				break;
			case TAG_TYPE_SIZE:
				{
					if (setParameters.size()>=1)
					{
						m_nSize = _wtoi(setParameters[0].c_str());
					}
					m_TagTypeRecords[nTagType].push_back(m_nSize);
				}
				break;
			case TAG_TYPE_FACE:
				break;
			case TAG_TYPE_DIR:
				break;
			case TAG_TYPE_RM:
				break;
			default:
				break;
			}
		}
	}
}


void CUBB::AddChar(TexCharInfo* charInfo)
{
	float fScaling = (float)m_nSize / (float)m_nFontSize;
	float fCharWidth = (charInfo->nWidth+charInfo->nOffsetX+m_nBold) * fScaling;/*abs(nItalic)*/;
	if (m_nLineHeight < m_nSize)
	{
		m_nLineHeight = m_nSize;
	}

	if (m_nShowWidth>0)
	{
		if (m_nLineWidth + fCharWidth > m_nShowWidth)
		{
			// 进入换行移位啦！
			UpdateTextLine();
		}
	}
	//m_nLineWidth+=(int)(charInfo->nOffsetX * fScaling);

	float fX0 = (float)m_nLineWidth+charInfo->nOffsetX * fScaling;
	float fX1 = m_nLineWidth+fCharWidth;
	float fY0 = -charInfo->nOffsetY * fScaling;
	float fY1 = fY0 + charInfo->nHeight * fScaling;

	// 非空白
	if (charInfo->nWidth > 0)
	{
		float u0,v0,u1,v1;
		if (m_bFlipH)
		{
			u0 = charInfo->fU1;
			u1 = charInfo->fU0;
		}
		else
		{
			u0 = charInfo->fU0;
			u1 = charInfo->fU1;
		}
		if (m_bFlipV)
		{
			v0 = charInfo->fV1;
			v1 = charInfo->fV0;
		}
		else
		{
			v0 = charInfo->fV0;
			v1 = charInfo->fV1;
		}
		VERTEX_XYZW_DIF_TEX v[4]=
		{
			Vec4D(fX0+(float)m_nItalic,fY0, 0.5f, 1.0f), m_dwColor, Vec2D(u0, v0),
			Vec4D(fX1+(float)m_nItalic,fY0, 0.5f, 1.0f), m_dwColor, Vec2D(u1, v0),
			Vec4D(fX1,			fY1, 0.5f, 1.0f), m_dwColor, Vec2D(u1, v1),
			Vec4D(fX0,			fY1, 0.5f, 1.0f), m_dwColor, Vec2D(u0, v1),
		};
		m_VB.push_back(v[0]);m_VB.push_back(v[1]);m_VB.push_back(v[2]);m_VB.push_back(v[3]);
	}
	m_nLineWidth += charInfo->nAdvX*fScaling;
}

void CUBB::UpdateTextLine()
{
	m_nLineEnd = m_VB.size();
	if (m_nLineBegin < m_nLineEnd)
	{
		// 文本高度设置
		m_nTextHeight += int(m_nLineHeight*1.5f);
		m_nLineHeight = 0;
		int nOffsetX = m_nShowLeft;
		// 文本对其设置
		if (m_nAlignType == ALIGN_TYPE_CENTER)
		{
			nOffsetX += (m_nShowWidth-m_nLineWidth)/2;
		}
		else if (m_nAlignType == ALIGN_TYPE_RIGHT)
		{
			nOffsetX += m_nShowWidth-m_nLineWidth;
		}
		for (int i = m_nLineBegin; i < m_nLineEnd; i++)
		{
			m_VB[i].p.x += nOffsetX;
			m_VB[i].p.y += m_nTextHeight+m_nShowTop;
		}
		// 下一行为行宽度为0
		m_nLineWidth = 0;
		// begin重置
		m_nLineBegin = m_nLineEnd;
	}
}