#include "Terrain.h"
#include <fstream>
#include "RenderSystem.h"
#include "Graphics.h"

CTerrain::CTerrain():
m_pVB(NULL),
m_pIB(NULL),
m_pGrassVB(NULL),
m_pGrassIB(NULL),
m_bShowBox(false),
m_nLightMap(0),
m_uShowTileIBCount(0)
{
	m_Tiles[0] = "Terrain.00";
	m_Tiles[1] = "Terrain.01";
	m_Tiles[2] = "Terrain.02";
	m_Tiles[3] = "Terrain.03";
	m_Tiles[4] = "Terrain.04";
	m_Tiles[5] = "Terrain.05";
	m_Tiles[6] = "Terrain.06";
	m_Tiles[7] = "Terrain.07";
	m_Tiles[8] = "Terrain.08";
}

CTerrain::~CTerrain()
{
	S_DEL(m_pVB);
	S_DEL(m_pIB);
	S_DEL(m_pGrassVB);
	S_DEL(m_pGrassIB);
}

void CTerrain::setTileMaterial(int nTileID, const std::string& strMaterialName)
{
	m_Tiles[nTileID] = strMaterialName;
}

CMaterial& CTerrain::getMaterial(const char* szMaterialName)
{
	return GetRenderSystem().getMaterialMgr().getItem(szMaterialName);
}

void CTerrain::updateIB()
{
	m_RenderTileSubsLayer[0].clear();
	m_RenderTileSubsLayer[1].clear();
	// 写IB
	if (m_pIB)
	{
		for (LIST_CUBES::iterator it=m_RenderCubesList.begin(); it!=m_RenderCubesList.end(); it++)
		{
			const BBox& bbox = (*it)->bbox;
			for (size_t y = bbox.vMin.z; y<bbox.vMax.z; ++y)
			{
				for (size_t x = bbox.vMin.x; x<bbox.vMax.x; ++x)
				{
					TerrainCell& cell = *CTerrainData::getCell(x,y);
					if ((cell.uAttribute&0x8)==0)// 透明
					{
						unsigned long uIndex = y*CTerrainData::GetVertexXCount()+x;
						const unsigned char uTileID0 = cell.uTileID[0];
						if (uTileID0!=0xFF)// 透明
						{
							m_RenderTileSubsLayer[0][uTileID0].myVertexIndex(uIndex);
						}
						const unsigned char uTileID1 = cell.uTileID[1];
						if (uTileID1!=0xFF)// 透明
						{
							m_RenderTileSubsLayer[1][uTileID1].myVertexIndex(uIndex);
						}
					}
				}
			}
		}

		// resize grass vertex.
		//size_t uGrassCount = m_RenderTileSubsLayer[0][0].icount/6;
		//if (uGrassCount>0)
		{
			size_t uGrassCount = 0;
			for (LIST_CUBES::iterator it=m_RenderCubesList.begin(); it!=m_RenderCubesList.end(); it++)
			{
				const BBox& bbox = (*it)->bbox;
				for (size_t y = bbox.vMin.z; y<bbox.vMax.z; ++y)
				{
					for (size_t x = bbox.vMin.x; x<bbox.vMax.x; ++x)
					{
						if (CTerrainData::hasGrass(x,y))
						{
							uGrassCount++;
						}
					}
				}
			}
			if (uGrassCount)
			{
				CreateGrassVB(uGrassCount);
				CreateGrassIB(uGrassCount);
				m_GrassSub.vbase=0;	// base vertex
				m_GrassSub.vstart=0;	// first vertex
				m_GrassSub.vcount=m_pGrassVB->getVerticesCount();	// num vertices
				m_GrassSub.istart=0;	// first index
				m_GrassSub.icount=m_pGrassIB->getIndexesCount();	// num indices
			}
		}

		m_uShowTileIBCount = 0;
		for (int nLayer=0; nLayer<2; nLayer++)
		{
			for (std::map<unsigned char,TerrainSub>::iterator it = m_RenderTileSubsLayer[nLayer].begin(); it!=m_RenderTileSubsLayer[nLayer].end(); it++)
			{
				it->second.istart = m_uShowTileIBCount;
				m_uShowTileIBCount += it->second.icount;
				it->second.icount = 0;
				it->second.vcount=it->second.vcount-it->second.vstart+1+CTerrainData::GetVertexXCount()+1;
			}
		}

		unsigned long uTempVertexXCount = CTerrainData::GetVertexXCount();
		unsigned short* index = (unsigned short*)m_pIB->lock(0, m_uShowTileIBCount*sizeof(unsigned short),CHardwareBuffer::HBL_NO_OVERWRITE);
		for (LIST_CUBES::iterator it=m_RenderCubesList.begin(); it!=m_RenderCubesList.end(); it++)
		{
			const BBox& bbox = (*it)->bbox;
			for (size_t y = bbox.vMin.z; y<bbox.vMax.z; ++y)
			{
				for (size_t x = bbox.vMin.x; x<bbox.vMax.x; ++x)
				{
					TerrainCell& cell = *CTerrainData::getCell(x,y);
					if ((cell.uAttribute&0x8)==0)// 透明
					{
						unsigned long uIndex = CTerrainData::getVertexIndex(x,y);
						for (int nLayer=0; nLayer<2; nLayer++)
						{
							const unsigned char uTileID = cell.uTileID[nLayer];
							TerrainSub& sub = m_RenderTileSubsLayer[nLayer][uTileID];
							if (uTileID!=0xFF)
							{
								// 2	 3
								//	*---*
								//	| / |
								//	*---*
								// 0	 1
								unsigned short* p = index+sub.istart+sub.icount;
								sub.icount+=6;
								*p = uIndex;
								p++;
								*p = uIndex+uTempVertexXCount;
								p++;
								*p = uIndex+uTempVertexXCount+1;
								p++;

								*p = uIndex;
								p++;
								*p = uIndex+uTempVertexXCount+1;
								p++;
								*p = uIndex+1;
								p++;
							}
						}
					}
				}
			}
		}
		m_pIB->unlock();
	}
}

