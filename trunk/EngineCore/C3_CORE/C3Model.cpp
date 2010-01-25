#include	<windows.h>
#include	"C3Model.h"
//#include	"basefunc.h"
#include	 <direct.h>

C3DTexture::C3DTexture ()
{
	m_nTexture	= 0;
}

C3DTexture::~C3DTexture ()
{
	this->Destroy ();
}

//------------------------------------------------------
BOOL C3DTexture::Create(const char* pszFileName)
{
	if (!pszFileName)
		return false;
	m_nTexture = GetRenderSystem().GetTextureMgr().RegisterTexture( pszFileName, 3);


	return m_nTexture!=-1;
}	

//------------------------------------------------------
void C3DTexture::Destroy(void)
{
	GetRenderSystem().GetTextureMgr().del( m_nTexture );
}

//------------------------------------------------------
//       ----------- C3DMotion -----------
//------------------------------------------------------
C3DMotion::C3DMotion ()
{
	for ( int n = 0; n < MOTION_MAX; n++ )
		m_motion[n] = 0;

	m_dwMotionNum = 0;
}

//------------------------------------------------------
C3DMotion::~C3DMotion ()
{
	Destroy ();
}

//------------------------------------------------------
bool C3DMotion::IsValid ( void )
{
	if ( m_motion )
		return true;
	return false;
}

//------------------------------------------------------
bool C3DMotion::Create(const std::string& strFilename)
{
	m_dwMotionNum = 0;

	IOReadBase* pRead = IOReadBase::autoOpen(strFilename);
	if (!pRead)
	{
		return false;
	}
	ChunkHeader chunk;
	while (!pRead->IsEof())
	{
		pRead->Read(&chunk, sizeof(ChunkHeader));
		chunk.dwChunkID=(chunk.dwChunkID<<24)+
			((chunk.dwChunkID&0xFF00)<<8)+
			((chunk.dwChunkID&0xFF0000)>>8)+
			(chunk.dwChunkID>>24);
		if ( chunk.dwChunkID == 'MOTI' )
		{
			m_motion[m_dwMotionNum] = C3Motion::New(*pRead);
			if (NULL == m_motion[m_dwMotionNum])
			{
				break;
			}
			m_dwMotionNum++;
		}
		else
		{
			pRead->Move(chunk.dwChunkSize);
		}
	}
	IOReadBase::autoClose(pRead);
	return (m_dwMotionNum>0);
}

//------------------------------------------------------
bool C3DMotion::Destroy ( void )
{
	for (uint32 n = 0; n < m_dwMotionNum; n++ )
	{
		S_DEL( m_motion[n] );
	}

	m_dwMotionNum	=0;
	return true;
}

//------------------------------------------------------
DWORD C3DMotion::GetFrameAmount()
{
	DWORD dwMax	=0;
	for (uint32 n = 0; n < m_dwMotionNum; n++ )
	{
		dwMax	=__max(dwMax, m_motion[n]->dwFrames);
	}

	return dwMax;
}

//------------------------------------------------------
//       ----------- C3DObj -----------
//------------------------------------------------------
C3DObj::C3DObj ()
{
	for ( int n = 0; n < PHY_MAX; n++ )
		m_phy[n] = 0;

	m_dwPhyNum = 0;
}

//------------------------------------------------------
C3DObj::~C3DObj ()
{
	Destroy ();
}

//------------------------------------------------------
bool C3DObj::IsValid ()
{
	if ( m_phy )
		return true;
	return false;
}

//------------------------------------------------------
bool C3DObj::Create(const std::string& strFilename)
{
	m_dwPhyNum = 0;
	for ( int n = 0; n < PHY_MAX; n++ )
		m_phy[n] = NULL;

	IOReadBase* pRead = IOReadBase::autoOpen(strFilename);
	
	if (!pRead)
	{
		return false;
	}
	pRead->Move(16);//fseek(m_fp, 16, SEEK_CUR);
	ChunkHeader chunk;
	while (!pRead->IsEof())
	{
		pRead->Read(&chunk, sizeof(ChunkHeader));
		chunk.dwChunkID=(chunk.dwChunkID<<24)+
			((chunk.dwChunkID&0xFF00)<<8)+
			((chunk.dwChunkID&0xFF0000)>>8)+
			(chunk.dwChunkID>>24);
		if ( chunk.dwChunkID == 'PHY ' )
		{
			m_phy[m_dwPhyNum] = C3Phy::New(*pRead);
			if (NULL==m_phy[m_dwPhyNum])
			{
				break;
			}
			m_dwPhyNum++;
		}
		else
		{
			pRead->Move(chunk.dwChunkSize);
		}
	}
	IOReadBase::autoClose(pRead);

	if (m_dwPhyNum > 0)
		return true;
	else
		return false;
}

