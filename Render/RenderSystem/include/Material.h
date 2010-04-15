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

class CMaterial
{
public:
	CMaterial():
	uDiffuse(-1),
		uEmissive(-1),
		uSpecular(-1),
		uNormal(-1),
		uReflection(-1),
		uLightMap(-1),
		uShader(-1),
		bDepthWrite(true),
		bAlphaTest(false),
		uAlphaTestValue(0x80),
		bBlend(false),
		uCull(0),
		vTexAnim(0.0f,0.0f),
		m_fOpacity(1.0f),
		cEmissive(255,255,255,255),
		vUVScale(0.0f,0.0f)
	{
	}

	E_MATERIAL_RENDER_TYPE getRenderType()
	{
		if (bAlphaTest)
		{
			return MATERIAL_RENDER_GEOMETRY;
		}
		else if (uDiffuse==0&&uNormal>0)
		{
			return MATERIAL_RENDER_BUMP;
		}
		else if (bBlend)
		{
			return MATERIAL_RENDER_GLOW;
		}
		return MATERIAL_RENDER_GEOMETRY;
	}

	int	getOrder()
	{
		int nOrder=0;
		if (m_fOpacity<1.0f)
		{
			nOrder--;
		}
		if (bAlphaTest&&bBlend)
		{
			nOrder--;
		}
		else if (bBlend)
		{
			nOrder-=2;
		}
		if (uDiffuse==0)
		{
			nOrder--;
			if (uEmissive!=0)
			{
				nOrder--;
			}
		}
		return nOrder;
	}

	void SetEmissiveColor(const Color32& color)
	{
		cEmissive = color;
	}
	// Texture
	void setDiffuse(const std::string& strFilename)
	{
		strDiffuse=strFilename;
		uDiffuse=-1;
	}
	void setEmissive(const std::string& strFilename)
	{
		strEmissive=strFilename;
		uEmissive=-1;
	}
	void setSpecular(const std::string& strFilename)
	{
		strSpecular=strFilename;
		uSpecular=-1;
	}
	void setNormal(const std::string& strFilename)
	{
		strNormal=strFilename;
		uNormal=-1;
	}
	void setReflection(const std::string& strFilename)
	{
		strReflection=strFilename;
		uReflection=-1;
	}
	void setLightMap(const std::string& strFilename)
	{
		strLightMap=strFilename;
		strLightMap=-1;
	}
	const std::string& getDiffuse()
	{
		return strDiffuse;
	}
	const std::string& getEmissive()
	{
		return strEmissive;
	}
	const std::string& getSpecular()
	{
		return strSpecular;
	}
	const std::string& getNormal()
	{
		return strNormal;
	}
	const std::string&  getReflection()
	{
		return strReflection;
	}
	const std::string&  getLightMap()
	{
		return strLightMap;
	}
	// Shader
	void setShader(const std::string& strFilename)
	{
		strShader=strFilename;
		uShader=-1;
	}
	const std::string& getShader()
	{
		return strShader;
	}
private:
	std::string	strDiffuse;
	std::string	strEmissive;
	std::string	strSpecular;
	std::string	strNormal;
	std::string	strReflection;	// nTexEnvironment
	std::string	strLightMap;
	std::string	strShader;		// shader
public:
	// texture
	uint32	uDiffuse;
	uint32	uEmissive;
	uint32	uSpecular;
	uint32	uNormal;
	uint32	uReflection;		// nTexEnvironment
	uint32	uLightMap;
	uint32	uShader;			// shader

	// other
	unsigned char uCull;
	bool	bDepthWrite;
	bool	bBlend;
	bool	bAlphaTest;
	uint8	uAlphaTestValue;
	Vec2D	vTexAnim;
	float	m_fOpacity;
	Color32 cEmissive;
	Vec2D	vUVScale; // for terrain's tile, temp
};