void CTerrain::UpdateCubeBBox(Cube& cube)
{
	if (cube.pChildCube==NULL)
	{
		float fInitHeight = CTerrainData::getVertexHeight(cube.bbox.vMin.x,cube.bbox.vMin.z);
		cube.bbox.vMin.y = fInitHeight;
		cube.bbox.vMax.y = fInitHeight;
		for (size_t y = cube.bbox.vMin.z; y<=cube.bbox.vMax.z; ++y)
		{
			for (size_t x = cube.bbox.vMin.x; x<=cube.bbox.vMax.x; ++x)
			{
				float fHeight = CTerrainData::getVertexHeight(x,y);
				cube.bbox.vMin.y = min(cube.bbox.vMin.y,fHeight);
				cube.bbox.vMax.y = max(cube.bbox.vMax.y,fHeight);
			}
		}
	}
	else
	{
		Cube* pChildCube = cube.pChildCube;
		UpdateCubeBBox(*pChildCube);
		cube.bbox.vMin.y = pChildCube->bbox.vMin.y;
		cube.bbox.vMax.y = pChildCube->bbox.vMax.y;
		pChildCube++;
		for (size_t i=1; i<2; ++i)
		{
			UpdateCubeBBox(*pChildCube);
			cube.bbox.vMin.y = min(cube.bbox.vMin.y, pChildCube->bbox.vMin.y);
			cube.bbox.vMax.y = max(cube.bbox.vMax.y, pChildCube->bbox.vMax.y);
			pChildCube++;
		}
	}
}

void CTerrain::updateVB(int nBeginX, int nBeginY, int nEndX, int nEndY)
{
	if (m_pVB==NULL)
	{
		return;
	}
	unsigned long uOffset	= sizeof(TerrainVertex)*CTerrainData::getVertexIndex(nBeginX,nBeginY);
	unsigned long uSize	= sizeof(TerrainVertex)*(CTerrainData::getVertexIndex(nEndX,nEndY)+1)-uOffset;
	if (uSize>0)
	{
		TerrainVertex*	pV = (TerrainVertex*)m_pVB->lock(uOffset, uSize, CHardwareBuffer::HBL_NO_OVERWRITE/*CHardwareBuffer::HBL_NORMAL*/);
		for (int y = nBeginY; y <= nEndY; ++y)
		{
			for (int x = nBeginX; x <= nEndX; ++x)
			{
				CTerrainData::getVertexByCell(x,y,*pV);
				pV++;
			}
		}
		m_pVB->unlock();
	}
}

void CTerrain::CreateVB()
{
	S_DEL(m_pVB);
	m_pVB = GetRenderSystem().GetHardwareBufferMgr().CreateVertexBuffer(CTerrainData::GetVertexCount(),sizeof(TerrainVertex));
	updateVB(0,0,CTerrainData::GetVertexXCount()-1,CTerrainData::GetVertexYCount()-1);/*CHardwareBuffer::HBL_NORMAL*/
}

