#pragma once
#include "Pos2D.h"
#include "Vec3D.h"
#include "Frustum.h"
#include "ModelObject.h"

enum
{
		MAP_NONE                = 0,
        MAP_TERRAIN             = 1,
        MAP_TERRAIN_PART        = 2,
        MAP_SCENE               = 3,
        MAP_COVER               = 4,
        MAP_ROLE                = 5,
		MAP_HERO				= 6,
		MAP_PLAYER				= 7,
		MAP_3DSIMPLE			= 9,
		MAP_3DEFFECT			= 10,
		MAP_2DITEM				= 11,
		MAP_3DNPC				= 12,
		MAP_3DOBJ				= 13,
		MAP_3DTRACE				= 14,
		MAP_SOUND				= 15,
		MAP_2DREGION			= 16,
		MAP_3DMAGICMAPITEM		= 17,
		MAP_3DITEM				= 18,
		MAP_3DEFFECTNEW			= 19,
};

class CMapObj :public CModelObject
{
public:
	CMapObj()													{m_nType = MAP_NONE;}
	virtual ~CMapObj(void)										{}

public:
	virtual void			renderFocus	()const					{}
	virtual void			renderDebug	()const					{}

	virtual int				GetObjType	()						{return m_nType;}
	virtual void			release		()						{delete this;}



	virtual bool			intersect	(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax){return false;}
	virtual BBox			getBBox		()const					{return BBox();}

	virtual Pos2D			getCellPos	()						{return Pos2D();}
	virtual void			SetCellPos	( Pos2D& posCell )		{}

	virtual bool			IsFocus		()						{return false;}

	virtual void			SetARGB		(unsigned long dwARGB)	{m_dwARGB = dwARGB;}
	virtual unsigned long	GetARGB		()						{return m_dwARGB;}
	virtual int				getOrder	()						{return m_nOrder;}
	virtual void			setOrder	(int nOrder)			{m_nOrder=nOrder;}
public:
	int				m_nType;
	unsigned long	m_dwARGB;
	int				m_nOrder;
};
#include <deque>
typedef std::deque<CMapObj*>  DEQUE_MAPOBJ;