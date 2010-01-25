#pragma once
#include "MapObj.h"

class CSound;
class CMapSound : public CMapObj
{
public:
	CMapSound();
	virtual ~CMapSound();
private:
	Pos2D	m_posCell;
	BOOL	m_bFocus;
	CSound* m_pSound;
	// info
	int		m_nRange;
	int		m_nVolume;
	std::string	m_strFilename;
	DWORD	m_dwInterval;
	BOOL	m_bWait;
	DWORD	m_dwWaitPoint;
	BOOL	m_bActive;
public:
	static BOOL s_bStroke;
public:
	virtual Pos2D getCellPos(){return m_posCell;}
	virtual void SetCellPos( Pos2D& posCell ){m_posCell = posCell;}

	virtual void render()const;
	virtual void Process(void* pInfo);
	virtual int  GetObjType(){return MAP_SOUND;}
	virtual BOOL IsFocus();
	virtual void setPos(Vec3D& vPos);

	virtual bool intersect(const Vec3D& vRayPos , const Vec3D& vRayDir,Vec3D& vCoord);
	void   Play();

	BOOL   Create(Vec3D vPos, int nRange, int nVolume, char* pszFile, DWORD dwInterval);
	static CMapSound* CreateNew(Vec3D vPos, int nRange, int nVolume, char* pszFile, DWORD dwInterval);
};