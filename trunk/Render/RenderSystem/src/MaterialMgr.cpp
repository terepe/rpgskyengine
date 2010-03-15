#include "MaterialMgr.h"
#include "IORead.h"
#include "CSVFile.h"
#include "RenderSystem.h"
#include "FileSystem.h"

CMaterialMgr::CMaterialMgr()
{
}

CMaterialMgr::~CMaterialMgr()
{
}

CMaterial& CMaterialMgr::getItem(const std::string& strMaterialName)
{
	return m_Items[strMaterialName];
}

bool CMaterialMgr::createFromFile(const std::string& strFilename)
{
	CCsvFile csv;
	if (csv.Open(strFilename))
	{
		return false;
	}
	while (csv.SeekNextLine())
	{
		const std::string strMaterialName	= csv.GetStr("SubID");
		CMaterial& material = getItem(strMaterialName);
		std::string strPath = GetParentPath(strFilename);
		CTextureMgr& TM = GetRenderSystem().GetTextureMgr();
		material.uDiffuse	=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Diffuse")));
		material.uEmissive	=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Emissive")));
		material.uSpecular	=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Specular")));
		material.uBump		=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Bump")));
		material.uReflection=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Reflection")));
		material.uLightMap	=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("LightMap")));
		material.m_fOpacity	=csv.GetFloat("Opacity");
		material.bAlphaTest	=csv.GetBool("IsAlphaTest");
		material.bBlend		=csv.GetBool("IsBlend");
		material.vTexAnim.x	=csv.GetFloat("TexAnimX");
		material.vTexAnim.y	=csv.GetFloat("TexAnimY");
		material.uEffect	=GetRenderSystem().GetShaderMgr().registerItem(getRealFilename(strPath,csv.GetStr("effect")));
	}
	csv.Close();
	return true;
}