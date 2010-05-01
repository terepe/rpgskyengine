#pragma once
#include "TerrainData.h"
#include "Frustum.h"
#include "HardwareIndexBuffer.h"

class DLL_EXPORT CTerrainDecal
{
public:
	CTerrainDecal();
	void setMaterial(const std::string& strMaterial);
	virtual void create(size_t uBufferWidth, float fRadius);
	virtual void createBySize(size_t uBufferWidth, size_t uWidth, size_t uHeight);
	virtual bool setIB();
	virtual void Draw(float fX, float fY);
	virtual void Draw(uint32 uBaseVertexIndex);
	float GetRadius(){return m_fRadius;}
public:
	void	SetVisible(bool bVisible){m_bVisible = bVisible;}
	bool	IsVisible(){return m_bVisible;}
	CHardwareIndexBuffer* GetIB(){return m_pIB;}
protected:
	int GalcLogicalLengthByRadius(float fRadius);
	virtual void CreateIB();
	virtual uint32 GetBaseVertexIndex(float fX, float fY);
	CHardwareIndexBuffer*	m_pIB;
	int						m_nBufferWidth;
	size_t					m_uWidth;
	size_t					m_uHeight;
	int						m_nIBCount;
	int						m_nVertexCount;
	std::string					m_strMaterial;
	float					m_fRadius;
	bool					m_bVisible;
};