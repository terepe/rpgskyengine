#pragma once
#include "Terrain.h"
#include "TerrainBrush.h"
struct EditLog
{
	int type,x,y,value;
};
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
	void showLayer0(bool bShow){m_bShowLayer0 = bShow;}
	void showLayer1(bool bShow){m_bShowLayer1 = bShow;}
	void showAttribute(bool bShow){m_bShowAttribute = bShow;}
	void showGrid(bool bShow){m_bShowGrid = bShow;}
	void showBrushDecal(bool bShow){m_bShowBrushDecal = bShow;}
	
	bool isShowLayer0(){return m_bShowLayer0;}
	bool isShowLayer1(){return m_bShowLayer1;}
	bool isShowAttribute(){return m_bShowAttribute;}
	bool isShowGrid(){return m_bShowGrid;}
	bool isShowBrushDecal(){return m_bShowBrushDecal;}

	void	CreateIB();
	void	updateIB();
protected:
	void createBrush();
	virtual bool create();
	bool m_bShowLayer0;
	bool m_bShowLayer1;
	bool m_bShowAttribute;
	bool m_bShowGrid;
	bool m_bShowBrushDecal;
	// ±ÊË¢Ìù»¨
	CTerrainBrush	m_BrushDecal;
	std::map<uint8,TerrainSub>	m_mapRenderAttributeSubs;
};