void CTerrain::CreateIB()
{
	S_DEL(m_pIB);
	m_pIB = GetRenderSystem().GetHardwareBufferMgr().CreateIndexBuffer(CTerrainData::GetCellCount()*2*6*sizeof(unsigned short),CHardwareIndexBuffer::IT_16BIT,CHardwareBuffer::HBU_WRITE_ONLY);
}

void CTerrain::CreateGrassVB(size_t uGrassCount)
{
	S_DEL(m_pGrassVB);
	m_pGrassVB = GetRenderSystem().GetHardwareBufferMgr().CreateVertexBuffer(uGrassCount*4,sizeof(TerrainVertex),CHardwareBuffer::HBU_DYNAMIC);
	if (m_pGrassVB)
	{
		TerrainVertex*	pV = (TerrainVertex*)m_pGrassVB->lock(CHardwareBuffer::HBL_NORMAL);
		for (LIST_CUBES::iterator it=m_RenderCubesList.begin(); it!=m_RenderCubesList.end(); it++)
		{
			const BBox& bbox = (*it)->bbox;
			for (size_t y = bbox.vMin.z; y<bbox.vMax.z; ++y)
			{
				for (size_t x = bbox.vMin.x; x<bbox.vMax.x; ++x)
				{
					if (CTerrainData::hasGrass(x,y))
					{
						CTerrainData::getGrassVertexByCell(x,y,pV);
						pV+=4;
					}
				}
			}
		}
		m_pGrassVB->unlock();
	}
}

void CTerrain::CreateGrassIB(size_t uGrassCount)
{
	S_DEL(m_pGrassIB);
	m_pGrassIB = GetRenderSystem().GetHardwareBufferMgr().CreateIndexBuffer(uGrassCount*6*sizeof(unsigned short),CHardwareIndexBuffer::IT_16BIT,CHardwareBuffer::HBU_WRITE_ONLY);
	unsigned short* index = (unsigned short*)m_pGrassIB->lock(0, uGrassCount*6*sizeof(unsigned short),CHardwareBuffer::HBL_NO_OVERWRITE);
	int i=0;
	for (LIST_CUBES::iterator it=m_RenderCubesList.begin(); it!=m_RenderCubesList.end(); it++)
	{
		const BBox& bbox = (*it)->bbox;
		for (size_t y = bbox.vMin.z; y<bbox.vMax.z; ++y)
		{
			for (size_t x = bbox.vMin.x; x<bbox.vMax.x; ++x)
			{
				if (CTerrainData::hasGrass(x,y))
				{
					// 1	 2
					//	*---*
					//	| / |
					//	*---*
					// 0	 3
					*index = i;
					index++;
					*index = i+1;
					index++;
					*index = i+2;
					index++;

					*index = i;
					index++;
					*index = i+2;
					index++;
					*index = i+3;
					index++;
					i+=4;
				}
			}
		}
	}
	m_pGrassIB->unlock();
}

void CTerrain::create(size_t width, size_t height, size_t cubeSize)
{
	CTerrainData::create(width, height, cubeSize);
	create();
}

bool CTerrain::resize(size_t width, size_t height, size_t cubeSize)
{
	if (CTerrainData::resize(width, height, cubeSize)==false)
	{
		return false;
	}
	return create();
}

void CTerrain::setLightMapTexture(const std::string& strFilename)
{
	GetRenderSystem().GetTextureMgr().del(m_nLightMap);
	m_nLightMap = GetRenderSystem().GetTextureMgr().RegisterTexture(strFilename);
}

bool CTerrain::create()
{
	m_RenderTileSubsLayer[0].clear();
	m_RenderTileSubsLayer[1].clear();
	m_RenderCubesList.clear();
	m_RenderChunkCubesList.clear();

	CreateVB();
	CreateIB();
	{
		S_DELS(m_RootCube.pChildCube);

		m_RootCube.bbox.vMin = Vec3D(0.0f,0.0f,0.0f);
		m_RootCube.bbox.vMax = Vec3D((float)CTerrainData::GetWidth(),0.0f,(float)CTerrainData::GetHeight());
		m_RootCube.createChildCube(CTerrainData::GetCubeSize());
		UpdateCubeBBox(m_RootCube);
	}
	m_LightMapDecal.createBySize(CTerrainData::GetVertexXCount(), CTerrainData::GetCubeSize(), CTerrainData::GetCubeSize());
	m_LightDecal.createBySize(CTerrainData::GetVertexXCount(),8,8);
	return true;
}

