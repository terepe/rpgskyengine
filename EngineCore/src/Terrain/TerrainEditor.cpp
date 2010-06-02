#include "TerrainEditor.h"
#include "RenderSystem.h"
#include "ModelObject.h"
#include "Scene.h"

CTerrainEditor::CTerrainEditor() : CTerrain(),
m_bShowLayer0(true),
m_bShowLayer1(true),
m_bShowAttribute(false),
m_bShowGrid(false),
m_bShowBrushDecal(false)
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
		if (m_bShowBrushDecal)
		{
			m_BrushDecal.Render();
		}
	}

	renderGrass();
	//RenderGrid();
}

void CTerrainEditor::drawGrid()
{
	CRenderSystem& R = GetRenderSystem();
	R.setTextureMatrix(0,TTF_DISABLE);
	if (R.prepareMaterial("grid"))
	{
		draw();
		R.finishMaterial();
	}
}

void CTerrainEditor::markEdit()
{
	if(m_setReback.size()>0)
	{
		if(m_setReback.back().empty())
		{
			return;
		}
	}
	m_setReback.resize(m_setReback.size()+1);
}

void CTerrainEditor::doEdit(MAP_EDIT_RECORD& mapEditRecordIn,MAP_EDIT_RECORD& mapEditRecordOut)
{
	if(mapEditRecordIn.empty())
	{
		return;
	}

	bool bUpdateCube = false;
	bool bUpdateIB = false;
	Pos2D posBegin = mapEditRecordIn.begin()->first.pos;

	for(std::map<EditTarget,EditValue>::iterator it=mapEditRecordIn.begin();it!=mapEditRecordIn.end();it++)
	{
		if(mapEditRecordOut.find(it->first)==mapEditRecordOut.end())
		{
			EditValue& editValue = mapEditRecordOut[it->first];
			switch(it->first.type)
			{
			case CTerrainBrush::BRUSH_TYPE_TERRAIN_HEIGHT:
				editValue.fHeight = m_TerrainData.getVertexHeight(it->first.pos);
				break;
			case CTerrainBrush::BRUSH_TYPE_TERRAIN_ATT:
				editValue.uAtt = m_TerrainData.getCellAttribute(it->first.pos);
				break;
			case CTerrainBrush::BRUSH_TYPE_TERRAIN_TEXTURE_ALPHA_PAINT:
				editValue.color = m_TerrainData.getVertexColor(it->first.pos).c;
				break;
			case CTerrainBrush::BRUSH_TYPE_TERRAIN_TILE_LAYER1:
				editValue.nTileID = m_TerrainData.GetCellTileID(it->first.pos,0);
				break;
			case CTerrainBrush::BRUSH_TYPE_TERRAIN_TILE_LAYER2:
				editValue.nTileID = m_TerrainData.GetCellTileID(it->first.pos,1);
				break;
			case CTerrainBrush::BRUSH_TYPE_MAX:
				break;
			default:
				break;
			}
		}

		switch(it->first.type)
		{
		case CTerrainBrush::BRUSH_TYPE_TERRAIN_HEIGHT:
			m_TerrainData.setVertexHeight(it->first.pos,it->second.fHeight);
			break;
		case CTerrainBrush::BRUSH_TYPE_TERRAIN_ATT:
			m_TerrainData.setCellAttribute(it->first.pos,it->second.uAtt);
			break;
		case CTerrainBrush::BRUSH_TYPE_TERRAIN_TEXTURE_ALPHA_PAINT:
			m_TerrainData.setVertexColor(it->first.pos,it->second.color);
			break;
		case CTerrainBrush::BRUSH_TYPE_TERRAIN_TILE_LAYER1:
			m_TerrainData.SetCellTileID(it->first.pos,it->second.nTileID, 0);
			break;
		case CTerrainBrush::BRUSH_TYPE_TERRAIN_TILE_LAYER2:
			m_TerrainData.SetCellTileID(it->first.pos,it->second.nTileID, 1);
			break;
		case CTerrainBrush::BRUSH_TYPE_MAX:
			break;
		default:
			break;
		}

		std::map<EditTarget,EditValue>::iterator itNext = it;
		itNext++;

		if(mapEditRecordIn.end()==itNext||it->first.type!=itNext->first.type||it->first.pos.y!=itNext->first.pos.y)
		{
			switch(it->first.type)
			{
			case CTerrainBrush::BRUSH_TYPE_TERRAIN_HEIGHT:
			case CTerrainBrush::BRUSH_TYPE_TERRAIN_TEXTURE_ALPHA_PAINT:
				updateVB(posBegin,it->first.pos);
				break;
			default:
				break;
			}

			switch(it->first.type)
			{
			case CTerrainBrush::BRUSH_TYPE_TERRAIN_HEIGHT:
				bUpdateCube = true;
				bUpdateIB = true;
				break;
			case CTerrainBrush::BRUSH_TYPE_TERRAIN_ATT:
			case CTerrainBrush::BRUSH_TYPE_TERRAIN_TILE_LAYER1:
			case CTerrainBrush::BRUSH_TYPE_TERRAIN_TILE_LAYER2:
				bUpdateIB = true;
				break;
			default:
				break;
			}

			if(mapEditRecordIn.end()!=itNext)
			{
				posBegin = itNext->first.pos;
			}
		}
	}
	if(bUpdateCube)
	{
		UpdateCubeBBox(m_RootCube);
	}
	if(bUpdateIB)
	{
		updateIB();
	}
}

