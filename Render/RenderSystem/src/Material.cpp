#pragma once
#include "Material.h"
#include "RenderSystem.h"
#include "Timer.h"

CMaterial::CMaterial():
uDiffuse(0),
uEmissive(0),
uSpecular(0),
uBump(0),
uReflection(0),
uLightMap(0),
uEffect(0),
bAlphaTest(false),
uAlphaTestValue(0x80),
bBlend(false),
bCull(true),
m_fOpacity(1.0f),
cEmissive(255,255,255,255)
{
}

void CMaterial::SetEmissiveColor(const Color32& color)
{
	cEmissive = color;
}
 
E_MATERIAL_RENDER_TYPE CMaterial::getRenderType()
{
	if (uDiffuse)
	{
		return MATERIAL_RENDER_GEOMETRY;
	}
	else if (uBump)
	{
		return MATERIAL_RENDER_BUMP;
	}
	else
	{
		return MATERIAL_RENDER_GLOW;
	}
	return MATERIAL_RENDER_GLOW;
}

bool CMaterial::Begin(float fOpacity)
{
	CRenderSystem& R = GetRenderSystem();
	if (0==uEffect)
	{
		R.SetSamplerAddressUV(0,ADDRESS_WRAP,ADDRESS_WRAP);
		R.SetCullingMode(bCull?CULL_ANTI_CLOCK_WISE:CULL_NONE);
		R.SetAlphaTestFunc(bAlphaTest,CMPF_GREATER_EQUAL,uAlphaTestValue);
		Color32 cFactor = cEmissive;
		if (m_fOpacity<0.0f)
		{
			fOpacity = rand()%255;
		}
		else
		{
			fOpacity *= m_fOpacity;
		}

		if (uDiffuse)
		{
			cFactor.a *= fOpacity;
			R.SetTextureFactor(cFactor);
			R.SetTextureColorOP(0, TBOP_MODULATE, TBS_TEXTURE, TBS_TFACTOR);
			if(bBlend||m_fOpacity<1.0f)
			{
				R.SetBlendFunc(true, BLENDOP_ADD, SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA);
				R.SetTextureAlphaOP(0, TBOP_MODULATE, TBS_TEXTURE, TBS_TFACTOR);
				R.SetDepthBufferFunc(true, false);
			}
			else
			{
				R.SetBlendFunc(false);
				if (bAlphaTest)
				{
					R.SetTextureAlphaOP(0, TBOP_SOURCE1, TBS_TEXTURE);
				}
				else
				{
					R.SetTextureAlphaOP(0, TBOP_DISABLE);
				}

				R.SetDepthBufferFunc(true, true);
			}
			R.SetTexture(0, uDiffuse);
			//////////////////////////////////////////////////////////////////////////
			if (uReflection)
			{
				R.SetTextureColorOP(1, TBOP_MODULATE_X2, TBS_CURRENT, TBS_TEXTURE);
				R.SetTextureAlphaOP(1, TBOP_SOURCE1, TBS_CURRENT);
				R.SetTexCoordIndex(1,TCI_CAMERASPACE_NORMAL|TCI_CAMERASPACE_POSITION);
				R.SetTexture(1, uReflection);
			}
			else if (uLightMap)
			{
				R.SetTextureColorOP(1, TBOP_MODULATE, TBS_CURRENT, TBS_TEXTURE);
				R.SetTextureAlphaOP(1, TBOP_SOURCE1, TBS_CURRENT);
				R.SetTexCoordIndex(1,1);
				R.SetTexture(1, uLightMap);
			}
			else if (uEmissive)
			{
				R.SetTextureColorOP(1, TBOP_ADD, TBS_CURRENT, TBS_TEXTURE);
				R.SetTextureAlphaOP(1, TBOP_SOURCE1, TBS_CURRENT);
				R.SetTexture(1, uEmissive);
			}
			else if(!bBlend&&m_fOpacity>=1.0f)
			{
				R.SetLightingEnabled(true);
				R.SetTextureColorOP(0, TBOP_MODULATE, TBS_TEXTURE, TBS_DIFFUSE);
			}
		}
		else
		{
			R.SetTextureFactor(cFactor);
			R.SetDepthBufferFunc(true, false);
			if(uReflection)
			{
				cFactor.r*=fOpacity;
				cFactor.g*=fOpacity;
				cFactor.b*=fOpacity;
				R.SetTextureFactor(cFactor);
				R.SetBlendFunc(true, BLENDOP_ADD, SBF_DEST_COLOUR, SBF_ONE);
				R.SetTextureColorOP(0, TBOP_MODULATE, TBS_TEXTURE, TBS_TFACTOR);
				R.SetTextureAlphaOP(0, TBOP_DISABLE);
				R.SetTexCoordIndex(0,TCI_CAMERASPACE_NORMAL|TCI_CAMERASPACE_POSITION);
				R.SetTexture(0, uReflection);
				if (vTexAnim.lengthSquared()>0.0f)
				{
					Matrix matTex=Matrix::UNIT;
					matTex._14=GetGlobalTimer().GetTime()*vTexAnim.x;
					matTex._24=GetGlobalTimer().GetTime()*vTexAnim.y;
					GetRenderSystem().setTextureMatrix(0, TTF_COUNT2, matTex);
				}
			}
			else if (uLightMap)
			{
				R.SetBlendFunc(true, BLENDOP_ADD, SBF_DEST_COLOUR, SBF_ZERO);
				R.SetTextureColorOP(0, TBOP_MODULATE, TBS_TEXTURE, TBS_TFACTOR);
				R.SetTextureAlphaOP(0, TBOP_DISABLE);
				R.SetTexture(0, uLightMap);
			}
			else if (uEmissive)
			{
				cFactor.r*=fOpacity;
				cFactor.g*=fOpacity;
				cFactor.b*=fOpacity;
				R.SetTextureFactor(cFactor);
				R.SetBlendFunc(true, BLENDOP_ADD, SBF_ONE, SBF_ONE);
				R.SetTextureColorOP(0, TBOP_MODULATE, TBS_TEXTURE, TBS_TFACTOR);
				R.SetTextureAlphaOP(0, TBOP_DISABLE);
				R.SetTexture(0, uEmissive);
				if (vTexAnim.lengthSquared()>0.0f)
				{
					Matrix matTex=Matrix::UNIT;
					matTex._13=GetGlobalTimer().GetTime()*vTexAnim.x;
					matTex._23=GetGlobalTimer().GetTime()*vTexAnim.y;
					GetRenderSystem().setTextureMatrix(0, TTF_COUNT2, matTex);
				}
			}
			else if (uBump)
			{
				CShader* pShader = R.GetShaderMgr().getSharedShader();
				if (pShader)
				{
					static size_t s_uShaderID = GetRenderSystem().GetShaderMgr().registerItem("Data\\fx\\SpaceBump.fx");
					pShader->setTexture("g_texNormal",uBump);
					R.SetShader(s_uShaderID);
				}
				//R.SetBlendFunc(true, BLENDOP_ADD, SBF_DEST_COLOUR, SBF_ZERO);
				//R.SetTextureColorOP(0, TBOP_MODULATE, TBS_TEXTURE, TBS_TFACTOR);
				//R.SetTextureAlphaOP(0, TBOP_DISABLE);
				//R.SetTexture(0, uLightMap);
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		CShader* pShader = R.GetShaderMgr().getSharedShader();
		if (pShader)
		{
			pShader->setTexture("g_texDiffuse",uDiffuse);
			pShader->setTexture("g_texLight",uLightMap);
			pShader->setTexture("g_texNormal",uBump);
			//pShader->setTexture("g_texEnvironment",uEmissive);
			//pShader->setTexture("g_texEmissive",uEmissive);
			pShader->setTexture("g_texSpecular",uSpecular);
		}
		R.SetShader(uEffect);
	}
	return true;
}
