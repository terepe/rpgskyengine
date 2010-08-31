#pragma once
#include "InterfaceScene.h"
#include "LumpFile.h"

//struct SceneObjectInfo
//{
//	uint32	uObjectID;
//	Vec3D	vPos;
//	float	fScale;
//	float	fYawAngle;
//	float	fPitchAngle;
//	uint32	uSkinID;
//};

enum E_TERRAIN_ATT_TYPE
{
	TERRAIN_ATT_TYPE_SAFE	= 1<<0,
	TERRAIN_ATT_TYPE_BALK	= 1<<2,
	TERRAIN_ATT_TYPE_CLEAR	= 1<<3,
};

// 地图文件数据
class CTerrainData:public iTerrainData
{
public:
	CTerrainData(); 
	~CTerrainData();

	//
	void clear();
	void create(size_t width, size_t height, size_t cubeSize);
	bool resize(size_t width, size_t height, size_t cubeSize);
	//
	int		GetVertexXCount()const{ return m_nVertexXCount; }
	int		GetVertexYCount()const{ return m_nVertexYCount; }
	int		GetVertexCount()const{ return m_nVertexCount; }
	//
	int		GetWidth()const{ return m_nWidth; }
	int		GetHeight()const{ return m_nHeight; }
	int		GetCubeSize()const{ return m_nCubeSize; }
	int		GetCellCount()const{ return m_nCellCount; }
	//
	bool	isCellIn(int nCellX, int nCellY)const;
	bool	isPointIn(int nCellX, int nCellY)const;
	//
	TerrainCell* getCell(int x, int y);
	const TerrainCell* getCell(int x, int y)const;
	//
	uint8	GetCellTileID(int nCellX, int nCellY, size_t layer = 0)const;
	void	SetCellTileID(int nCellX, int nCellY, uint8 uTileID, size_t layer = 0);
	//
	uint32	GetVertexIndex(int nCellX, int nCellY)const;
	int		GetCellXByVertexIndex(uint32 uVertexIndex)const;
	int		GetCellYByVertexIndex(uint32 uVertexIndex)const;
	Pos2D	GetCellPosByVertexIndex(uint32 uVertexIndex)const;
	//
	float	getVertexHeight(int nCellX, int nCellY)const;
	void	setVertexHeight(int nCellX, int nCellY, float fHeight);
	//
	Vec3D	getVertexNormal(int nCellX, int nCellY)const;
	//
	uint8	getCellAttribute(int nCellX, int nCellY)const;
	void	setCellAttribute(int nCellX, int nCellY, uint8 uAtt);
	//
	bool	isCellSearched(int nCellX, int nCellY)const;
	void	setCellSearched(int nCellX, int nCellY, bool bSearched);
	//
	Color32	getVertexColor(int nCellX, int nCellY)const;
	void	setVertexColor(int nCellX, int nCellY, Color32 color);
	//
	float	GetHeight(const Vec2D& vPos)const;
	Vec4D	GetColor(const Vec2D& vPos)const;
	// 碰撞精选
	virtual bool PickCell(int nCellX, int nCellY, const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D* pPos = NULL)const;
	virtual bool Pick(const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D* pPos = NULL)const;
	//
	const std::string& getFilename()const{return m_strFilename;}
	//
	Vec3D		GetLightDir()const{return m_vLightDir;}

	void	getVertexByCell(int nCellX, int nCellY, TerrainVertex& vertex)const;
	void	getGrassVertexByCell(int nCellX, int nCellY, TerrainVertex*& vertex)const;

	bool	hasGrass(int nCellX, int nCellY)const;

	std::vector<TerrainCell>&		getCells()	{return m_Cells;}
protected:
	std::string	m_strFilename;
	char		m_szName[32];
	int			m_nWidth;
	int			m_nHeight;
	int			m_nCubeSize;
	int			m_nCellCount;

	Pos2D		m_posTileCount;

	int			m_nVertexXCount;
	int			m_nVertexYCount;
	int			m_nVertexCount;

	Vec3D		m_vLightDir;

	std::vector<TerrainCell>	m_Cells;

	uint16					m_uMuFlgMap;
	uint32					m_uMuFlgAtt;

};