void CTerrainEditor::doEdit(MAP_EDIT_RECORD& mapEditRecordIn)
{
	if(mapEditRecordIn.empty())
	{
		return;
	}
	if(m_setReback.empty())
	{
		m_setReback.resize(m_setReback.size()+1);
	}
	doEdit(mapEditRecordIn,*m_setReback.rbegin());
	m_setRedo.clear();
}

void CTerrainEditor::doEdit(std::vector<MAP_EDIT_RECORD>& mapEditRecordIn,std::vector<MAP_EDIT_RECORD>& mapEditRecordOut)
{
	if(mapEditRecordIn.empty())
	{
		return;
	}
	mapEditRecordOut.resize(mapEditRecordOut.size()+1);
	doEdit(mapEditRecordIn.back(),mapEditRecordOut.back());
	mapEditRecordIn.pop_back();
}

void CTerrainEditor::rebackEdit()
{
	doEdit(m_setReback,m_setRedo);
}

void CTerrainEditor::redoEdit()
{
	doEdit(m_setRedo,m_setReback);
}

void CTerrainEditor::brushATT(float fPosX, float fPosY, byte uAtt, float fRadius)
{
	MAP_EDIT_RECORD mapEditRecord;
	if (m_TerrainData.isCellIn(Pos2D(fPosX, fPosY)))
	{
		fPosX -=0.5f;
		fPosY -=0.5f;
		EditTarget editTarget;
		EditValue editValue;
		editTarget.type = CTerrainBrush::BRUSH_TYPE_TERRAIN_ATT;
		for (int y=fPosY-fRadius; y<fPosY+fRadius; y++)
		{
			for (int x=fPosX-fRadius; x<fPosX+fRadius; x++)
			{
				Pos2D posCell(x,y);
				editTarget.pos = posCell;
				if (m_TerrainData.isCellIn(posCell))
				{
					Vec2D vLength(fPosX-x, fPosY-y);
					float fOffset = 1.0f-vLength.length()/fRadius;
					if (fOffset>0)
					{
						editValue.uAtt = uAtt;
						mapEditRecord[editTarget]=editValue;
					}
				}
			}
		}
	}
	doEdit(mapEditRecord);
}

