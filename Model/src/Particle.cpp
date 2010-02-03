#include "ModelData.h"
#include "Particle.h"
#include "RenderSystem.h"
#include "Graphics.h"
#include "ParticleEmitter.h"

void CParticleGroup::Init(CParticleEmitter* pEmitter, CBone* pBone)
{
	m_pEmitter = pEmitter;
	m_pBone = pBone;
	m_uTexID = m_pEmitter->uTexID;
}

void CParticleGroup::update(float fElapsedTime)
{
	if (m_pEmitter==NULL)
	{
		return;
	}
	//m_pEmitter->update(this, fElapsedTime);

}

void CParticleGroup::SetTex(int nTexID)
{
	m_uTexID = nTexID;
}

void CParticleGroup::Setup(int nTime)
{
	m_nTime = nTime;

	/*
	if (transform) {
		// transform every particle by the parent trans matrix   - apparently this isn't needed
		Matrix m = pBone->m_mat;
		for (ParticleList::iterator it = m_Particles.begin(); it != m_Particles.end(); ++it) {
			it->tpos = m * it->pos;
		}
	} else {
		for (ParticleList::iterator it = m_Particles.begin(); it != m_Particles.end(); ++it) {
			it->tpos = it->pos;
		}
	}
	*/
}
bool CParticleGroup::passBegin(E_MATERIAL_RENDER_TYPE eRenderType)const
{
	if(m_pEmitter)
	{
		return false;
	}
	if (!(m_pEmitter->m_Material.getRenderType()&eRenderType))
	{
		return false;
	}

	CRenderSystem& R = GetRenderSystem();
	//// 设置混合模式
	//switch (m_pEmitter->m_nBlend)
	//{
	//case 0:	// 透明 镂空
	//	R.SetBlendFunc(true, BLENDOP_ADD, SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA);
	//	R.SetAlphaTestFunc(true);
	//	break;
	//case 1:	//MODULATE 加亮
	//	R.SetBlendFunc(true, BLENDOP_ADD, SBF_SOURCE_COLOUR, SBF_ONE);
	//	R.SetAlphaTestFunc(false);
	//	break;
	//case 2: // 透明
	//	R.SetBlendFunc(true, BLENDOP_ADD, SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA);
	//	R.SetAlphaTestFunc(false);
	//	break;
	//case 3:	// 镂空
	//	R.SetBlendFunc(false);
	//	R.SetAlphaTestFunc(true);
	//	break;
	//case 4: // 加亮
	//	R.SetBlendFunc(true, BLENDOP_ADD, SBF_SOURCE_ALPHA, SBF_ONE);
	//	R.SetAlphaTestFunc(false);
	//	break;
	//}
	R.SetCullingMode(CULL_NONE);
	R.SetLightingEnabled(false);
	R.SetDepthBufferFunc(true, false);

	R.SetTextureColorOP(0,TBOP_MODULATE, TBS_TEXTURE, TBS_DIFFUSE);
	R.SetTextureAlphaOP(0,TBOP_MODULATE, TBS_TEXTURE, TBS_DIFFUSE);

	R.SetTexture(0 , m_uTexID);
}

void CParticleGroup::passEnd()const
{

}