bool CTerrain::Prepare()
{
	if (m_pVB==NULL||m_pIB==NULL)
	{
		return false;
	}
	CRenderSystem& R = GetRenderSystem();
	R.SetLightingEnabled(false);
	R.SetCullingMode(CULL_ANTI_CLOCK_WISE);
	R.SetDepthBufferFunc(true,true);

	R.SetFVF(TERRAIN_VERTEX_FVF);
	R.SetStreamSource(0, m_pVB, 0, sizeof(TerrainVertex));
	R.SetIndices(m_pIB);
	return true;
}

bool CTerrain::LightMapPrepare()
{
	CRenderSystem& R = GetRenderSystem();
	R.SetLightingEnabled(false);
	R.SetCullingMode(CULL_ANTI_CLOCK_WISE);
	//R.SetRenderState(D3DRS_FOGENABLE, false);
	R.SetDepthBufferFunc(true,false);
	R.SetAlphaTestFunc(false);
	R.SetBlendFunc(true,BLENDOP_ADD,SBF_DEST_COLOUR,SBF_ZERO);

	R.SetTextureColorOP(0,TBOP_SOURCE1,TBS_TEXTURE,TBS_DIFFUSE);
	R.SetTextureAlphaOP(0,TBOP_DISABLE);
	R.SetTextureColorOP(1,TBOP_DISABLE);
	R.SetTexCoordIndex(0, 2);
	//bump
	//static int nTexID = GetRenderSystem().GetTextureMgr().RegisterTexture("Data/Textures/Tile/road.dds");
	R.SetTexture(0,m_nLightMap);

	R.SetIndices(m_LightMapDecal.GetIB());
	return true;
}

void CTerrain::LightMapFinish()
{
	CRenderSystem& R = GetRenderSystem();
	//R.BlendFunc(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
	//R.SetTexCoordIndex(0, 0);
	R.setTextureMatrix(0,TTF_DISABLE);
	R.setTextureMatrix(1,TTF_DISABLE);
	R.SetTextureColorOP(1,TBOP_DISABLE);
	R.SetTextureColorOP(2,TBOP_DISABLE);
}

void CTerrain::UpdateRender(const CFrustum& frustum)
{
	//
	m_RenderCubesList.clear();
	getCubesByFrustum(frustum, m_RenderCubesList);
	//
	m_RenderChunkCubesList.clear();
	for (size_t i=0; i<m_RenderCubesList.size(); ++i)
	{
		m_RenderCubesList[i]->getChildCrunodeCubes(m_RenderChunkCubesList);
	}
	//
	updateIB();
}

void CTerrain::DrawCubeBoxes(Color32 color)
{
	CRenderSystem& R = GetRenderSystem();
	R.SetDepthBufferFunc(true,true);
	for (size_t i=0; i<m_RenderCubesList.size(); ++i)
	{
		GetGraphics().drawBBox(m_RenderCubesList[i]->bbox,color);
	}
}

void CTerrain::drawLayer0()
{
	CRenderSystem& R = GetRenderSystem();
	R.SetTexCoordIndex(0, 0);	// Diffuse
	R.SetTexCoordIndex(1, 1);	// lightmap
	for (std::map<unsigned char,TerrainSub>::iterator it = m_RenderTileSubsLayer[0].begin(); it!=m_RenderTileSubsLayer[0].end(); it++)
	{
		if (R.prepareMaterial(m_Tiles[it->first].c_str()))
		{
			R.SetAlphaTestFunc(false);
			R.SetBlendFunc(false);
			R.SetTextureAlphaOP(0,TBOP_DISABLE);
			R.drawIndexedSubset(it->second);
		}
		R.finishMaterial();
	}
}

void CTerrain::drawLayer1()
{
	CRenderSystem& R = GetRenderSystem();
	R.SetTexCoordIndex(0, 0);	// Diffuse
	R.SetTexCoordIndex(1, 1);	// lightmap
	for (std::map<unsigned char,TerrainSub>::iterator it = m_RenderTileSubsLayer[1].begin(); it!=m_RenderTileSubsLayer[1].end(); it++)
	{
		if (R.prepareMaterial(m_Tiles[it->first].c_str()))
		{
			R.drawIndexedSubset(it->second);
		}
		R.finishMaterial();
	}
}