void CTerrainEditor::brushTile(float fPosX, float fPosY, int nTileID , int nLayer, float fRadius)
{
	MAP_EDIT_RECORD mapEditRecord;
	if (m_TerrainData.isCellIn(Pos2D(fPosX, fPosY)))
	{
		fPosX -=0.5f;
		fPosY -=0.5f;
		EditTarget editTarget;
		EditValue editValue;
		if (nLayer==0)
		{
			editTarget.type = CTerrainBrush::BRUSH_TYPE_TERRAIN_TILE_LAYER1;
		}
		else
		{
			editTarget.type = CTerrainBrush::BRUSH_TYPE_TERRAIN_TILE_LAYER2;
		}
		for (int y=fPosY-fRadius; y<fPosY+fRadius; y++)
		{
			for (int x=fPosX-fRadius; x<fPosX+fRadius; x++)
			{
				Pos2D posCell(x,y);
				editTarget.pos = posCell;
				if (m_TerrainData.isCellIn(posCell))
				{
					Vec2D vLength(fPosX-x, fPosY-y);
					float fOffset = 1.0f-vLength.length()/fRadius;
					if (fOffset>0)
					{
						if (m_TerrainData.getVertexColor(posCell).a==0xFF&&
							m_TerrainData.getVertexColor(Pos2D(x+1,y)).a==0xFF&&
							m_TerrainData.getVertexColor(Pos2D(x,y+1)).a==0xFF&&
							m_TerrainData.getVertexColor(Pos2D(x+1,y+1)).a==0xFF)
						{
							editTarget.type = CTerrainBrush::BRUSH_TYPE_TERRAIN_TILE_LAYER1;
							editValue.nTileID=nTileID;
							mapEditRecord[editTarget]=editValue;
							editTarget.type = CTerrainBrush::BRUSH_TYPE_TERRAIN_TILE_LAYER2;
							editValue.nTileID=0xFF;
							mapEditRecord[editTarget]=editValue;
							if (/*m_TerrainData.GetCellTileID(Pos2D(x,y),1)==0xFF&&*/
								m_TerrainData.GetCellTileID(Pos2D(x+1,y),1)==0xFF&&
								m_TerrainData.GetCellTileID(Pos2D(x,y+1),1)==0xFF&&
								m_TerrainData.GetCellTileID(Pos2D(x+1,y+1),1)==0xFF)
							{
								editTarget.type = CTerrainBrush::BRUSH_TYPE_TERRAIN_TEXTURE_ALPHA_PAINT;
								Color32 color = m_TerrainData.getVertexColor(Pos2D(x+1,y+1));
								color.a = 0;
								editValue.color = color.c;
								mapEditRecord[editTarget]=editValue;
							}
							if (m_TerrainData.GetCellTileID(Pos2D(x,y-1),1)==0xFF&&
								m_TerrainData.GetCellTileID(Pos2D(x+1,y-1),1)==0xFF&&
								/*m_TerrainData.GetCellTileID(Pos2D(x,y),1)==0xFF&&*/
								m_TerrainData.GetCellTileID(Pos2D(x+1,y),1)==0xFF)
							{
								editTarget.type = CTerrainBrush::BRUSH_TYPE_TERRAIN_TEXTURE_ALPHA_PAINT;
								Color32 color = m_TerrainData.getVertexColor(Pos2D(x+1,y));
								color.a = 0;
								editValue.color = color.c;
								mapEditRecord[editTarget]=editValue;
							}
							if (m_TerrainData.GetCellTileID(Pos2D(x-1,y),1)==0xFF&&
								/*m_TerrainData.GetCellTileID(Pos2D(x,y),1)==0xFF&&*/
								m_TerrainData.GetCellTileID(Pos2D(x-1,y+1),1)==0xFF&&
								m_TerrainData.GetCellTileID(Pos2D(x,y+1),1)==0xFF)
							{
								editTarget.type = CTerrainBrush::BRUSH_TYPE_TERRAIN_TEXTURE_ALPHA_PAINT;
								Color32 color = m_TerrainData.getVertexColor(Pos2D(x,y+1));
								color.a = 0;
								editValue.color = color.c;
								mapEditRecord[editTarget]=editValue;
							}
							if (m_TerrainData.GetCellTileID(Pos2D(x-1,y-1),1)==0xFF&&
								m_TerrainData.GetCellTileID(Pos2D(x,y-1),1)==0xFF&&
								m_TerrainData.GetCellTileID(Pos2D(x-1,y),1)==0xFF/*&&*/
								/*m_TerrainData.GetCellTileID(Pos2D(x,y),1)==0xFF*/)
							{
								editTarget.type = CTerrainBrush::BRUSH_TYPE_TERRAIN_TEXTURE_ALPHA_PAINT;
								Color32 color = m_TerrainData.getVertexColor(Pos2D(x,y));
								color.a = 0;
								editValue.color = color.c;
								mapEditRecord[editTarget]=editValue;
							}
						}
						else
						{
							editTarget.type = CTerrainBrush::BRUSH_TYPE_TERRAIN_TILE_LAYER2;
							editValue.nTileID=nTileID;
							mapEditRecord[editTarget]=editValue;
						}
						//editValue.nTileID=nTileID;
						//mapEditRecord[editTarget]=editValue;
					}
				}
			}
		}
	}
	doEdit(mapEditRecord);
}

void CTerrainEditor::brushAlpha(float fPosX, float fPosY, float fRadius, float fHardness, float fStrength)
{
	MAP_EDIT_RECORD mapEditRecord;
	if (m_TerrainData.isCellIn(Pos2D(fPosX, fPosY)))
	{
		EditTarget editTarget;
		EditValue editValue;
		editTarget.type = CTerrainBrush::BRUSH_TYPE_TERRAIN_TEXTURE_ALPHA_PAINT;
		for (int y=fPosY-fRadius; y<fPosY+fRadius; y++)
		{
			for (int x=fPosX-fRadius; x<fPosX+fRadius; x++)
			{
				Pos2D posCell(x,y);
				editTarget.pos = posCell;
				if (m_TerrainData.isCellIn(posCell))
				{
					Vec2D vLength(fPosX-x, fPosY-y);
					float fOffset = 1.0f-vLength.length()/fRadius;
					fOffset = min(fOffset/(1.0f-fHardness),1.0f);
					if (fOffset>0)
					{
						float fRate=fOffset*fStrength;
						Color32 color = m_TerrainData.getVertexColor(posCell);
						color.a = min(max(color.a+fRate*255,0),255);
						editValue.color = color.c;
						mapEditRecord[editTarget]=editValue;
					}
				}
			}
		}
	}
	doEdit(mapEditRecord);
}

