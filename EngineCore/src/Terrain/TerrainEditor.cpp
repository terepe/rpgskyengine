#include "TerrainEditor.h"
#include "RenderSystem.h"
#include "ModelObject.h"
#include "Scene.h"

CTerrainEditor::CTerrainEditor() : CTerrain(),
m_bShowLayer0(true),
m_bShowLayer1(true),
m_bShowAttribute(false),
m_bShowGrid(false)
{
}

CTerrainEditor::~CTerrainEditor()
{
}

bool CTerrainEditor::create()
{
	CTerrain::create();
	createBrush();
	return true;
}

#define  BRUSH_SIZE 8
void CTerrainEditor::createBrush()
{
	m_BrushDecal.create(m_TerrainData.GetVertexXCount(),BRUSH_SIZE);
	m_BrushDecal.SetRandiusTex(GetRenderSystem().GetTextureMgr().RegisterTexture("Data/Textures/Brush00.dds"));
	m_BrushDecal.SetHardnessTex(GetRenderSystem().GetTextureMgr().RegisterTexture("Data/Textures/Brush01.dds"));
}

void CTerrainEditor::drawAttribute()
{
	CRenderSystem& R = GetRenderSystem();
	{
		R.SetAlphaTestFunc(false);
		R.SetBlendFunc(true);
		R.SetTextureColorOP(0,TBOP_SOURCE1,TBS_TFACTOR);
		R.SetTextureAlphaOP(0,TBOP_SOURCE1,TBS_TFACTOR);
	}
	for (std::map<uint8,TerrainSub>::iterator it = m_mapRenderAttributeSubs.begin(); it!=m_mapRenderAttributeSubs.end(); it++)
	{
		Color32 c=0x80000000;
		if (it->first&(1<<0))
		{
			c.g=255;
		}
		if (it->first&(1<<2))
		{
			c.r=255;
		}
		if (it->first&(1<<3))
		{
			c.b=255;
		}
		if (it->first&(1<<1))
		{
			c=0x80778899;
		}
		if (it->first&(1<<4))
		{
			c=0x80778899;
		}
		R.SetTextureFactor(c);
		//switch(it->first)
		//{
		//case 0:// unknown
		//	R.SetTextureFactor(0x80FFFFFF);
		//	break;
		//case 1:// safe
		//	R.SetTextureFactor(0x8000FF00);
		//	break;
		//case 4:// break
		//	R.SetTextureFactor(0x80FF0000);
		//	break;
		//case 5:// safe&break
		//	R.SetTextureFactor(0x80FFFF00);
		//	break;
		//case 0xC:// break&unvisible
		//	R.SetTextureFactor(0x80FF00FF);
		//	break;
		//case 0xFF:
		//	R.SetTextureFactor(0x80000000);
		//	break;
		//default:// unknown
		//	
		//	break;
		//}
		R.drawIndexedSubset(it->second);
	}
}

void CTerrainEditor::Render()
{
	//CTerrain::Render();

	if (m_bShowBox)
	{
		DrawCubeBoxes();
	}
	// 地形 VB设置一次
	if (Prepare())
	{
		// 地块
		if (m_bShowLayer0)
		{
			drawLayer0();
		}
		if (m_bShowLayer1)
		{
			drawLayer1();
		}
		if (m_bShowAttribute)
		{
			drawAttribute();
		}
		if (m_bShowGrid)
		{
			drawGrid();
		}

		// 阴影
		//if (LightMapPrepare())
		//{
		//	
		//}
		//LightMapFinish();
		m_BrushDecal.Render();
	}

	renderGrass();
	//RenderGrid();
}

void CTerrainEditor::drawGrid()
{
	CRenderSystem& R = GetRenderSystem();
	R.setTextureMatrix(0,TTF_DISABLE);
	//R.SetWorldMatrix(Matrix::newTranslation(Vec3D(0.0f,0.1f,0.0f)));
	R.SetLightingEnabled(false);
	R.SetCullingMode(CULL_NONE);
	//R.SetRenderState(D3DRS_FOGENABLE, false);
	R.SetDepthBufferFunc(true,true);
	R.SetAlphaTestFunc(true);
	R.SetBlendFunc(true);
	R.SetTextureFactor(0xFF0000FF);
	R.SetTextureColorOP(0,TBOP_SOURCE1,TBS_TFACTOR);
	R.SetTextureAlphaOP(0,TBOP_SOURCE1);
	R.SetTextureColorOP(1,TBOP_DISABLE);
	static int nTexGrid = R.GetTextureMgr().RegisterTexture("Data\\Textures\\grid.tga",1);// level is 1,let the grid more clear.
	R.SetTexture(0,nTexGrid);
	draw();
}