void CTerrain::renderGrass()
{
	CRenderSystem& R = GetRenderSystem();
	{
		if (m_pGrassVB==NULL||m_pGrassIB==NULL)
		{
			return;
		}
		R.SetLightingEnabled(false);
		R.SetCullingMode(CULL_NONE);
		R.SetDepthBufferFunc(true,true);
		R.SetAlphaTestFunc(true,CMPF_GREATER_EQUAL,0x80);

		R.SetFVF(TERRAIN_VERTEX_FVF);
		R.SetStreamSource(0, m_pGrassVB, 0, sizeof(TerrainVertex));
		R.SetIndices(m_pGrassIB);
	}
	if (R.prepareMaterial("Terrain.Grass"))
	{
		R.drawIndexedSubset(m_GrassSub);
	}
	R.finishMaterial();
}

void CTerrain::Render()
{
	if (m_bShowBox)
	{
		DrawCubeBoxes();
	}
	// 地形 VB设置一次
	if (Prepare())
	{
		drawLayer0();
		drawLayer1();
		// 阴影
		//if (LightMapPrepare())
		//{
		//	drawTerrain();
		//}
		//LightMapFinish();
	}
	//renderGrass();		// Show grass.
}

void CTerrain::draw()
{
	if (m_LightMapDecal.setIB())
	{
		for (size_t i=0;i<m_RenderChunkCubesList.size();++i)
		{
			DrawChunk(*m_RenderChunkCubesList[i]);
		}
	}
}

void CTerrain::DrawChunk(const Cube& cube)
{
	unsigned long uBaseVertexIndex = CTerrainData::getVertexIndex((int)cube.bbox.vMin.x,(int)cube.bbox.vMin.z);
	m_LightMapDecal.Draw(uBaseVertexIndex);
}

void CTerrain::brushLightColor(MAP_EDIT_RECORD& mapEditRecord, float fPosX, float fPosY, Color32 colorPaint, float fRadius, float fHardness, float fStrength)
{
	if (isPointIn(fPosX, fPosY))
	{
		EditTarget editTarget;
		//editTarget.type = CTerrainBrush::BRUSH_TYPE_TERRAIN_COLOR;
		for (int y=fPosY-fRadius; y<fPosY+fRadius; y++)
		{
			for (int x=fPosX-fRadius; x<fPosX+fRadius; x++)
			{
				editTarget.x = x;
				editTarget.y = y;
				if (isPointIn(x,y))
				{
					Vec2D vLength(fPosX-x, fPosY-y);
					float fOffset = 1.0f-vLength.length()/fRadius;
					fOffset = min(fOffset/(1.0f-fHardness),1.0f);
					if (fOffset>0)
					{
						float fRate=fOffset*fStrength;
						Color32& colorDest = (Color32)mapEditRecord[editTarget].color;
						if(colorDest.c==0)
						{
							colorDest=getVertexColor(x,y);
						}
						colorDest += colorPaint*fRate;
					}
				}
			}
		}
	}
}

void CTerrain::drawLightDecal(float x, float y, float fSize, Color32 color)
{
	if (!m_LightDecal.setIB())
	{
		return;
	}
	CRenderSystem& R = GetRenderSystem();
	R.SetSamplerAddressUV(0,ADDRESS_BORDER,ADDRESS_BORDER);
	// 打开纹理矩阵

	float fScale = 0.5f/fSize;
	// －贴花中心Ｕ×缩放比＋０.５纹理偏移
	float fPosX = 0.5f-x*fScale;
	// １－（－贴花中心Ｖ×缩放比＋０.５纹理偏移）
	float fPosY = 0.5f+y*fScale;
	//
	Matrix mLightDecalTrans = Matrix(
		fScale,0,fPosX,0,
		0,-fScale,fPosY,0,
		0,0,0,0,
		0,0,0,0);
	R.setTextureMatrix(0,TTF_COUNT2,mLightDecalTrans);
	m_LightDecal.Draw((int)x,(int)y);

	//
	R.setTextureMatrix(0,TTF_DISABLE);
	R.SetSamplerAddressUV(0,ADDRESS_WRAP,ADDRESS_WRAP);
}

void CTerrain::getCubesByFrustum(const CFrustum& frustum, LIST_CUBES& CubeList)const
{
	m_RootCube.getChildCubesByFrustum(frustum,CubeList);
}

void CTerrain::getCrunodeCubes(LIST_CUBES& CubeList)const
{
	m_RootCube.getChildCrunodeCubes(CubeList);
}

void CTerrain::loadTilesMaterial(const char* szFilename, const char* szParentDir)
{
	//GetRenderSystem().getMaterialMgr().load(szFilename, szParentDir);
}

void CTerrain::clearAllTiles()
{
	//m_Tiles.clear();
}