#include "TerrainData.h"
#include <fstream>
//#include "tinyxml.h"
#include "interpolation.h"
#include "IORead.h"
#include "Intersect.h"

#define		MAX_PICK_COUNT	200
CTerrainData::CTerrainData():
m_nWidth(0),
m_nHeight(0),
m_nCubeSize(8),
m_nCellCount(0),
m_posTileCount(0,0),
m_nVertexXCount(0),
m_nVertexYCount(0),
m_nVertexCount(0),
m_vLightDir(0,-1,0)
{
	m_vLightDir = Vec3D(-1,-1,-1).normalize();
}

CTerrainData::~CTerrainData()
{
}

void CTerrainData::clear()
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_nCubeSize = 0;
	m_nCellCount = 0;
	m_nVertexXCount = 0;
	m_nVertexYCount = 0;
	m_nVertexCount = 0;
	m_Cells.clear();
}

void CTerrainData::Create(size_t width, size_t height, size_t cubeSize)
{
	clear();
	resize(width,height,cubeSize);
}
template <class T>
inline void resizeVector(std::vector<T>& v, size_t srcWidth, size_t srcHeight,size_t destWidth, size_t destHeight,T val=0)
{
	std::vector<T> srcV=v;
	v.resize(destWidth*destHeight,val);
	if (srcV.empty())
	{
		return;
	}
	size_t width = min(srcWidth,destWidth);
	size_t height = min(srcHeight,destHeight);
	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
		{
			v[y*destWidth+x] = srcV[y*srcWidth+x];
		}
	}
}

bool CTerrainData::resize(size_t width, size_t height, size_t cubeSize)
{
	if (cubeSize==0)
	{
		cubeSize = 8;
		for (size_t i=8; i<30; ++i)
		{
			if (width%i==0&&width%i==0)
			{
				cubeSize = i;
				break;
			}
		}
	}
	width=(width/cubeSize)*cubeSize;
	height=(height/cubeSize)*cubeSize;

	if (65536<(width+1)*(height+1))
	{
		return false;
	}
	m_nWidth = width;
	m_nHeight = height;
	m_nCubeSize = cubeSize;
	m_nCellCount = m_nWidth * m_nHeight;
	m_nVertexXCount = m_nWidth + 1;
	m_nVertexYCount = m_nHeight + 1;
	m_nVertexCount = m_nVertexXCount * m_nVertexYCount;

	m_Cells.resize(m_nVertexCount);
// 	resizeVector(m_TileID[0],m_nWidth,m_nHeight,width,height);
// 	resizeVector(m_TileID[1],m_nWidth,m_nHeight,width,height,(int8)255);
// 	resizeVector(m_Normals,m_nWidth+1,m_nHeight+1,width+1,height+1,Vec3D(0.0f,0.0f,0.0f));
// 	resizeVector(m_Colors,m_nWidth+1,m_nHeight+1,width+1,height+1,Color32(255,255,255,255));
// 	resizeVector(m_LightColors,m_nWidth+1,m_nHeight+1,width+1,height+1,Color32(255,255,255,255));
// 	resizeVector(m_Height,m_nWidth+1,m_nHeight+1,width+1,height+1);
// 	resizeVector(m_Attribute,m_nWidth,m_nHeight,width,height);
// 	resizeVector(m_Other,m_nWidth,m_nHeight,width,height);
// 	resizeVector(m_Searched,m_nWidth,m_nHeight,width,height,false);
	return true;
}

TerrainCell* CTerrainData::getCell(int x, int y)
{
	int nCellIndex = y*m_nVertexXCount+x;
	return &m_Cells[nCellIndex];
}

const TerrainCell* CTerrainData::getCell(int x, int y)const
{
	int nCellIndex = y*m_nVertexXCount+x;
	return &m_Cells[nCellIndex];
}

float CTerrainData::GetCellXLength(const Pos2D& posCell)const
{
	int nCellIndex = GetVertexIndex(posCell);
	//TerrainCell& cell=m_Cells[nCellIndex];
	float fOffsetHeight = m_Cells[nCellIndex+1].fHeight-m_Cells[nCellIndex].fHeight;
	return Vec2D(1,fOffsetHeight).length();
}

float CTerrainData::GetCellYLength(const Pos2D& posCell)const
{
	int nCellIndex = GetVertexIndex(posCell);
	float fOffsetHeight = m_Cells[nCellIndex+GetVertexXCount()].fHeight-m_Cells[nCellIndex].fHeight;
	return Vec2D(1,fOffsetHeight).length();
}