void CTerrainEditor::Brush(float fPosX, float fPosY)
{
	if (m_TerrainData.isCellIn(Pos2D(fPosX, fPosY)))
	{
		// 给更新纹理用的
		float fRadius = m_BrushDecal.GetRadius()*m_BrushDecal.GetSize();
		if (CTerrainBrush::BRUSH_TYPE_TERRAIN_TEXTURE_PAINT == m_BrushDecal.GetBrushType())
		{
			fPosX -=0.5f;
			fPosY -=0.5f;
		}
		for (int y=fPosY-fRadius; y<fPosY+fRadius; y++)
		{
			std::vector<size_t> updateRange;
			for (int x=fPosX-fRadius; x<fPosX+fRadius; x++)
			{
				Pos2D posCell(x,y);
				if (m_TerrainData.isCellIn(posCell))
				{
					Vec2D vLength(fPosX-x, fPosY-y);
					float fOffset = 1.0f-vLength.length()/fRadius;
					fOffset = min(fOffset/(1.0f-m_BrushDecal.GetHardness()),1.0f);
					if (fOffset>0)
					{
						updateRange.push_back(m_TerrainData.GetVertexIndex(posCell));
						switch(m_BrushDecal.GetBrushType())
						{
						case CTerrainBrush::BRUSH_TYPE_TERRAIN_HEIGHT:
							{
								float fHeight = m_TerrainData.getVertexHeight(posCell);
								switch(m_BrushDecal.getBrushHeightType())
								{
								case CTerrainBrush::BHT_NORMAL:
									{
										fHeight +=fOffset*(m_BrushDecal.GetStrength()-0.5f);
									}
									break;
								case CTerrainBrush::BHT_SMOOTH:
									{
										float fRate = fOffset/fRadius;
										fHeight = fHeight*fRate+
											(m_TerrainData.getVertexHeight(Pos2D(x+1,y))+
											m_TerrainData.getVertexHeight(Pos2D(x-1,y))+
											m_TerrainData.getVertexHeight(Pos2D(x,y+1))+
											m_TerrainData.getVertexHeight(Pos2D(x,y-1)))
											*0.25f*(1.0f-fRate);
									}
									break;
								}
								fHeight = min(max(fHeight,m_BrushDecal.getHeightMin()),m_BrushDecal.getHeightMax());
								m_TerrainData.setVertexHeight(posCell,fHeight);
							}
							break;
						case CTerrainBrush::BRUSH_TYPE_TERRAIN_ATT:
							{
								uint8 uAtt = m_BrushDecal.GetAtt();
								m_TerrainData.setCellAttribute(posCell,uAtt);
							}
							break;
						case CTerrainBrush::BRUSH_TYPE_TERRAIN_TEXTURE_ALPHA_PAINT:
							{
								Color32 color = m_TerrainData.getVertexColor(posCell);
								float fRate=fOffset*(m_BrushDecal.GetStrength()-0.5f);
								color.a = min(max(color.a+fRate*255,0),255);
								m_TerrainData.setVertexColor(posCell,color);
							}
							break;
						case CTerrainBrush::BRUSH_TYPE_TERRAIN_TEXTURE_PAINT:
							{
								int nTileID = m_BrushDecal.GetTileID(0,0);
								m_TerrainData.SetCellTileID(posCell,nTileID, m_BrushDecal.GetTileLayer());
							}
							break;
						default:
							break;
						}
					}
				}
			}
			switch(m_BrushDecal.GetBrushType())
			{
			case CTerrainBrush::BRUSH_TYPE_TERRAIN_HEIGHT:
			case CTerrainBrush::BRUSH_TYPE_TERRAIN_TEXTURE_ALPHA_PAINT:
				if (updateRange.size()>0)
				{
					if (m_pVB)
					{
						uint32 uOffset	= sizeof(TerrainVertex)*updateRange[0];
						uint32 uSize	= sizeof(TerrainVertex)*updateRange.size();
						TerrainVertex*	pV = (TerrainVertex*)m_pVB->lock(uOffset, uSize, CHardwareBuffer::HBL_NO_OVERWRITE);
						for (size_t n=updateRange[0]; n<updateRange[0]+updateRange.size(); ++n)
						{
							Pos2D posCell = m_TerrainData.GetCellPosByVertexIndex(n);
							m_TerrainData.getVertexByCell(posCell.x,posCell.y,*pV);
							pV++;
						}
						m_pVB->unlock();
					}
				}
				break;
			default:
				break;
			}
		}
		switch(m_BrushDecal.GetBrushType())
		{
		case CTerrainBrush::BRUSH_TYPE_TERRAIN_HEIGHT:
			UpdateCubeBBox(m_RootCube);
			break;
		case CTerrainBrush::BRUSH_TYPE_TERRAIN_ATT:
		case CTerrainBrush::BRUSH_TYPE_TERRAIN_TEXTURE_PAINT:
			updateIB();
			break;
		default:
			break;
		}
	}
}

