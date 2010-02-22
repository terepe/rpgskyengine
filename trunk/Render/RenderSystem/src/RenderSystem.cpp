#include "RenderSystem.h"

CRenderSystem* g_pRenderSystem = NULL;

void SetRenderSystem(CRenderSystem* pRenderSystem)
{
	g_pRenderSystem = pRenderSystem;
}

CRenderSystem& GetRenderSystem()
{
	//assert(g_pRenderSystem);
	return *g_pRenderSystem;
}

CRenderSystem::CRenderSystem()
{
}
CRenderSystem::~CRenderSystem()
{
}

void CRenderSystem::world2Screen(const Vec3D& vWorldPos, Pos2D& posScreen)
{
	Matrix mProj;
	Matrix View;
	getProjectionMatrix(mProj);
	getViewMatrix(View);
	Vec4D vOut = mProj*View*Vec4D(vWorldPos,1);
	float fW = vOut.w;
	RECT rc;
	getViewport(rc);
	posScreen.x = int(rc.left+(rc.right-rc.left)*(0.5f+vOut.x*0.5f/fW));
	posScreen.y = int(rc.top+(rc.bottom-rc.top)*(0.5f-vOut.y*0.5f/fW));
}

void CRenderSystem::GetPickRay(Vec3D& vRayPos, Vec3D& vRayDir,int x, int y)
{
	Matrix mProj;
	getProjectionMatrix(mProj);
	RECT rc;
	getViewport(rc);
	Vec3D v;
	v.x =  (((2.0f * (x-rc.left)) / (rc.right-rc.left)) - 1) / mProj._11;
	v.y = -(((2.0f * (y-rc.top)) / (rc.bottom-rc.top)) - 1) / mProj._22;
	v.z =  1.0f;

	Matrix mView;
	getViewMatrix(mView);
	Matrix mViewInv = mView;
	mViewInv.Invert();
	//vRayDir = mViewInv*v-Vec3D(mView._14,mView._24,mView._34);
	vRayDir.normalize();
	//vRayPos = m_vEye;
}
#include "Timer.h"
bool CRenderSystem::prepareMaterial(const CMaterial& material, float fOpacity)
{
	if (0==material.uEffect)
	{
		SetSamplerAddressUV(0,ADDRESS_WRAP,ADDRESS_WRAP);
		SetCullingMode(material.bCull?CULL_ANTI_CLOCK_WISE:CULL_NONE);
		SetAlphaTestFunc(material.bAlphaTest,CMPF_GREATER_EQUAL,material.uAlphaTestValue);
		Color32 cFactor = material.cEmissive;
		if (material.m_fOpacity<0.0f)
		{
			fOpacity = (float)(rand()%255)/255.0f;
		}
		else
		{
			fOpacity *= material.m_fOpacity;
		}

		if (material.uDiffuse)
		{
			cFactor.a*=(unsigned char)(cFactor.a*fOpacity);
			SetTextureFactor(cFactor);
			SetTextureColorOP(0, TBOP_MODULATE, TBS_TEXTURE, TBS_TFACTOR);
			if(material.bBlend||material.m_fOpacity<1.0f)
			{
				SetBlendFunc(true, BLENDOP_ADD, SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA);
				SetTextureAlphaOP(0, TBOP_MODULATE, TBS_TEXTURE, TBS_TFACTOR);
				SetDepthBufferFunc(true, false);
			}
			else
			{
				SetBlendFunc(false);
				if (material.bAlphaTest)
				{
					SetTextureAlphaOP(0, TBOP_SOURCE1, TBS_TEXTURE);
				}
				else
				{
					SetTextureAlphaOP(0, TBOP_DISABLE);
				}

				SetDepthBufferFunc(true, true);
			}
			SetTexture(0, material.uDiffuse);
			//////////////////////////////////////////////////////////////////////////
			if (material.uReflection)
			{
				SetTextureColorOP(1, TBOP_MODULATE_X2, TBS_CURRENT, TBS_TEXTURE);
				SetTextureAlphaOP(1, TBOP_SOURCE1, TBS_CURRENT);
				SetTexCoordIndex(1,TCI_CAMERASPACE_NORMAL|TCI_CAMERASPACE_POSITION);
				SetTexture(1, material.uReflection);
			}
			else if (material.uLightMap)
			{
				SetTextureColorOP(1, TBOP_MODULATE, TBS_CURRENT, TBS_TEXTURE);
				SetTextureAlphaOP(1, TBOP_SOURCE1, TBS_CURRENT);
				SetTexCoordIndex(1,1);
				SetTexture(1, material.uLightMap);
			}
			else if (material.uEmissive)
			{
				SetTextureColorOP(1, TBOP_ADD, TBS_CURRENT, TBS_TEXTURE);
				SetTextureAlphaOP(1, TBOP_SOURCE1, TBS_CURRENT);
				SetTexture(1, material.uEmissive);
			}
			else if(!material.bBlend&&material.m_fOpacity>=1.0f)
			{
				SetLightingEnabled(true);
				SetTextureColorOP(0, TBOP_MODULATE, TBS_TEXTURE, TBS_DIFFUSE);
			}
		}
		else
		{
			SetLightingEnabled(false);
			SetTextureFactor(cFactor);
			SetDepthBufferFunc(true, false);
			if(material.uReflection)
			{
				cFactor.r*=(unsigned char)(cFactor.r*fOpacity);
				cFactor.g*=(unsigned char)(cFactor.g*fOpacity);
				cFactor.b*=(unsigned char)(cFactor.b*fOpacity);
				SetTextureFactor(cFactor);
				SetBlendFunc(true, BLENDOP_ADD, SBF_DEST_COLOUR, SBF_ONE);
				SetTextureColorOP(0, TBOP_MODULATE, TBS_TEXTURE, TBS_TFACTOR);
				SetTextureAlphaOP(0, TBOP_DISABLE);
				SetTexCoordIndex(0,TCI_CAMERASPACE_NORMAL|TCI_CAMERASPACE_POSITION);
				SetTexture(0, material.uReflection);
				if (material.vTexAnim.lengthSquared()>0.0f)
				{
					Matrix matTex=Matrix::UNIT;
					float fTime = (float)GetGlobalTimer().GetTime();
					matTex._14=fTime*material.vTexAnim.x;
					matTex._24=fTime*material.vTexAnim.y;
					GetRenderSystem().setTextureMatrix(0, TTF_COUNT2, matTex);
				}
			}
			else if (material.uLightMap)
			{
				SetBlendFunc(true, BLENDOP_ADD, SBF_DEST_COLOUR, SBF_ZERO);
				SetTextureColorOP(0, TBOP_MODULATE, TBS_TEXTURE, TBS_TFACTOR);
				SetTextureAlphaOP(0, TBOP_DISABLE);
				SetTexture(0, material.uLightMap);
			}
			else if (material.uEmissive)
			{
				cFactor.r*=(unsigned char)(cFactor.r*fOpacity);
				cFactor.g*=(unsigned char)(cFactor.g*fOpacity);
				cFactor.b*=(unsigned char)(cFactor.b*fOpacity);
				SetTextureFactor(cFactor);
				SetBlendFunc(true, BLENDOP_ADD, SBF_ONE, SBF_ONE);
				SetTextureColorOP(0, TBOP_MODULATE, TBS_TEXTURE, TBS_TFACTOR);
				SetTextureAlphaOP(0, TBOP_DISABLE);
				SetTexture(0, material.uEmissive);
				if (material.vTexAnim.lengthSquared()>0.0f)
				{
					Matrix matTex=Matrix::UNIT;
					float fTime = (float)GetGlobalTimer().GetTime();
					matTex._13=fTime*material.vTexAnim.x;
					matTex._23=fTime*material.vTexAnim.y;
					GetRenderSystem().setTextureMatrix(0, TTF_COUNT2, matTex);
				}
			}
			else if (material.uBump)
			{
				CShader* pShader = GetShaderMgr().getSharedShader();
				if (pShader)
				{
					static size_t s_uShaderID = GetRenderSystem().GetShaderMgr().registerItem("Data\\fx\\SpaceBump.fx");
					pShader->setTexture("g_texNormal",material.uBump);
					SetShader(s_uShaderID);
				}
				//SetBlendFunc(true, BLENDOP_ADD, SBF_DEST_COLOUR, SBF_ZERO);
				//SetTextureColorOP(0, TBOP_MODULATE, TBS_TEXTURE, TBS_TFACTOR);
				//SetTextureAlphaOP(0, TBOP_DISABLE);
				//SetTexture(0, uLightMap);
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		CShader* pShader = GetShaderMgr().getSharedShader();
		if (pShader)
		{
			pShader->setTexture("g_texDiffuse",material.uDiffuse);
			pShader->setTexture("g_texLight",material.uLightMap);
			pShader->setTexture("g_texNormal",material.uBump);
			//pShader->setTexture("g_texEnvironment",uEmissive);
			//pShader->setTexture("g_texEmissive",uEmissive);
			pShader->setTexture("g_texSpecular",material.uSpecular);
			// for Terrain
			pShader->setVec2D("g_fScaleUV",material.vUVScale);
		}
		SetShader(material.uEffect);
	}
	return true;
}
void CRenderSystem::finishMaterial()
{
	SetShader((CShader*)NULL);
}