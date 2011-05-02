#pragma once
#include "RenderNode.h"

class CHardwareVertexBuffer;
class CLodMesh;
class CSkinModel: public CRenderNode
{
public:
	CSkinModel();
	~CSkinModel();
	virtual int		getType			() {return NODE_MODEL;}
	virtual void	frameMove		(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void	render			(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
	virtual bool	intersectSelf	(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax)
	void			setMesh			(CLodMesh* pMesh);
	bool			Prepare			()const;
	void			SetLOD			(unsigned long uLodID);		// …Ë÷√LodID
	void			SetLightMap		(const char* szFilename);	// SetLightMap

protected:
	CHardwareVertexBuffer*			m_pVB;				// ∂•µ„ª∫≥Â
	CLodMesh*						m_pMesh;
	unsigned long					m_uLodLevel;		// Current Lod Level
	unsigned long					m_uLightMapTex;		//
	bool							m_bLightmap;
};