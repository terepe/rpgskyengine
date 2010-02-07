#pragma once
#include "Material.h"
#include "RenderSystem.h"


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