void CTerrainEditor::CreateIB()
{
	S_DEL(m_pIB);
	m_pIB = GetRenderSystem().GetHardwareBufferMgr().CreateIndexBuffer(m_TerrainData.GetCellCount()*3*6*sizeof(uint16),CHardwareIndexBuffer::IT_16BIT,CHardwareBuffer::HBU_WRITE_ONLY);
}

void CTerrainEditor::updateIB()
{
	CTerrain::updateIB();
	m_mapRenderAttributeSubs.clear();
	if (m_pIB)
	{
		for (LIST_CUBES::iterator it=m_RenderCubesList.begin(); it!=m_RenderCubesList.end(); it++)
		{
			const BBox& bbox = (*it)->bbox;
			for (size_t y = bbox.vMin.z; y<bbox.vMax.z; ++y)
			{
				for (size_t x = bbox.vMin.x; x<bbox.vMax.x; ++x)
				{
					Pos2D posCell=Pos2D(x,y);
					uint32 uIndex = m_TerrainData.GetVertexIndex(posCell);

					const uint8 uAttribute = m_TerrainData.getCellAttribute(posCell);
					if (uAttribute!=0x0)
					{
						m_mapRenderAttributeSubs[uAttribute].myVertexIndex(uIndex);
					}
				}
			}
		}
		uint32 m_uShowAttributeIBCount = 0;

		for (std::map<uint8,TerrainSub>::iterator it = m_mapRenderAttributeSubs.begin(); it!=m_mapRenderAttributeSubs.end(); it++)
		{
			it->second.istart = m_uShowTileIBCount+m_uShowAttributeIBCount;
			m_uShowAttributeIBCount += it->second.icount;
			it->second.icount = 0;
			it->second.vcount=it->second.vcount-it->second.istart+1+m_TerrainData.GetVertexXCount()+1;
		}

		uint32 uTempVertexXCount = m_TerrainData.GetVertexXCount();
		uint16* index = (uint16*)m_pIB->lock(m_uShowTileIBCount*sizeof(uint16), m_uShowAttributeIBCount*sizeof(uint16),CHardwareBuffer::HBL_NO_OVERWRITE);
		for (LIST_CUBES::iterator it=m_RenderCubesList.begin(); it!=m_RenderCubesList.end(); it++)
		{
			const BBox& bbox = (*it)->bbox;
			for (size_t y = bbox.vMin.z; y<bbox.vMax.z; ++y)
			{
				for (size_t x = bbox.vMin.x; x<bbox.vMax.x; ++x)
				{
					Pos2D posCell=Pos2D(x,y);
					uint32 uIndex = m_TerrainData.GetVertexIndex(posCell);
		
					const uint8 uAttribute = m_TerrainData.getCellAttribute(posCell);
					if (uAttribute!=0x0)
					{
						TerrainSub& sub = m_mapRenderAttributeSubs[uAttribute];
						{
							// 2	 3
							//	*---*
							//	| / |
							//	*---*
							// 0	 1
							uint16* p = index+sub.istart+sub.icount-m_uShowTileIBCount;
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
		m_pIB->unlock();
	}
}