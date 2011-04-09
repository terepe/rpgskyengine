#include "ModelData.h"
#include "Particle.h"
#include "RenderSystem.h"
#include "Graphics.h"
#include "ParticleData.h"
#include "ModelObject.h"

void CParticleEmitter::init(CParticleData* pData)
{
	m_pData = pData;
	m_nBindingBoneID = m_pData->m_nBoneID;
}

void CParticleEmitter::frameMove(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	Matrix mNewWorld = mWorld*m_mWorldMatrix;
	// ----
	if (m_pParent&&m_pParent->getType()==NODE_MODEL)
	{
		CModelObject* pModel = (CModelObject*)m_pParent;
		const Matrix& matBone = pModel->m_setBonesMatrix[m_pData->m_nBoneID];
		// ----
		mNewWorld = matBone*mNewWorld;
	}
	// ----
	m_pData->update(mNewWorld,*this,fElapsedTime);
	// ----
	CRenderNode::frameMove(mWorld,fTime,fElapsedTime);
}

void CParticleEmitter::render(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType)const
{
	if(!m_pData)
	{
		return;
	}
	CRenderSystem& R = GetRenderSystem();
	// ----
	R.setWorldMatrix(Matrix::UNIT);
	// ----
	CMaterial& material = R.getMaterialMgr().getItem(m_pData->m_strMaterialName.c_str());
	if (!(material.getRenderType()&eRenderType))
	{
		return;
	}
	// ----
	if (R.prepareMaterial(material))
	{
		Vec3D bv0,bv1,bv2,bv3;
		{
			Matrix mbb=Matrix::UNIT;
			if (m_pData->m_bBillboard)
			{
				// 获取公告板矩阵
				Matrix mTrans;
			GetRenderSystem().getViewMatrix(mTrans);
			mTrans.Invert();

			if (m_pData->flags == 569) // Faith shoulders, do cylindrical billboarding
			{
				mbb._11 = 1;
				mbb._31 = 0;
				mbb._13 = 0;
				mbb._33 = 1;
			}
			else
			{
				// everything else, do sphererical billboarding
				//Why after calculating the inverse would you do all this stuff?
				/*Vec3D camera = mTrans * Vec3D(0.0f,0.0f,0.0f);
				Vec3D look = Vec3D(0.0f,0.0f,0.0f);//(camera - pos).normalize();
				Vec3D up = ((mTrans * Vec3D(0,1,0)) - camera).normalize();
				Vec3D right = (up % look).normalize();
				up = (look % right).normalize();

				// calculate the billboard matrix
				mbb._12 = right.x;
				mbb._22 = right.y;
				mbb._32 = right.z;
				mbb._13 = up.x;
				mbb._23 = up.y;
				mbb._33 = up.z;
				mbb._11 = look.x;
				mbb._21 = look.y;
				mbb._31 = look.z;*/

				mbb=mTrans;
				mbb._14=0;
				mbb._24=0;
				mbb._34=0;
			}
			mbb=mTrans;
			mbb._14=0;
			mbb._24=0;
			mbb._34=0;
		}

		if (m_pData->type==0 || m_pData->type==2)			// 正常的粒子
		{
			float f = 0.5;//0.707106781f; // sqrt(2)/2
			if (m_pData->m_bBillboard)
			{
				bv0 = mbb * Vec3D(+f,-f,0);
				bv1 = mbb * Vec3D(+f,+f,0);
				bv2 = mbb * Vec3D(-f,+f,0);
				bv3 = mbb * Vec3D(-f,-f,0);
			}
			else // 平板
			{
				bv0 = Vec3D(-f,0,+f);
				bv1 = Vec3D(+f,0,+f);
				bv2 = Vec3D(+f,0,-f);
				bv3 = Vec3D(-f,0,-f);
			}
			// TODO: per-particle rotation in a non-expensive way?? :|

			CGraphics& bg = GetGraphics();
			bg.begin(VROT_TRIANGLE_LIST, m_Particles.size()*4);
			for (ParticleList::const_iterator it = m_Particles.begin(); it != m_Particles.end(); ++it)
			{
				bg.c(it->color);

				bg.t(m_pData->m_Tiles[it->nTile].tc[0]);
				bg.v(it->vPos + bv0 * it->fSize);

				bg.t(m_pData->m_Tiles[it->nTile].tc[1]);
				bg.v(it->vPos + bv1 * it->fSize);

				bg.t(m_pData->m_Tiles[it->nTile].tc[2]);
				bg.v(it->vPos + bv2 * it->fSize);

				bg.t(m_pData->m_Tiles[it->nTile].tc[3]);
				bg.v(it->vPos + bv3 * it->fSize);
			}
			bg.end();
		}
		else if (m_pData->type==1) // 粒子射线发射器 particles from origin to position
		{
			bv0 = mbb * Vec3D(-1.0f,0,0);
			bv1 = mbb * Vec3D(+1.0f,0,0);

			CGraphics& bg = GetGraphics();
			bg.begin(VROT_TRIANGLE_LIST, m_Particles.size()*4);
			for (ParticleList::const_iterator it = m_Particles.begin(); it != m_Particles.end(); ++it)
			{
				Vec3D P,O;
				P=it->vPos;
				O=it->vOrigin;
				bg.c(it->color);

				bg.t(m_pData->m_Tiles[it->nTile].tc[0]);
				bg.v(it->vPos + bv0 * it->fSize);

				bg.t(m_pData->m_Tiles[it->nTile].tc[1]);
				bg.v(it->vPos + bv1 * it->fSize);

				bg.t(m_pData->m_Tiles[it->nTile].tc[2]);
				bg.v(it->vOrigin + bv1 * it->fSize);

				bg.t(m_pData->m_Tiles[it->nTile].tc[3]);
				bg.v(it->vOrigin + bv0 * it->fSize);
			}
			bg.end();
		}
	}
	}
	R.finishMaterial();
}