float CTerrainData::GetXLength(int nCellY)const
{
	float fLength = 0;
	for (int x = 0; x < m_nWidth; x++)
	{
		//fLength += GetCellXLength(x,nCellY);
	}
	return fLength;
}

float CTerrainData::GetYLength(int nCellX)const
{
	float fLength = 0;
	for (int y = 1; y < m_nHeight; y++)
	{
		//fLength += GetCellYLength(nCellX,y);
	}
	return fLength;
}

void CTerrainData::CalcTexUV()
{
	//m_EquableTexUV.clear();
	//m_EquableTexUV.resize(GetVertexCount());
	//for (int y = 0; y < m_nVertexYCount; y++)
	//{
	//	float fLength = GetXLength(y);
	//	m_EquableTexUV[GetVertexIndex(0,y)].x = 0;
	//	for (int x = 1; x < m_nVertexXCount; x++)
	//	{
	//		m_EquableTexUV[GetVertexIndex(x,y)].x = 
	//			m_EquableTexUV[GetVertexIndex(x-1,y)].x+
	//			GetCellXLength(x-1,y)*m_nWidth/fLength;
	//	}
	//}

	//for (int x = 0; x < m_nVertexXCount; x++)
	//{
	//	float fLength = GetYLength(x);
	//	m_EquableTexUV[GetVertexIndex(x,0)].y = 0;
	//	for (int y = 1; y < m_nVertexYCount; y++)
	//	{
	//		m_EquableTexUV[GetVertexIndex(x,y)].y = 
	//			m_EquableTexUV[GetVertexIndex(x,y-1)].y+
	//			GetCellYLength(x,y-1)*m_nHeight/fLength;
	//	}
	//}
}

void CTerrainData::getVertexByCell(int nCellX, int nCellY, TerrainVertex& vertex)const
{
	const TerrainCell* cell = getCell(nCellX,nCellY);
	vertex.p = Vec3D((float)nCellX, cell->fHeight, (float)nCellY);
	vertex.n = cell->vNormals;

	/*Vec3D vBinormal = vertex.n.cross(Vec3D(0,0,1)).normalize();
	Vec3D vTangent =  Vec3D(1,0,0).cross(vertex.n).normalize();
	Matrix mTangent;
	mTangent.Zero();
	mTangent._11=vBinormal.x;
	mTangent._21=vBinormal.y;
	mTangent._31=vBinormal.z;

	mTangent._12=vertex.n.x;
	mTangent._22=vertex.n.y;
	mTangent._32=vertex.n.z;

	mTangent._13=vTangent.x;
	mTangent._23=vTangent.y;
	mTangent._33=vTangent.z;

	vertex.n = mTangent*GetLightDir();
	vertex.n=vertex.n.normalize();*/

	vertex.c = cell->color;
	//vertex.t = m_EquableTexUV[nCellIndex];
	vertex.t0.x = (float)nCellX;
	vertex.t0.y = (float)nCellY;
	vertex.t1.x = (float)nCellX/(float)GetWidth();
	vertex.t1.y = (float)nCellY/(float)GetHeight();
}

void CTerrainData::getGrassVertexByCell(int nCellX, int nCellY, TerrainVertex*& vertex)const
{
	const TerrainCell* cell1 = getCell(nCellX,nCellY);
	const TerrainCell* cell2 = getCell(nCellX+1,nCellY+1);
	int	nRand = (((nCellY*m_nVertexXCount+nCellX+nCellX*nCellY)*214013L+2531011L)>>16)&0x7fff;   
	float fTexU = (nRand%4)*0.25f;
	vertex[0].p = Vec3D((float)nCellX, cell1->fHeight, (float)nCellY);
	//vertex[0].n = Vec3DGetCellNormal(posCell1);
	vertex[0].c = cell1->color;
	vertex[0].t0 = Vec2D(fTexU,1.0f);

	vertex[1].p = vertex[0].p+Vec3D(0.0f,1.5f,0.0f);
	//vertex[1].n = vertex[0].n;
	vertex[1].c = vertex[0].c;
	vertex[1].t0 = Vec2D(fTexU,0.0f);

	vertex[3].p = Vec3D((float)(nCellX+1), cell2->fHeight, (float)(nCellY+1));
	//vertex[3].n = GetCellNormal(posCell2);
	vertex[3].c = cell2->color;
	vertex[3].t0 = Vec2D(fTexU+0.25f,1.0f);

	vertex[2].p = vertex[3].p+Vec3D(0.0f,1.5f,0.0f);
	//vertex[2].n = vertex[3].n;
	vertex[2].c = vertex[3].c;
	vertex[2].t0 = Vec2D(fTexU+0.25f,0.0f);
}

