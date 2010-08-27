#include "MapSound.h"
#include "Audio.h"
//#include "SceneMgr.h"
//#include "Hero.h"

BOOL CMapSound::s_bStroke = false;

CMapSound::CMapSound()
{
	m_nType = MAP_SOUND;
	m_pSound = NULL;
	m_dwInterval = 0;
	m_bActive = false;
}
//-------------------------------------------------------------
CMapSound::~CMapSound()
{
	m_bFocus = false;
//	if(m_pSound)
//		DXClose2DSound(m_pSound);
}
//-------------------------------------------------------------

void CMapSound::render()const
{
#ifdef _ARHUN_DEBUG
	if(!s_bStroke)
		return;
	static size_t s_uTexSund = 0;
	if(!s_uTexSund)
	{
		s_uTexSund = GetRenderSystem().GetTextureMgr().RegisterTexture("sound.tga");
	}
	Pos2D posScreen;
	GetSceneMgr().World2Screen( m_vPos, posScreen );
	GetGraphics().DrawTex(posScreen.x,posScreen.y,s_uTexSund,0xFFFFFFFF);
#endif
}
//-------------------------------------------------------------
void CMapSound::Process(void* pInfo)
{
#ifdef _ARHUN_DEBUG
	Pos2D posCell;
	GetSceneMgr().World2Cell( m_vPos, posCell );
	if(GetSceneMgr().IsPosVisible(posCell))
	{
		GetSceneMgr().ApplyObjShow(this);
	}
	else
	{
		m_bFocus = false;
	}
#endif
//	if(m_dwInterval == 0)
//		return;
	Pos2D posHero;
	Vec3D vHeroWorldpos;
	//g_objHero.GetCellPos( posHero );
	//GetSceneMgr().Cell2World( posHero, vHeroWorldpos );
	//BOOL bActive;
	if((abs(vHeroWorldpos.x - m_vPos.x) > m_nRange)// && abs(posHero.x - m_vPos.x) > 1000)
		|| (abs(vHeroWorldpos.z - m_vPos.z) > m_nRange))// && abs(posHero.y - m_vPos.y) > 1000))
	{
		if(m_bActive)
		{
			m_bActive = false;
	//		if(Check2DSound(m_pSound))
	//			::DXClose2DSound(m_pSound);
		}
	}
	else
	{
		m_bActive = true;
	}
	
//	if(!Check2DSound(m_pSound) && m_bActive)
	{
		if(!m_bWait)
		{
			m_bWait = true;
			m_dwWaitPoint = ::TimeGet();
		}
		if(::TimeGet() - m_dwWaitPoint > m_dwInterval)
		{
			this->Play();
			m_bWait = false;
		}
	}
}
//-------------------------------------------------------------
BOOL CMapSound::IsFocus()
{
	return m_bFocus;
}

//-------------------------------------------------------------
void CMapSound::setPos(const Vec3D& vPos)
{
	m_vPos = vPos;
//	if(m_pSound)
//		DXSet2DSoundPos(m_pSound, vPos.x, vPos.z);
	m_posCell.x = vPos.x;
	m_posCell.y = vPos.z;
}

bool CMapSound::intersect(const Vec3D& vRayPos , const Vec3D& vRayDir,Vec3D& vCoord)
{
	return false;
}

BOOL CMapSound::Create(const Vec3D& vPos, int nRange, int nVolume, char* pszFile, DWORD dwInterval)
{
	if(!pszFile)
		return false;

	this->setPos( vPos );
	m_nRange = nRange;
	m_nVolume = nVolume;
	m_strFilename = pszFile;
	m_dwInterval = dwInterval; 
//	GetAudio().playSound(m_szFile);
//	m_pSound = (CSound*)GetAudio().Play3DSound(m_szFile, 0, 0,
//											m_nRange, -1, m_nVolume);
	return true;
}
//-------------------------------------------------------------
CMapSound* CMapSound::CreateNew(const Vec3D& vPos, int nRange, int nVolume, char* pszFile, DWORD dwInterval)
{
	CMapSound* pSound = new CMapSound;
	MYASSERT(pSound);
	if(!pSound->Create( vPos, nRange, nVolume, pszFile, dwInterval ))
	{
		S_DEL(pSound);
		return NULL;
	}
	return pSound;
}
//-------------------------------------------------------------
void CMapSound::Play()
{
//	if(m_pSound && Check2DSound(m_pSound))
//		DXClose2DSound(m_pSound);
	if(m_dwInterval == 0)
		//m_pSound = (CSound*)
		GetAudio().Play3DSound(m_strFilename.c_str(), m_vPos.x, m_vPos.z,
											m_nRange, m_nVolume);
	else
		//m_pSound = (CSound*)
		GetAudio().Play3DSound(m_strFilename.c_str(), m_vPos.x, m_vPos.z,
											m_nRange, m_nVolume);
}
//-------------------------------------------------------------
