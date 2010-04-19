#include "2DMapObj.h"

C2DMapObj::C2DMapObj()
{
}

C2DMapObj::~C2DMapObj()
{
}

void C2DMapObj::GetWorldPos( Vec3D& vWorldPos )
{
	//GetSceneMgr().Cell2World( m_posCell, vWorldPos );
}

void C2DMapObj::GetCellPos( Pos2D& posCell )
{
	posCell = m_posCell;
}

void C2DMapObj::SetCellPos( Pos2D& posCell )
{
	m_posCell = posCell;
}