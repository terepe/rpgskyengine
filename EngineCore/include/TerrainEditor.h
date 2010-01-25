#pragma once
#include "Terrain.h"
#include "TerrainBrush.h"

class DLL_EXPORT CTerrainEditor : public CTerrain
{
public:
	CTerrainEditor(); 
	CTerrainEditor(std::string strFilename); 
	~CTerrainEditor();
	//
	virtual void Render();
	//
	void drawAttribute();
	void drawGrid();
	//
	CTerrainBrush& GetBrushDecal(){return m_BrushDecal;}
	//
	void Brush(float fPosX, float fPosY);
	//
	void showLayer0(bool bShowLayer0){m_bShowLayer0 = bShowLayer0;}
	void showLayer1(bool bShowLayer1){m_bShowLayer1 = bShowLayer1;}
	void showAttribute(bool bShowAttribute){m_bShowAttribute = bShowAttribute;}
	void showGrid(bool bShowGrid){m_bShowGrid = bShowGrid;}
	bool isShowLayer0(){return m_bShowLayer0;}
	bool isShowLayer1(){return m_bShowLayer1;}
	bool isShowAttribute(){return m_bShowAttribute;}
	bool isShowGrid(){return m_bShowGrid;}

	void	CreateIB();
	void	updateIB();
protected:
	void createBrush();
	virtual bool create();
	bool m_bShowLayer0;
	bool m_bShowLayer1;
	bool m_bShowAttribute;
	bool m_bShowGrid;
	// ±ÊË¢Ìù»¨
	CTerrainBrush	m_BrushDecal;
	std::map<uint8,TerrainSub>	m_mapRenderAttributeSubs;
};