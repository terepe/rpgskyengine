#pragma once
#include "3DMapObj.h"

class C3DMapEffect 	:	public C3DMapObj 
{
public:
	static BOOL s_bStroke;
protected:
	BOOL			m_bDelSelf;
	BOOL			m_bDie;
	int				m_nDelay;
	BOOL			m_bSave;
	char			m_szIndex[64];
	float			m_fHorizontal;
	float			m_fVertical;
	BOOL			m_bOver;
	BOOL			m_bSimpleObj;
	////C3DSimpleObj	m_objSimple;
	DWORD			m_dwFrameIndex;

public:
	C3DMapEffect();
	virtual ~C3DMapEffect();

public:
	static  C3DMapEffect* CreateNew( Vec3D vWorldPos, char* pszIndex, BOOL bDelSelf = true, BOOL bSave = false);
	BOOL Create( Vec3D vWorldPos, char* pszIndex, BOOL bDelSelf = true, BOOL bSave = false);

public:
	void SetRotate(float fHorizontal, float fVertical);
	virtual void Show(void* pInfo);
	virtual void Process(void* pInfo);
	virtual int GetObjType(){return MAP_3DEFFECT;}
	virtual void Die(){m_bDie = true;}
public:
	BOOL	NeedSave(){return m_bSave;}
	void	LoadDataObj(FILE* fp);
	void	LoadTextObj(FILE* fp);
	void	SetDir(int nDir);
public:
	BOOL	IsOver(){return m_bOver;}
};