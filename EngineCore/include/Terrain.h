#pragma once

#include "TerrainData.h"
#include "TerrainDecal.h"
#include "RenderSystem.h"

#define TERRAIN_VERTEX_FVF	(FVF_XYZ | FVF_NORMAL | FVF_DIFFUSE | FVF_TEX3)

struct TerrainSub:public IndexedSubset
{
	void myVertexIndex(unsigned long uIndex)
	{
		if (icount==0)
		{
			vstart=uIndex;
		}
		vstart=min(vstart,uIndex);
		vcount=max(vcount,uIndex);
		icount+=6;
	}
};

class CTerrain:public CTerrainData
{
public:
	CTerrain(); 
	CTerrain(const std::string& strFilename); 
	~CTerrain();
	//
	virtual void setTileMaterial(int nTileID, const std::string& strMaterialName);
	virtual CMaterial& getMaterial(const std::string& strMaterialName);
	//
	virtual void create(size_t width, size_t height,  size_t cubeSize);
	virtual bool resize(size_t width, size_t height,  size_t cubeSize);
	//
	void	ShowBox(bool bShowBox){m_bShowBox = bShowBox;}
	bool	IsShowBox(){return m_bShowBox;}
	//void FormMove();
	// 渲染
	//virtual void GetRenderObject(const CFrustum& frustum, LIST_OBJECTPASS& ObjectList);

	virtual void getCubesByFrustum(const CFrustum& frustum, LIST_CUBES& CubeList)const;
	virtual void getCrunodeCubes(LIST_CUBES& CubeList)const;

	virtual void UpdateRender(const CFrustum& frustum);

	virtual bool LightMapPrepare();
	virtual void LightMapFinish();
	virtual bool Prepare();
	virtual void DrawCubeBoxes(Color32 color=0xFF00FF00);
	virtual void drawLayer0();
	virtual void drawLayer1();

	virtual void renderGrass();

	virtual void Render();
	virtual void draw();
	virtual void DrawChunk(const Cube& cube);
	//
	MAP_TILES& GetTiles(){return m_Tiles;}
	Cube& GetRootCube(){return m_RootCube;}
	CHardwareVertexBuffer* GetVB(){return m_pVB;}
	CTerrainDecal& GetLightMapDecal(){return m_LightMapDecal;}
	std::string	getTileListFilename(){return m_strTileListFilename;}
	void clearAllTiles();
	void setLightMapTexture(const std::string& strFilename);
	virtual bool create();// 初始化
protected:
	//
	virtual void updateVB(int nBeginX, int nBeginY, int nEndX, int nEndY);
	virtual void CreateVB();
	virtual void CreateIB();
	virtual void CreateGrassVB(size_t uGrassCount);
	virtual void CreateGrassIB(size_t uGrassCount);
	//
	virtual void updateIB();
	// Calculate
	void	CalcChunkIB(Cube& cube);
	void	UpdateCubeBBox(Cube& cube);
protected:
	//int						m_nVBID;
	CHardwareVertexBuffer*	m_pVB;
	CHardwareIndexBuffer*	m_pIB;
	// grass
	CHardwareVertexBuffer*	m_pGrassVB;
	CHardwareIndexBuffer*	m_pGrassIB;

	Cube			m_RootCube;
	bool			m_bShowBox;
	//
	// 贴花
	CTerrainDecal	m_LightMapDecal;
	// 纹理Tile
	MAP_TILES		m_Tiles;
	int				m_nLightMap;
	unsigned long			m_uShowTileIBCount;
	// For Render
	std::map<unsigned char,TerrainSub>	m_RenderTileSubsLayer[2];
	TerrainSub					m_GrassSub;
	//
	LIST_CUBES		m_RenderCubesList;
	LIST_CUBES		m_RenderChunkCubesList;

	std::string		m_strTileListFilename;
};