#pragma once
#include "Common.h"
#include "Color.h"
#include "Vec2D.h"
#include "CSVFile.h"

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
	CMaterial();
	void createByScript(const std::string& strMaterialScript);
	void readFromCSV(CCsvFile& csv,const std::string& strPath);

	int	getOrder();
/*
	void setDiffuse(const std::string& strFilename);
	void setEmissive(const std::string& strFilename);
	void setSpecular(const std::string& strFilename);
	void setBump(const std::string& strFilename);
	void setReflection(const std::string& strFilename);
	void setLightMap(const std::string& strFilename);
	void setShader(const std::string& strFilename);*/
protected:
	void create(const std::string& strDiffuse, const std::string& strEmissive,
		const std::string& strSpecular, const std::string& strNormal,
		const std::string& strEnvironment, const std::string& strShader,
		int nChannel, bool bBlend, bool bCull, bool bAlphaTest, unsigned char uAlphaTestValue, float fTexScaleU, float fTexScaleV);
private:
public:
	// texture
	uint32	uDiffuse;
	uint32	uEmissive;
	uint32	uSpecular;
	uint32	uBump;
	uint32	uReflection;//nTexEnvironment
	uint32	uLightMap;
	// shader
	uint32	uEffect;

	// other
	bool	bAlphaTest;
	uint8	uAlphaTestValue;
	bool	bBlend;
	bool	bCull;
	Vec2D	vTexAnim;
	float	m_fOpacity;
	Color32 cEmissive;
	Vec2D	vUVScale; // for terrain's tile, temp
	void SetEmissiveColor(const Color32& color);
	E_MATERIAL_RENDER_TYPE getRenderType();
};
