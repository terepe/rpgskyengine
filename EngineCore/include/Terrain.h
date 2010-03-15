#pragma once

#include "TerrainData.h"
#include "TerrainDecal.h"
#include "RenderSystem.h"

#define TERRAIN_VERTEX_FVF	(FVF_XYZ | FVF_NORMAL | FVF_DIFFUSE | FVF_TEX3)

struct TerrainSub:public IndexedSubset
{
	void myVertexIndex(uint32 uIndex)
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

class CTerrain:public iTerrain
{
public:
	CTerrain(); 
	CTerrain(const std::string& strFilename); 
	~CTerrain();
	//
	virtual void setTile(int nTileID, const std::string& strMaterialName);
	//
	virtual void create(size_t width, size_t height,  size_t cubeSize);
	virtual void resize(size_t width, size_t height,  size_t cubeSize);
	//
	void InitTiles();
	//
	void	ShowBox(bool bShowBox){m_bShowBox = bShowBox;}
	bool	IsShowBox(){return m_bShowBox;}
	//void FormMove();
	// ��Ⱦ
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

	virtual bool prepareGrass();
	virtual void drawGrass();
	virtual void finishGrass();

	virtual void Render();
	virtual void draw();
	virtual void DrawChunk(const Cube& cube);
	virtual void Finish();
	//
	MAP_TILES& GetTiles(){return m_Tiles;}
	iTerrainData& GetData(){return m_TerrainData;}
	const iTerrainData& GetData()const{return m_TerrainData;}
	Cube& GetRootCube(){return m_RootCube;}
	CHardwareVertexBuffer* GetVB(){return m_pVB;}
	CTerrainDecal& GetLightMapDecal(){return m_LightMapDecal;}
	std::string	getTileListFilename(){return m_strTileListFilename;}
	void clearAllTiles();
	void setLightMapTexture(const std::string& strFilename);
	void setGrassTexture(const std::string& strFilename);
	void setGrassShader(const std::string& strFilename);
	virtual bool create();// ��ʼ��
protected:
	//
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
	CTerrainData			m_TerrainData;
	//int						m_nVBID;
	CHardwareVertexBuffer*	m_pVB;
	CHardwareIndexBuffer*	m_pIB;
	// grass
	CHardwareVertexBuffer*	m_pGrassVB;
	CHardwareIndexBuffer*	m_pGrassIB;

	Cube			m_RootCube;
	bool			m_bShowBox;
	//
	// ����
	CTerrainDecal	m_LightMapDecal;
	// ����Tile
	MAP_TILES		m_Tiles;
	int				m_nLightMap;
	uint32			m_uShowTileIBCount;
	// For Render
	std::map<uint8,TerrainSub>	m_RenderTileSubsLayer[2];
	//
	LIST_CUBES		m_RenderCubesList;
	LIST_CUBES		m_RenderChunkCubesList;

	std::string		m_strTileListFilename;
	uint32			m_uGrassTexID;
	uint32			m_uGrassShaderID;
};