void CTerrainEditor::brushHeight(float fPosX, float fPosY, float fRadius, float fHardness, float fStrength, float fMin, float fMax, bool bSmooth)
{
	MAP_EDIT_RECORD mapEditRecord;
	if (m_TerrainData.isCellIn(Pos2D(fPosX, fPosY)))
	{
		EditTarget editTarget;
		EditValue editValue;
		editTarget.type = CTerrainBrush::BRUSH_TYPE_TERRAIN_HEIGHT;
		for (int y=fPosY-fRadius; y<fPosY+fRadius; y++)
		{
			for (int x=fPosX-fRadius; x<fPosX+fRadius; x++)
			{
				Pos2D posCell(x,y);
				editTarget.pos = posCell;
				if (m_TerrainData.isCellIn(posCell))
				{
					Vec2D vLength(fPosX-x, fPosY-y);
					float fOffset = 1.0f-vLength.length()/fRadius;
					fOffset = min(fOffset/(1.0f-fHardness),1.0f);
					if (fOffset>0)
					{
						float fHeight = m_TerrainData.getVertexHeight(posCell);
						if (bSmooth)
						{
							float fRate = fOffset/fRadius;
							fHeight = fHeight*fRate+
								(m_TerrainData.getVertexHeight(Pos2D(x+1,y))+
								m_TerrainData.getVertexHeight(Pos2D(x-1,y))+
								m_TerrainData.getVertexHeight(Pos2D(x,y+1))+
								m_TerrainData.getVertexHeight(Pos2D(x,y-1)))
								*0.25f*(1.0f-fRate);
						}
						else
						{
							fHeight +=fOffset*m_BrushDecal.GetStrength();
						}
						editValue.fHeight = min(max(fHeight,fMin),fMax);
						mapEditRecord[editTarget]=editValue;
					}
				}
			}
		}
	}
	doEdit(mapEditRecord);
}

void CTerrainEditor::Brush(float fPosX, float fPosY)
{
	float fRadius = m_BrushDecal.GetRadius()*m_BrushDecal.GetSize();
	switch(m_BrushDecal.GetBrushType())
	{
	case CTerrainBrush::BRUSH_TYPE_TERRAIN_HEIGHT:
		brushHeight(fPosX, fPosY, fRadius, m_BrushDecal.GetHardness(), m_BrushDecal.GetStrength(),
			m_BrushDecal.getHeightMin(), m_BrushDecal.getHeightMax(), m_BrushDecal.getBrushHeightType()==CTerrainBrush::BHT_SMOOTH);
		break;
	case CTerrainBrush::BRUSH_TYPE_TERRAIN_ATT:
		brushATT(fPosX, fPosY, m_BrushDecal.GetAtt(), fRadius);
		break;
	case CTerrainBrush::BRUSH_TYPE_TERRAIN_TEXTURE_ALPHA_PAINT:
		brushAlpha(fPosX, fPosY, fRadius, m_BrushDecal.GetHardness(), m_BrushDecal.GetStrength());
		break;
	case CTerrainBrush::BRUSH_TYPE_TERRAIN_TILE_LAYER1:
		if(m_BrushDecal.GetStrength()>0)
		{
			brushTile(fPosX, fPosY, m_BrushDecal.GetTileID(0,0), 0, fRadius);
		}
		else
		{
			brushTile(fPosX, fPosY, 0xFF, 0, fRadius);
		}
		break;
	case CTerrainBrush::BRUSH_TYPE_TERRAIN_TILE_LAYER2:
		brushAlpha(fPosX, fPosY, fRadius, m_BrushDecal.GetHardness(), m_BrushDecal.GetStrength());
		if(m_BrushDecal.GetStrength()>0)
		{
			brushTile(fPosX, fPosY, m_BrushDecal.GetTileID(0,0), 1, fRadius);
		}
		else
		{
			brushTile(fPosX, fPosY, 0xFF, 1, fRadius);
		}
		break;
	default:
		break;
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