bool CTerrainData::hasGrass(int nCellX, int nCellY)const
{
	Pos2D posCell=Pos2D(nCellX,nCellY);
	const uint8 uAttribute = getCellAttribute(posCell);
	return 0==GetCellTileID(posCell,0)&&255==GetCellTileID(posCell,1)&&0==(uAttribute&0x8);
}

bool CTerrainData::PickCell(const Pos2D& posCell, const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D* pPos)const
{
	if (isCellIn(posCell))
	{
		int index = GetVertexIndex(posCell);
		Vec3D v0((float)(posCell.x),	m_Cells[index].fHeight,					(float)(posCell.y));
		Vec3D v1((float)(posCell.x),	m_Cells[index+m_nVertexXCount].fHeight,	(float)(posCell.y+1));
		Vec3D v2((float)(posCell.x+1),	m_Cells[index+m_nVertexXCount+1].fHeight,(float)(posCell.y+1));
		Vec3D v3((float)(posCell.x+1),	m_Cells[index+1].fHeight,				(float)(posCell.y));
		Vec3D vOut;
		if(IntersectTri(v1,v2,v0,vRayPos,vRayDir,vOut))
		{
			if (pPos)
			{
				*pPos = vOut;
			}
			return true;
		}
		else if (IntersectTri(v3,v0,v2,vRayPos,vRayDir,vOut))
		{
			if (pPos)
			{
				*pPos = vOut;
			}
			return true;
		}
	}
	return false;
}

bool CTerrainData::Pick(const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D* pPos)const
{
	// 格子地图的快速PICK
	int nIncX = vRayDir.x>0?1:-1;
	int nIncY = vRayDir.z>0?1:-1;
	float fK = vRayDir.z/vRayDir.x;
	float fB = vRayPos.z - fK*vRayPos.x;
	int nX = int(vRayPos.x)-nIncX;
	int nY = int(vRayPos.z)-nIncY;

	Pos2D posCell;
	if (fK<1&&fK>-1)
	{
		for (int i = 0; i < MAX_PICK_COUNT; i++)
		{
			posCell.x = nX + nIncX;
			posCell.y = (int)(fK*(float)posCell.x+fB);
			if (posCell.y!=nY)
			{
				nY = posCell.y;
				posCell.x = nX - (nIncX<0);
				posCell.y = nY - nIncY*(nIncX<0);
			}
			else
			{
				nX = posCell.x;
			}
			if (PickCell(posCell, vRayPos, vRayDir, pPos))
			{
				return true;
			}
		}
	}
	else
	{
		for (int i = 0; i < MAX_PICK_COUNT; i++)
		{
			posCell.y = nY + nIncY;
			posCell.x = (int)(((float)posCell.y-fB)/fK);
			if (posCell.x!=nX)
			{
				nX = posCell.x;
				posCell.x = nX - nIncX*(nIncY<0);
				posCell.y = nY - (nIncY<0);
			}
			else
			{
				nY = posCell.y;
			}
			if (PickCell(posCell, vRayPos, vRayDir, pPos))
			{
				return true;
			}
		}
	}
	return false;
}

uint32 CTerrainData::GetVertexIndex(const Pos2D& posCell)const
{
	return posCell.y*m_nVertexXCount + posCell.x;
}

int CTerrainData::GetCellXByVertexIndex(uint32 uVertexIndex)const
{
	return uVertexIndex%m_nVertexXCount;
}

int CTerrainData::GetCellYByVertexIndex(uint32 uVertexIndex)const
{ 
	return uVertexIndex/m_nVertexXCount;
}

Pos2D CTerrainData::GetCellPosByVertexIndex(uint32 uVertexIndex)const
{
	return Pos2D(uVertexIndex%m_nVertexXCount,uVertexIndex/m_nVertexXCount);
}

bool CTerrainData::isCellIn(const Pos2D& posCell)const
{
	if (posCell.x>=0 && posCell.x<m_nWidth && posCell.y>=0 && posCell.y<m_nHeight)
	{
		return true;
	}
	return false;
}

