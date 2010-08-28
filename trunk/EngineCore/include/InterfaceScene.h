#pragma once
#include "InterfaceDataPlugsBase.h"
#include "Common.h"
#include "Vec4D.h"
#include "Vec2D.h"
#include "Color.h"
#include "Pos2D.h"
#include "Material.h"
//////////////////////////////////////////////////////////////////////////
// TerrainData
//////////////////////////////////////////////////////////////////////////
struct TerrainVertex
{
	//enum { FVF = (FVF_XYZ | FVF_NORMAL | FVF_DIFFUSE | FVF_TEX3) };
	Vec3D	p;
	Vec3D	n;
	Color32	c;
	Vec2D	t0;
	Vec2D	t1;
};

struct TerrainCell
{
	unsigned char	uTileID[2];
	Color32			color;
	Color32			lightColor;
	float			fHeight;
	Vec3D			vNormals;
	unsigned char	uAttribute;
	unsigned char	uOther;
	Vec2D			vEquableTexUV;
	bool			bSearched;
};

class iTerrainData
{
public:
	iTerrainData(){};
	virtual ~iTerrainData(){};
	//
	virtual void clear()=0;
	virtual void Create(size_t width, size_t height, size_t cubeSize)=0;
	virtual bool resize(size_t width, size_t height, size_t cubeSize)=0;
	//
	virtual int	GetVertexXCount()const=0;
	virtual int	GetVertexYCount()const=0;
	virtual int	GetVertexCount()const=0;
	//
	virtual int GetWidth()const=0;
	virtual int GetHeight()const=0;
	virtual int GetCubeSize()const=0;
	virtual int GetCellCount()const=0;
	//
	virtual bool isCellIn(const Pos2D& posCell)const=0;
	virtual bool isPointIn(const Pos2D& posCell)const=0;
	//
	virtual uint8	GetCellTileID(const Pos2D& posCell, size_t layer = 0)const=0;
	virtual void	SetCellTileID(const Pos2D& posCell, uint8 uTileID, size_t layer = 0)=0;
	//
	virtual uint32	GetVertexIndex(const Pos2D& posCell)const=0;
	virtual int		GetCellXByVertexIndex(uint32 uVertexIndex)const=0;
	virtual int		GetCellYByVertexIndex(uint32 uVertexIndex)const=0;
	virtual Pos2D	GetCellPosByVertexIndex(uint32 uVertexIndex)const=0;
	//
	virtual float	getVertexHeight(const Pos2D& posCell)const=0;
	virtual void	setVertexHeight(const Pos2D& posCell, float fHeight)=0;
	//
	virtual Vec3D	getVertexNormal(const Pos2D& posCell)const=0;
	//
	virtual uint8	getCellAttribute(const Pos2D& posCell)const=0;
	virtual void	setCellAttribute(const Pos2D& posCell, uint8 uAtt)=0;

	virtual bool	isCellSearched(const Pos2D& posCell)const=0;
	virtual void	setCellSearched(const Pos2D& posCell, bool bSearched)=0;
	//
	virtual Color32	getVertexColor(const Pos2D& posCell)const=0;
	virtual void	setVertexColor(const Pos2D& posCell, Color32 color)=0;
	//
	virtual float	GetHeight(const Vec2D& vPos)const=0;
	virtual void	SetHeight(const Vec2D& vPos, float fHeight)=0;
	virtual Vec4D	GetColor(const Vec2D& vPos)const=0;
	//
	virtual bool	PickCell(const Pos2D& posCell, const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D* pPos = NULL)const=0;
	virtual bool	Pick(const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D* pPos = NULL)const=0;
	//
	virtual void	CalcTexUV()=0;
	//
	virtual const std::string& getFilename()const=0;
	//
	virtual Vec3D	GetLightDir()const=0;

	virtual void	getVertexByCell(int nCellX, int nCellY, TerrainVertex& vertex)const=0;
	virtual void	getGrassVertexByCell(int nCellX, int nCellY, TerrainVertex*& vertex)const=0;

	virtual bool	hasGrass(int nCellX, int nCellY)const=0;

	virtual std::vector<TerrainCell>& getCells()=0;

protected:
	virtual float	GetCellXLength(const Pos2D& posCell)const=0;
	virtual float	GetCellYLength(const Pos2D& posCell)const=0;
	virtual float	GetXLength(int nCellY)const=0;
	virtual float	GetYLength(int nCellX)const=0;
};

//////////////////////////////////////////////////////////////////////////
// Terrain
//////////////////////////////////////////////////////////////////////////
#include "Frustum.h"

struct Cube
{
	typedef std::vector<const Cube*>		LIST_CUBES;
	Cube()
	{
		pChildCube=NULL;
	}
	~Cube()
	{
		S_DELS(pChildCube);
	}
	Cube* pChildCube;
	BBox bbox;

