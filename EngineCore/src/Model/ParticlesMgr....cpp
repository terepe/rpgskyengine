#include "ParticlesMgr.h"
#include <fstream>
#include "D3D9RenderSystem.h"

CParticlesMgr* GetParticlesMgr()
{
	static CParticlesMgr g_ParticlesMgr;
	return &g_ParticlesMgr;
};

CParticlesMgr::CParticlesMgr(void)
{
	m_pVB = NULL;
	m_pIB = NULL;
}

CParticlesMgr::~CParticlesMgr(void)
{
	void*	pV = NULL;		// 顶点缓冲 lock的时候得到的值
	if (m_pVB)
	{
		m_pVB->Lock( 0, 0, (void**)&pV, 0 );
		m_pVB->Unlock();
	}
	S_REL( m_pVB );
	S_REL( m_pIB );
}

void CParticlesMgr::OnResetDevice()
{
	if (m_pVB == NULL)
	{
		m_pDevice->CreateVertexBuffer( 10000 * sizeof(VERTEX_XYZ_DIF_TEX), D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, VERTEX_XYZ_DIF_TEX::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL );
	}
}

void CParticlesMgr::OnLostDevice()
{
	S_REL( m_pVB );
}

void CParticlesMgr::OnDestroyDevice()
{
	S_REL( m_pVB );
}

void CParticlesMgr::Render()
{
	if ( BeginDraw() )
	{
		for ( int i = 0; i < m_Attribs.size(); i++ )
		{
			DrawSubset( i );
		}
	}
	EndDraw();
}

bool CParticlesMgr::BeginDraw()
{
	if ( m_pVB == NULL || m_pIB == NULL )
	{
		return false;
	}
	CD3DRenderSystem& R = GetRenderSystem();
	R->SetFVF( m_dwFVF );
	r->SetStreamSource( 0, m_pVB, 0, m_dwVertexSize );
	r->SetIndices( m_pIB );	
	return true;
}

void CParticlesMgr::DrawSubset( DWORD AttribId )
{
	CRenderState* r = GetRenderState();
	LPATTRIBUTERANGE pAttrib = &m_Attribs[AttribId];
	if ( r->Commit() )// 检测状态是否正确
	{
		r->GetDevice()->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,//(D3DPRIMITIVETYPE)
			0,
			pAttrib->VertexStart,
			pAttrib->VertexCount,
			pAttrib->IndexStart,
			pAttrib->IndexCount/3 );
	}
}
bool CParticlesMgr::EndDraw()
{
	return true;
}

void CParticlesMgr::Update()
{
}