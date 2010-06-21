#include "UBB.h"
#include "RenderSystem.h"
#include "Vec4D.h"
#include "Vec3D.h"

//#include "FileSystem.h"
#include "IORead.h"

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

	m_nMaxWidth = 0;
	m_nMaxHeight = 0;

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
					if (setParameters.empty())
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
					if (setParameters.empty())
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
#ifdef _3D_TEXT
		VERTEX_XYZ_DIF_TEX v[4]=
		{
			Vec3D(fX0+(float)m_nItalic,fY0, 0.0f), m_dwColor, Vec2D(u0, v0),
			Vec3D(fX1+(float)m_nItalic,fY0, 0.0f), m_dwColor, Vec2D(u1, v0),
			Vec3D(fX1,			fY1, 0.0f), m_dwColor, Vec2D(u1, v1),
			Vec3D(fX0,			fY1, 0.0f), m_dwColor, Vec2D(u0, v1),
		};
#else
		VERTEX_XYZW_DIF_TEX v[4]=
		{
			Vec4D(fX0+(float)m_nItalic,fY0, 0.5f, 1.0f), m_dwColor, Vec2D(u0, v0),
			Vec4D(fX1+(float)m_nItalic,fY0, 0.5f, 1.0f), m_dwColor, Vec2D(u1, v0),
			Vec4D(fX1,			fY1, 0.5f, 1.0f), m_dwColor, Vec2D(u1, v1),
			Vec4D(fX0,			fY1, 0.5f, 1.0f), m_dwColor, Vec2D(u0, v1),
		};
#endif
		m_VB.push_back(v[0]);m_VB.push_back(v[1]);m_VB.push_back(v[2]);m_VB.push_back(v[3]);
	}
	m_nLineWidth += charInfo->nAdvX*fScaling;
	m_nMaxWidth = max(m_nMaxWidth,m_nLineWidth);
}

void CUBB::UpdateTextLine()
{
	m_nLineEnd = m_VB.size();
	if (m_nLineBegin < m_nLineEnd)
	{
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
			m_VB[i].p.y += m_nMaxHeight+m_nLineHeight+m_nShowTop;
		}
		// 文本高度设置
		m_nMaxHeight += int(m_nLineHeight*1.5f);
		// 下一行为行宽度为0
		m_nLineWidth = 0;
		m_nLineHeight = 0;
		// begin重置
		m_nLineBegin = m_nLineEnd;
	}
}

RECT CUBB::getRect()
{
	RECT rc;
	rc.left = m_nShowLeft;
	rc.top = m_nShowTop;
	rc.right = rc.left+m_nMaxWidth;
	rc.bottom = rc.top+m_nMaxHeight;
	return rc;
}