void CParticleGroup::draw()const
{
	Vec3D bv0,bv1,bv2,bv3;
	//if (/*supportPointSprites &&*/ m_pEmitter->m_nRows==1 && m_pEmitter->m_nCols==1&&false) {
	/*	//// This is how will our point sprite's size will be modified by 
		//// distance from the viewer
		//float quadratic[] = {0.1f, 0.0f, 0.5f};
		////float quadratic[] = {0.88f, 0.001f, 0.000004f};
		//glPointParameterfvARB(GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic);

		//// Query for the max point size supported by the hardware
		float maxSize = 1.0f;
		////glGetFloatv(GL_POINT_SIZE_MAX_ARB, &maxSize);

		//// Clamp size to 100.0f or the sprites could get a little too big on some  
		//// of the newer graphic cards. My ATI card at home supports a max point 
		//// size of 1024.0f!
		////if(maxSize > 100.0f)
		////	maxSize = 100.0f;

		//glPointSize(maxSize);

		//// The alpha of a point is calculated to allow the fading of points 
		//// instead of shrinking them past a defined threshold size. The threshold 
		//// is defined by GL_POINT_FADE_THRESHOLD_SIZE_ARB and is not clamped to 
		//// the minimum and maximum point sizes.
		//glPointParameterfARB(GL_POINT_FADE_THRESHOLD_SIZE_ARB, 60.0f);

		//glPointParameterfARB(GL_POINT_SIZE_MIN_ARB, 1.0f);
		//glPointParameterfARB(GL_POINT_SIZE_MAX_ARB, maxSize);

		//// Specify point sprite texture coordinate replacement mode for each texture unit
		//glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
		//// Render point sprites...
		//glEnable(GL_POINT_SPRITE_ARB);

		//glBegin(GL_POINTS);
		//{
		//	for (ParticleList::iterator it = m_Particles.begin(); it != m_Particles.end(); ++it) {
		//		glPointSize(it->size);
		//		glTexCoord2fv(m_Tiles[it->tile].tc[0]);
		//		glColor4fv(it->color);
		//		glVertex3fv(it->pos);
		//	}
		//}
		//glEnd();

		//glDisable(GL_POINT_SPRITE_ARB);
		//glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_FALSE);

		R.SetRenderState(D3DRS_POINTSPRITEENABLE, true);
		R.SetRenderState(D3DRS_POINTSCALEENABLE, true);

		R.SetRenderState(D3DRS_POINTSIZE, static_cast<uint32>(maxSize);

		CGraphics* bg = &GetGraphics();
		bg->Begin(BGMODE_POIN, m_Particles.size());
		for (ParticleList::iterator it = m_Particles.begin(); it != m_Particles.end(); ++it)
		{
			bg->Color32(it->color.c);
			bg->TexCoord2fv(m_pEmitter->m_Tiles[it->nTile].tc[0]);
			bg->Vertex3fv(it->vPos);
		}
		bg->End();
		R.SetRenderState(D3DRS_POINTSPRITEENABLE, false);
		R.SetRenderState(D3DRS_POINTSCALEENABLE, false);
	}
	else*/
	{
		Matrix mbb=Matrix::UNIT;
		if (m_pEmitter->m_bBillboard)
		{
			// 获取公告板矩阵
			Matrix mTrans;
			GetRenderSystem().getViewMatrix(mTrans);
			mTrans.Invert();

			if (m_pEmitter->flags == 569) // Faith shoulders, do cylindrical billboarding
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
		}

		if (m_pEmitter->type==0 || m_pEmitter->type==2)			// 正常的粒子
		{
			float f = 0.5;//0.707106781f; // sqrt(2)/2
			if (m_pEmitter->m_bBillboard)
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

			CGraphics* bg = &GetGraphics();
			bg->Begin(BGMODE_QUADS, m_Particles.size()*4);
			for (ParticleList::const_iterator it = m_Particles.begin(); it != m_Particles.end(); ++it)
			{
				bg->Color(it->color);

				bg->TexCoord2fv(m_pEmitter->m_Tiles[it->nTile].tc[0]);
				bg->Vertex3fv(it->vPos + bv0 * it->fSize);

				bg->TexCoord2fv(m_pEmitter->m_Tiles[it->nTile].tc[1]);
				bg->Vertex3fv(it->vPos + bv1 * it->fSize);

				bg->TexCoord2fv(m_pEmitter->m_Tiles[it->nTile].tc[2]);
				bg->Vertex3fv(it->vPos + bv2 * it->fSize);

				bg->TexCoord2fv(m_pEmitter->m_Tiles[it->nTile].tc[3]);
				bg->Vertex3fv(it->vPos + bv3 * it->fSize);
			}
			bg->End();
		}
		else if (m_pEmitter->type==1) // 粒子射线发射器 particles from origin to position
		{
			bv0 = mbb * Vec3D(-1.0f,0,0);
			bv1 = mbb * Vec3D(+1.0f,0,0);

			CGraphics* bg = &GetGraphics();
			bg->Begin(BGMODE_QUADS, m_Particles.size()*4);
			for (ParticleList::const_iterator it = m_Particles.begin(); it != m_Particles.end(); ++it)
			{
				Vec3D P,O;
				P=it->vPos;
				O=it->vOrigin;
				bg->Color(it->color);

				bg->TexCoord2fv(m_pEmitter->m_Tiles[it->nTile].tc[0]);
				bg->Vertex3fv(it->vPos + bv0 * it->fSize);

				bg->TexCoord2fv(m_pEmitter->m_Tiles[it->nTile].tc[1]);
				bg->Vertex3fv(it->vPos + bv1 * it->fSize);

				bg->TexCoord2fv(m_pEmitter->m_Tiles[it->nTile].tc[2]);
				bg->Vertex3fv(it->vOrigin + bv1 * it->fSize);

				bg->TexCoord2fv(m_pEmitter->m_Tiles[it->nTile].tc[3]);
				bg->Vertex3fv(it->vOrigin + bv0 * it->fSize);
			}
			bg->End();
		}
	}
}

void CParticleGroup::render(E_MATERIAL_RENDER_TYPE eRenderType)const
{
	if (passBegin(eRenderType))
	{
		draw();
	}
	passEnd();
}