bool CTerrainData::isPointIn(const Pos2D& posCell)const
{
	if (posCell.x>=0 && posCell.x<m_nVertexXCount && posCell.y>=0 && posCell.y<m_nVertexYCount)
	{
		return true;
	}
	return false;
}

uint8 CTerrainData::GetCellTileID(const Pos2D& posCell, size_t layer)const
{
	if (isCellIn(posCell))
	{
		if (2>layer)
		{
			return m_Cells[GetVertexIndex(posCell)].uTileID[layer];
		}
	}
	return 0;
}

void CTerrainData::SetCellTileID(const Pos2D& posCell, uint8 uTileID, size_t layer)
{
	if (isCellIn(posCell))
	{
		if (2>layer)
		{
			m_Cells[GetVertexIndex(posCell)].uTileID[layer] = uTileID;
		}
	}
}

uint8 CTerrainData::getCellAttribute(const Pos2D& posCell)const
{
	if (isCellIn(posCell))
	{
		return m_Cells[GetVertexIndex(posCell)].uAttribute;
	}
	return 0;
}

void CTerrainData::setCellAttribute(const Pos2D& posCell, uint8 uAtt)
{
	if (isCellIn(posCell))
	{
		m_Cells[GetVertexIndex(posCell)].uAttribute = uAtt;
	}
}

bool CTerrainData::isCellSearched(const Pos2D& posCell)const
{
	if (isCellIn(posCell))
	{
		return m_Cells[GetVertexIndex(posCell)].bSearched;
	}
	return true;
}

void CTerrainData::setCellSearched(const Pos2D& posCell, bool bSearched)
{
	if (isCellIn(posCell))
	{
		m_Cells[GetVertexIndex(posCell)].bSearched = bSearched;
	}
}

float CTerrainData::getVertexHeight(const Pos2D& posCell)const
{
	if (isPointIn(posCell))
	{
		return m_Cells[GetVertexIndex(posCell)].fHeight;
	}
	return 0.0f;
}

void CTerrainData::setVertexHeight(const Pos2D& posCell, float fHeight)
{
	if (isPointIn(posCell))
	{
		m_Cells[GetVertexIndex(posCell)].fHeight = fHeight;
	}
}

Vec3D CTerrainData::getVertexNormal(const Pos2D& posCell)const
{
	float a = getVertexHeight(posCell);
	float b = getVertexHeight(posCell+Pos2D(0,1));
	float c = getVertexHeight(posCell+Pos2D(1,0));
	Vec3D vVector0(0,(b-a),1);
	Vec3D vVector1(1,(c-a),0);
	Vec3D vN = vVector0.cross(vVector1);
	return vN.normalize();
}

Color32 CTerrainData::getVertexColor(const Pos2D& posCell)const
{
	if (isPointIn(posCell))
	{
		return m_Cells[GetVertexIndex(posCell)].color;
	}
	return 0xFFFFFFFF;
}

void CTerrainData::setVertexColor(const Pos2D& posCell, Color32 color)
{
	if (isPointIn(posCell))
	{
		m_Cells[GetVertexIndex(posCell)].color = color;
	}
}

float CTerrainData::GetHeight(const Vec2D& vPos)const
{
	Pos2D posCell(int(vPos.x),int(vPos.y));
	float u = vPos.x - posCell.x;
	float v = vPos.y - posCell.y;
	float a = getVertexHeight(posCell);
	float b = getVertexHeight(posCell+Pos2D(1,0));
	float c = getVertexHeight(posCell+Pos2D(0,1));
	float d = getVertexHeight(posCell+Pos2D(1,1));
	return bilinearInterpolation(a,b,c,d,u,v);
}

void CTerrainData::SetHeight(const Vec2D& vPos, float fHeight)
{
}

Vec4D CTerrainData::GetColor(const Vec2D& vPos)const
{
	Pos2D posCell(int(vPos.x),int(vPos.y));
	float u = vPos.x - posCell.x;
	float v = vPos.y - posCell.y;
	Vec4D a = getVertexColor(posCell);
	Vec4D b = getVertexColor(posCell+Pos2D(1,0));
	Vec4D c = getVertexColor(posCell+Pos2D(0,1));
	Vec4D d = getVertexColor(posCell+Pos2D(1,1));
	return bilinearInterpolation(a,b,c,d,u,v);
}