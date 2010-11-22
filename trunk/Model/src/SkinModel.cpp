#include "SkinModel.h"

void CSkinModel::frameMove(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	//animate(fElapsedTime);
	// ----
	if (m_pParent&&m_pParent->getType()==NODEL_MODEL)
	{
		CModelObject* pModel = (CModelObject*)m_pParent;
		// ----
		if (m_pModelData)
		{
			if (m_pModelData->m_Mesh.m_bSkinMesh)
			{
				m_pModelData->m_Mesh.skinningMesh(m_pVB, pModel->m_setBonesMatrix);
			}
		}
	}
	// ----
	CRenderNodel::frameMove(mWorld,fTime,fElapsedTime);
}