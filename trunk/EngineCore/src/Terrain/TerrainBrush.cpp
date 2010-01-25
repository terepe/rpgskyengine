#include "TerrainBrush.h"
#include "RenderSystem.h"

CTerrainBrush::CTerrainBrush() :
m_fX(0),
m_fY(0),
m_fStrength(0.5f),
m_fSize(0.5f),
m_fHardness(0.5f),
m_eBrushType(BRUSH_TYPE_TERRAIN_HEIGHT),
m_eBrushHeightType(BHT_NORMAL),
m_nTileID(0),
m_nTileLayer(0),
m_uAtt(0),
m_uBaseVertexIndex(0),
m_bObjectMoveingClingToFloor(false),
m_fHeightMin(-100),
m_fHeightMax(100),
CTerrainDecal()
{
	UpdateTexTrans();
}

void CTerrainBrush::SetPos(float fX, float fY)
{
	m_fX = fX;
	m_fY = fY;
	m_uBaseVertexIndex = GetBaseVertexIndex(fX, fY);
	UpdateTexTrans();
}

void CTerrainBrush::SetSize(float fSize)
{
	m_fSize = fSize;
	UpdateTexTrans();
}

void CTerrainBrush::SetTileID(int nTileID)
{
	m_nTileID = nTileID;
}

int CTerrainBrush::GetTileID(int nX, int nY)const
{
	return m_nTileID;
}

void CTerrainBrush::SetTileLayer(int nTileLayer)
{
	m_nTileLayer = nTileLayer;
}

int CTerrainBrush::GetTileLayer()const
{
	return m_nTileLayer;
}

void CTerrainBrush::SetAtt(uint8 uAtt)
{
	m_uAtt = uAtt;
}

uint8 CTerrainBrush::GetAtt()const
{
	return m_uAtt;
}

void CTerrainBrush::setHeightRang(float fMin, float fMax)
{
	m_fHeightMin = fMin;
	m_fHeightMax = fMax;
}

float CTerrainBrush::getHeightMin()const
{
	return m_fHeightMin;
}

float CTerrainBrush::getHeightMax()const
{
	return m_fHeightMax;
}

void CTerrainBrush::UpdateTexTrans()
{
	/********************************************************************************
	顶点纹理坐标	＝（顶点纹理坐标偏移处理）×缩放比
					＝（顶点纹理坐标－贴花中心坐标)×缩放比+０.５纹理偏移
					＝顶点纹理坐标×缩放比－贴花中心坐标×缩放比+０.５纹理偏移
	顶点纹理Ｖ		＝１－（顶点纹理Ｖ×缩放比－贴花中心Ｖ×缩放比+０.５纹理偏移）
	********************************************************************************/

	// 缩放比＝半纹理／半径／ＳＩＺＥ
	float fZoom = 0.5f/(m_fRadius*m_fSize);

	// 顶点纹理Ｕ＝顶点纹理Ｕ×缩放比
	// 顶点纹理Ｖ＝－顶点纹理Ｖ×缩放比

	// －贴花中心Ｕ×缩放比＋０.５纹理偏移
	float fPosX = 0.5f-m_fX*fZoom;
	// １－（－贴花中心Ｖ×缩放比＋０.５纹理偏移）
	float fPosY = 0.5f+m_fY*fZoom;
	//
	m_mRandiusTexTrans = Matrix(
		fZoom,0,fPosX,0,
		0,-fZoom,fPosY,0,
		0,0,0,0,
		0,0,0,0);

	fZoom = 0.5f/(m_fRadius*m_fSize*m_fHardness);
	fPosX = 0.5f-m_fX*fZoom;
	fPosY = 0.5f+m_fY*fZoom;
	m_mHardnessTexTrans = Matrix(
		fZoom,0,fPosX,0,
		0,-fZoom,fPosY,0,
		0,0,0,0,
		0,0,0,0);
}

void CTerrainBrush::Render()
{
	if (Prepare())
	{
		CRenderSystem& R = GetRenderSystem();

		// 打开纹理矩阵
		R.setTextureMatrix(0,TTF_COUNT2,m_mRandiusTexTrans);
		R.SetTexture(0,m_nRandiusTexID);
		R.DrawIndexedPrimitive(
			VROT_TRIANGLE_LIST,
			m_uBaseVertexIndex,
			0,
			m_nVertexCount,
			0,
			m_nIBCount/3);
		//
		R.setTextureMatrix(0,TTF_COUNT2,m_mHardnessTexTrans);
		R.SetTexture(0,m_nHardnessTexID);
		R.DrawIndexedPrimitive(
			VROT_TRIANGLE_LIST,
			m_uBaseVertexIndex,
			0,
			m_nVertexCount,
			0,
			m_nIBCount/3);
		//
		R.setTextureMatrix(0,TTF_DISABLE);
		Finish();
	}
}

CTerrainGridDecal::CTerrainGridDecal()
{
	SetVisible(false);
}

void CTerrainGridDecal::SetPos(float fX, float fY)
{
	m_uBaseVertexIndex = GetBaseVertexIndex(fX, fY);
}

void CTerrainGridDecal::Render()
{
	if (Prepare())
	{
		CRenderSystem& R = GetRenderSystem();
		R.setWorldMatrix(Matrix::newTranslation(Vec3D(0.0f,0.1f,0.0f)));
		R.SetLightingEnabled(false);
		R.SetCullingMode(CULL_NONE);
		//R.SetRenderState(D3DRS_FOGENABLE, false);

		R.SetDepthBufferFunc(true,false);
		R.SetAlphaTestFunc(false);
		R.SetBlendFunc(false);

		R.SetTextureFactor(0xFF808000);

		R.SetTextureColorOP(0,TBOP_SOURCE1,TBS_TEXTURE);
		R.SetTextureAlphaOP(0,TBOP_DISABLE);

		// 在里面设置纹理矩阵
		R.setTextureMatrix(0,TTF_COUNT2,m_mTexTrans);
		R.DrawIndexedPrimitive(
			VROT_LINE_STRIP,
			m_uBaseVertexIndex,
			0,
			m_nVertexCount,
			0,
			m_nIBCount-1);
		R.setTextureMatrix(0,TTF_DISABLE);
		Finish();
	}
}

void CTerrainGridDecal::CreateIB()
{
	m_pIB = GetRenderSystem().GetHardwareBufferMgr().CreateIndexBuffer(m_nIBCount);
	if (m_pIB)
	{
		uint16* index = (uint16*)m_pIB->lock(CHardwareBuffer::HBL_NORMAL);
		for (size_t nY = 0; nY <= m_uHeight; ++nY)
		{
			for (size_t nX = 0; nX <= m_uWidth; ++nX)
			{
				int nID = nY * m_nBufferWidth + ((nY%2)?(m_uWidth - nX):nX);
				*index = nID;
				index++;
			}
		}
		for (size_t nX = 0; nX <= m_uWidth; ++nX)
		{
			for (size_t nY = 0; nY <= m_uHeight; ++nY)
			{
				int nID = ((nX%2)?(m_uHeight - nY):nY) * m_nBufferWidth + nX;
				*index = nID;
				index++;
			}
		}
		m_pIB->unlock();
	}
}