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
	void Create(size_t width, size_t height, size_t cubeSize);
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
	bool	isCellIn(const Pos2D& posCell)const;
	bool	isPointIn(const Pos2D& posCell)const;
	//
	uint8	GetCellTileID(const Pos2D& posCell, int nLayer = 0)const;
	void	SetCellTileID(const Pos2D& posCell, uint8 uTileID, int nLayer = 0);
	//
	uint32	GetVertexIndex(const Pos2D& posCell)const;
	int		GetCellXByVertexIndex(uint32 uVertexIndex)const;
	int		GetCellYByVertexIndex(uint32 uVertexIndex)const;
	Pos2D	GetCellPosByVertexIndex(uint32 uVertexIndex)const;
	//
	float	getVertexHeight(const Pos2D& posCell)const;
	void	setVertexHeight(const Pos2D& posCell, float fHeight);
	//
	Vec3D	getVertexNormal(const Pos2D& posCell)const;
	//
	uint8	getCellAttribute(const Pos2D& posCell)const;
	void	setCellAttribute(const Pos2D& posCell, uint8 uAtt);
	//
	bool	isCellSearched(const Pos2D& posCell)const;
	void	setCellSearched(const Pos2D& posCell, bool bSearched);
	//
	Color32	getVertexColor(const Pos2D& posCell)const;
	void	setVertexColor(const Pos2D& posCell, Color32 color);
	//
	float	GetHeight(const Vec2D& vPos)const;
	void	SetHeight(const Vec2D& vPos, float fHeight);
	Vec4D	GetColor(const Vec2D& vPos)const;
	// 碰撞精选
	virtual bool PickCell(const Pos2D& posCell, const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D* pPos = NULL)const;
	virtual bool Pick(const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D* pPos = NULL)const;
	//
	void	CalcTexUV();
	//
	const std::string& getFilename()const{return m_strFilename;}
	//
	Vec3D		GetLightDir()const{return m_vLightDir;}

	void	getVertexByCell(int nCellX, int nCellY, TerrainVertex& vertex)const;
	void	getGrassVertexByCell(int nCellX, int nCellY, TerrainVertex*& vertex)const;

	bool	hasGrass(int nCellX, int nCellY)const;

	std::vector<int8>&		getTiles(size_t layer)	{return m_TileID[layer];}
	std::vector<Color32>&	getColors()				{return m_Colors;}
	std::vector<Color32>&	getLightColors()		{return m_LightColors;}
	std::vector<float>&		getHeights()			{return m_Height;}
	std::vector<Vec3D>&		getNormals()			{return m_Normals;}
	std::vector<int8>&		getAttributes()			{return m_Attribute;}
	std::vector<int8>&		getOthers()				{return m_Other;}
	std::vector<Vec2D>&		getEquableTexUV()		{return m_EquableTexUV;}
protected:
	float	GetCellXLength(const Pos2D& posCell)const;
	float	GetCellYLength(const Pos2D& posCell)const;
	float	GetXLength(int nCellY)const;
	float	GetYLength(int nCellX)const;
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

	std::vector<int8>		m_TileID[2];
	std::vector<Color32>	m_Colors;
	std::vector<Color32>	m_LightColors;
	std::vector<float>		m_Height;
	std::vector<Vec3D>		m_Normals;
	std::vector<int8>		m_Attribute;
	std::vector<int8>		m_Other;
	std::vector<Vec2D>		m_EquableTexUV;
	std::vector<bool>		m_Searched;

	uint16					m_uMuFlgMap;
	uint32					m_uMuFlgAtt;

};