//------------------------------------------------------
void C3DObj::Destroy ()
{
	for (uint32 n = 0; n < m_dwPhyNum; n++ )
	{
		S_DEL( m_phy[n] );
	}
}



//------------------------------------------------------
void C3DObj::SetMotion ( C3DMotion* pMotion )
{
	//if (m_dwPhyNum != pMotion->m_dwMotionNum)
	if (m_dwPhyNum > pMotion->m_dwMotionNum)
		return;

	for (uint32 n = 0; n < m_dwPhyNum; n++ )
		m_phy[n]->lpMotion	=pMotion->m_motion[n];
}

//------------------------------------------------------
void C3DObj::SetARGB ( float alpha, float red, float green, float blue )
{
	for (uint32 n = 0; n < m_dwPhyNum; n++ )
	{
		m_phy[n]->SetColor(alpha, red, green, blue);
	}
}

//------------------------------------------------------
int C3DObj::GetIndexByName ( char *lpName )
{
	for (uint32 n = 0; n < m_dwPhyNum; n++ )
	{
		if ( stricmp ( m_phy[n]->lpName, lpName ) == 0 )
			return n;
	}
	return -1;
}

//------------------------------------------------------
void C3DObj::NextFrame ( int nStep )
{
	for (uint32 n = 0; n < m_dwPhyNum; n++ )
	{
		m_phy[n]->NextFrame(nStep);
	}
}

//------------------------------------------------------
void C3DObj::SetFrame (DWORD dwFrame)
{
	for (uint32 n = 0; n < m_dwPhyNum; n++ )
	{
		m_phy[n]->SetFrame(dwFrame);
	}
}