	void createChildCube(size_t size)
	{
		int nWidth = int((bbox.vMax.x-bbox.vMin.x)/size);
		int nHeight = int((bbox.vMax.z-bbox.vMin.z)/size);
		if (1<nWidth||1<nHeight)
		{
			pChildCube = new Cube[2];
			pChildCube[0].bbox = bbox;
			pChildCube[1].bbox = bbox;
			if (nWidth>=nHeight)
			{
				float fX = bbox.vMin.x+(int(nWidth/2))*size;
				pChildCube[0].bbox.vMax.x = fX;
				pChildCube[1].bbox.vMin.x = fX;
			}
			else
			{
				float fZ = bbox.vMin.z+(int(nHeight/2))*size;
				pChildCube[0].bbox.vMax.z = fZ;
				pChildCube[1].bbox.vMin.z = fZ;
			}
			pChildCube[0].createChildCube(size);
			pChildCube[1].createChildCube(size);
		}
	}

	Pos2D getMinCellPos()const
	{
		return Pos2D((int)bbox.vMin.x,(int)bbox.vMin.z);
	};

	void getChildCubesByFrustum(const CFrustum& frustum, LIST_CUBES& CubeList)const
	{
		CrossRet crossRet = frustum.CheckAABBVisible(bbox);

		if (cross_include == crossRet)
		{
			CubeList.push_back(this);
		}
		else if (cross_cross == crossRet)
		{
			if (pChildCube)
			{
				for (int i=0; i<2; ++i)
				{
					pChildCube[i].getChildCubesByFrustum(frustum, CubeList);
				}
			}
			else
			{
				CubeList.push_back(this);
			}
		}
	}

	void getChildCrunodeCubes(LIST_CUBES& CubeList)const
	{
		if (pChildCube)
		{
			for (int i=0; i<2; ++i)
			{
				pChildCube[i].getChildCrunodeCubes(CubeList);
			}
		}
		else
		{
			CubeList.push_back(this);
		}
	}

	bool isPosIn(const Pos2D& posCell)
	{
		return posCell.x>=bbox.vMin.x&&
			posCell.x<bbox.vMax.x&&
			posCell.y>=bbox.vMin.y&&
			posCell.y<bbox.vMax.y;
	}

	Cube* getCrunodeCubeByPos(const Pos2D& posCell)
	{
		if (pChildCube==NULL)
		{
			return this;
		}
		for (int i = 0; i<2; i++)
		{
			if (pChildCube[i].isPosIn(posCell))
			{
				return getCrunodeCubeByPos(posCell);
			}
		}
		return NULL;
	}
};

class CTerrainDecal;

class iTerrain
{
public:
	typedef std::map<int,std::string>		MAP_TILES;
	typedef std::vector<const Cube*>		LIST_CUBES;
	virtual void setTileMaterial(int nTileID, const std::string& strMaterialName)=0;
	virtual CMaterial& getMaterial(const std::string& strMaterialName)=0;
	virtual iTerrainData& GetData()=0;
	virtual const iTerrainData& GetData()const=0;

	virtual CTerrainDecal& GetLightMapDecal()=0;

	virtual void getCubesByFrustum(const CFrustum& frustum, LIST_CUBES& CubeList)const=0;
	virtual void getCrunodeCubes(LIST_CUBES& CubeList)const=0;

	virtual bool Prepare()=0;
	virtual void DrawChunk(const Cube& cube)=0;

	virtual void clearAllTiles()=0;

	virtual void setLightMapTexture(const std::string& strFilename)=0;

	virtual bool create()=0;
};


//////////////////////////////////////////////////////////////////////////
// Scene
//////////////////////////////////////////////////////////////////////////
#include "MapObj.h"
#include "RenderSystemCommon.h"
class iScene
{
public:
	virtual iTerrain* getTerrain()=0;
	virtual const iTerrain*	getTerrain()const=0;
	virtual void clearObjectResources()=0;
	virtual void setObjectResources(int64 uID,const std::string& strName,const std::string& strFilename)=0;
	virtual void createObjectTree(const BBox& box, size_t size)=0;
	virtual CMapObj* add3DMapSceneObj(int64 uID,const Vec3D& vPos,const Vec3D& vRotate,const Vec3D& vScale)=0;
	virtual void removeAllObjects()=0;
	virtual void getAllObjects(DEQUE_MAPOBJ&  setObject)=0;

	virtual void setFog(const Fog& fog)=0;
	virtual const Fog& getFog()=0;
	virtual void setLight(const DirectionalLight& light)=0;
	virtual const DirectionalLight& getLight()=0;
};

//////////////////////////////////////////////////////////////////////////
// Plug
//////////////////////////////////////////////////////////////////////////
enum E_SCENE_PLUGIN_TYPE
{
	E_SCENE_PLUGIN_EXPORT	= 1,
	E_SCENE_PLUGIN_IMPORT	= 2,
	E_SCENE_PLUGIN_TOOL		= 3,
};

class CScenePlugBase:public CDataPlugBase
{
public:
	CScenePlugBase(){};
	virtual ~CScenePlugBase(){};

	virtual E_SCENE_PLUGIN_TYPE GetType()	= 0;
	virtual const char * GetTitle()		= 0;
	virtual const char * GetFormat()	= 0;
	virtual int Execute(iScene * pScene, bool bShowDlg, bool bSpecifyFileName) = 0;
	virtual int importData(iScene * pScene, const std::string& strFilename)=0;
	virtual int exportData(iScene * pScene, const std::string& strFilename)=0;

	virtual DWORD GetExportDataType() = 0;
	virtual DWORD GetImportDataType() = 0;

	virtual void Release() = 0;
};