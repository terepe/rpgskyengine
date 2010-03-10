#pragma once
#include "Material.h"
#include "RenderSystem.h"

#include "FileSystem.h"

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
bCull(false),
vTexAnim(0.0f,0.0f),
m_fOpacity(1.0f),
cEmissive(255,255,255,255),
vUVScale(0.0f,0.0f)
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

void CMaterial::create(const std::string& strDiffuse, const std::string& strEmissive,
					   const std::string& strSpecular, const std::string& strNormal,
					   const std::string& strEnvironment, const std::string& strShader,
					   int nChannel, bool bBlend, bool bCull, bool bAlphaTest, unsigned char uAlphaTestValue, float fTexScaleU, float fTexScaleV)
{
	//nChannel
	//bBlend
	bCull = bCull;
	bAlphaTest			= bAlphaTest;
	uAlphaTestValue	= uAlphaTestValue;
	vUVScale.x			= fTexScaleU;
	vUVScale.y			= fTexScaleV;

	CTextureMgr& TM = GetRenderSystem().GetTextureMgr();
	uDiffuse	= TM.RegisterTexture(strDiffuse);
	uEmissive	= TM.RegisterTexture(strEmissive);
	uSpecular	= TM.RegisterTexture(strSpecular);
	uBump		= TM.RegisterTexture(strNormal);
	uReflection= TM.RegisterTexture(strEnvironment);
	uEffect	= GetRenderSystem().GetShaderMgr().registerItem(strShader);
}

void CMaterial::createByScript(const std::string& strMaterialScript)
{
	std::vector<std::string> line;
	Tokenizer(strMaterialScript,line);
	if (line.size()<13)
	{
		return;
	}
	std::string strDiffuse		= line[0];
	std::string strEmissive		= line[1];
	std::string strSpecular		= line[2];
	std::string strNormal		= line[3];
	std::string strEnvironment	= line[4];
	std::string strShader		= line[5];
	int nChannel		= atoi(line[6].c_str());
	bool bBlend			= atoi(line[7].c_str())!=0;
	bool bCull			= atoi(line[8].c_str())!=0;
	bool bAlphaTest		= atoi(line[9].c_str())!=0;
	unsigned char uAlphaTestValue = atoi(line[10].c_str());
	float fTexScaleU = 1.0f/(float)atof(line[11].c_str());
	float fTexScaleV = 1.0f/(float)atof(line[12].c_str());

	create(strDiffuse, strEmissive, strSpecular, strNormal, strEnvironment, strShader,
		nChannel, bBlend, bCull, bAlphaTest, uAlphaTestValue, fTexScaleU, fTexScaleV);
}

void CMaterial::readFromCSV(CCsvFile& csv,const std::string& strPath)
{
	CTextureMgr& TM = GetRenderSystem().GetTextureMgr();
	uDiffuse	=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Diffuse")));
	uEmissive	=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Emissive")));
	uSpecular	=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Specular")));
	uBump		=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Bump")));
	uReflection=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Reflection")));
	uLightMap	=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("LightMap")));
	m_fOpacity	=csv.GetFloat("Opacity");
	bAlphaTest	=csv.GetBool("IsAlphaTest");
	bBlend		=csv.GetBool("IsBlend");
	vTexAnim.x	=csv.GetFloat("TexAnimX");
	vTexAnim.y	=csv.GetFloat("TexAnimY");
	uEffect	=GetRenderSystem().GetShaderMgr().registerItem(getRealFilename(strPath,csv.GetStr("effect")));
}

int	CMaterial::getOrder()
{
	int nOrder=0;
	if (m_fOpacity<1.0f)
	{
		nOrder--;
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