//------------------------------------------------------
void C3DObj::Draw ( int type, float lightx, float lighty, float lightz, float sa, float sr, float sg, float sb )
{
	C3DObj::Prepare();

	for (uint32 n = 0; n < m_dwPhyNum; n++ )
	{
		if ( stricmp( m_phy[n]->lpName, _V_ARMET	) == 0 ||
			stricmp ( m_phy[n]->lpName,	_V_MISC		) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_L_WEAPON ) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_R_WEAPON ) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_L_SHIELD ) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_R_SHIELD ) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_L_SHOE	) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_R_SHOE	) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_PET	) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_BACK	) == 0 ||

			stricmp ( m_phy[n]->lpName, _V_HEAD	) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_L_ARM) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_R_ARM) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_L_LEG) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_R_LEG) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_MANTLE) == 0)

			continue;

		// 画实体
		m_phy[n]->Calculate();
		m_phy[n]->DrawNormal();

		/*switch ( type )
		{
		case 2:
		{
		// 画阴影
		Matrix matrix;
		matrix.unit();
		Common_Rotate ( &matrix, D3DXToRadian ( -45 ), 0, 0 );

		Vec4D	pos;
		pos.x = lightx;
		pos.y = lighty;
		pos.z = lightz;
		pos.w = 1;
		D3DXVec3TransformCoord ( ( Vec3D* )&pos, ( Vec3D* )&pos, &matrix );

		pos.x += m_phy[n]->lpMotion->matrix[0]._41;
		pos.z += m_phy[n]->lpMotion->matrix[0]._43;

		Vec3D point;
		point.x = m_phy[n]->lpMotion->matrix[0]._41;
		point.y = m_phy[n]->lpMotion->matrix[0]._42;
		point.z = m_phy[n]->lpMotion->matrix[0]._43;

		Vec3D normal;
		normal.x = 0;
		normal.y = 0;
		normal.z = -1;
		D3DXVec3TransformCoord ( &normal, &normal, &matrix );
		D3DXVec3Normalize ( &normal, &normal );

		int tex = m_phy[n]->nTex;
		m_phy[n]->nTex = -1;
		m_phy[n]->SetColor(sa, sr, sg, sb);

		matrix.unit();
		//Common_Shadow ( &matrix, &pos, &point, &normal );
		for ( int b = 0; b < m_phy[n]->lpMotion->dwBoneCount; b++ )
		m_phy[n]->Muliply( b, &matrix );
		Phy_Draw ( m_phy[n] );

		m_phy[n]->nTex = tex;
		}
		break;
		};*/
		//m_phy[n]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
		//m_phy[n]->ClearMatrix();
	}
}
void C3DObj::DrawAlpha ( int type, float lightx, float lighty, float lightz, float sa, float sr, float sg, float sb, float height )
{
	C3DObj::Prepare();

	for (uint32 n = 0; n < m_dwPhyNum; n++ )
	{
		if ( stricmp( m_phy[n]->lpName,	_V_ARMET	) == 0 ||
			stricmp ( m_phy[n]->lpName,	_V_MISC		) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_L_WEAPON ) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_R_WEAPON ) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_L_SHIELD ) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_R_SHIELD ) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_L_SHOE	) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_R_SHOE	) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_PET	) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_BACK	) == 0 ||

			stricmp ( m_phy[n]->lpName, _V_HEAD	) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_L_ARM) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_R_ARM) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_L_LEG) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_R_LEG) == 0 ||
			stricmp ( m_phy[n]->lpName, _V_MANTLE) == 0)


			continue;

		// 画实体
		//m_phy[n]->Calculate();
		m_phy[n]->DrawAlpha(true);

		switch ( type )
		{
		case 2:
			{
				// 画阴影
				Matrix matrix;
				matrix.unit();
				//Common_Rotate ( &matrix, D3DXToRadian ( -37 ), 0, 0 );

				Vec4D	pos;
				pos.x = lightx;
				pos.y = lighty;
				pos.z = lightz;
				pos.w = 1;
				//>>		D3DXVec3TransformCoord ( ( Vec3D* )&pos, ( Vec3D* )&pos, &matrix );

				//pos.x += m_x[n];//m_phy[n]->lpMotion->matrix[0]._41;
				//pos.z += m_z[n];//m_phy[n]->lpMotion->matrix[0]._43 - height;

				Vec3D point;


				/*point.x = m_phy[n]->lpMotion->matrix[0]._41;
				point.y = m_phy[n]->lpMotion->matrix[0]._42;
				point.z = m_phy[n]->lpMotion->matrix[0]._43 - height;*/


				point.x = 0;
				point.y = 0;
				point.z = 0 - height;

				Vec3D normal;
				normal.x = 0;
				normal.y = 0;
				normal.z = -1;
				//>>		D3DXVec3TransformCoord ( &normal, &normal, &matrix );
				//>>			D3DXVec3Normalize ( &normal, &normal );

				//int tex = m_phy[n]->nTex;
				//m_phy[n]->nTex = -1;
				m_phy[n]->SetColor(sa, sr, sg, sb);
				//GetRenderSystem().SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );

				matrix.unit();
				//Common_Shadow ( &matrix, &pos, &point, &normal );
				Matrix mWorld;
				GetRenderSystem().getWorldMatrix(mWorld);
				mWorld = mWorld*matrix;
				GetRenderSystem().setWorldMatrix( mWorld );
				//Matrix mUnit;mUnit.unit();GetRenderSystem().SetWorldMatrix( mUnit );
				m_phy[n]->Calculate();
				m_phy[n]->DrawNormal();
				m_phy[n]->DrawAlpha(true);

				//GetRenderSystem().SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

				//m_phy[n]->nTex = tex;
			}
			break;
		};
		for (uint32 n = 0; n < m_dwPhyNum; n++ )
		{
			m_phy[n]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
			//			m_phy[n]->ClearMatrix();
		}
	}
}
//------------------------------------------------------
void C3DObj::ChangeTexture ( C3DTexture* pTexture, char *objname )
{
	if (!pTexture)
		return;

	if (!objname)
	{
		for (uint32 i=0; i<m_dwPhyNum; i++)
		{
			m_phy[i]->ChangeTexture(pTexture->GetTexture());
		}
	}
	else
	{
		int nIndex = GetIndexByName ( objname );
		if (nIndex < 0)
			nIndex = 0;

		m_phy[nIndex]->ChangeTexture(pTexture->GetTexture());
	}
}

//------------------------------------------------------
void C3DObj::Prepare ( void )
{
	C3Phy::Prepare();
}
//------------------------------------------------------
void C3DObj::ClearMatrix ( void )
{
	for (uint32 n = 0; n < m_dwPhyNum; n++)
	{
		m_phy[n]->ClearMatrix();
	}
}