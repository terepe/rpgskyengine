#pragma once
#include "Common.h"
#include "Color.h"
#include "Vec2D.h"

enum E_MATERIAL_RENDER_TYPE
{
	MATERIAL_RENDER_NOTHING		= 0,
	MATERIAL_RENDER_GEOMETRY	= 1<<0,
	MATERIAL_RENDER_ALPHA		= 1<<1,
	MATERIAL_RENDER_GLOW		= 1<<2,
	MATERIAL_RENDER_BUMP		= 1<<3,
	MATERIAL_RENDER_NORMAL		= MATERIAL_RENDER_GEOMETRY+MATERIAL_RENDER_ALPHA+MATERIAL_RENDER_GLOW,
	MATERIAL_RENDER_ALL			= MATERIAL_RENDER_NORMAL+MATERIAL_RENDER_BUMP,
};

struct CMaterial
{
	std::string strDiffuse;
	std::string strEmissive;
	std::string strSpecular;
	std::string strBump;
	std::string strReflection;
	std::string strEffect;
	std::string strLightMap;

	uint32	uDiffuse;
	uint32	uEmissive;
	uint32	uSpecular;
	uint32	uBump;
	uint32	uReflection;//nTexEnvironment
	uint32	uLightMap;
	uint32	uEffect;//uShaderID
	bool	bAlphaTest;
	uint8	uAlphaTestValue;
	bool	bBlend;
	bool	bCull;
	Vec2D	vTexAnim;
	float	m_fOpacity;
	Color32 cEmissive;
	Vec2D	vUVScale; // for terrain's tile, temp
	CMaterial();
	void SetEmissiveColor(const Color32& color);
	E_MATERIAL_RENDER_TYPE getRenderType();
};
