#pragma once
#include "Terrain.h"
#include "TerrainBrush.h"

struct EditValue
{
	union 
	{
		float fHeight;
		uint8 uAtt;
		unsigned long color;
		struct {
			byte id;
			byte layer;
		} tile;
	};
};

struct EditTarget
{
	int type;
	Pos2D pos;
	bool operator< (const EditTarget &et) const
	{
		if(type!=et.type)
		{
			return type<et.type;
		}
		if(pos.y!=et.pos.y)
		{
			return pos.y<et.pos.y;
		}
		if(pos.x!=et.pos.x)
		{
			return pos.x<et.pos.x;
		}
		return false;
	}
};

struct EditRecord
{
	std::map<EditTarget,EditValue> mapItem;
};
typedef std::map<EditTarget,EditValue>  MAP_EDIT_RECORD;

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
	void markEdit();
	void doEdit(MAP_EDIT_RECORD& mapEditRecordIn,MAP_EDIT_RECORD& mapEditRecordOut);
	void doEdit(MAP_EDIT_RECORD& mapEditRecordIn);
	void doEdit(std::vector<MAP_EDIT_RECORD>& mapEditRecordIn,std::vector<MAP_EDIT_RECORD>& mapEditRecordOut);
	void rebackEdit();
	void redoEdit();
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

	std::vector<MAP_EDIT_RECORD> m_setReback;
	std::vector<MAP_EDIT_RECORD> m_setRedo;

};