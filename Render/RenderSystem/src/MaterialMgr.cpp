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

bool CMaterialMgr::createFromFile(const std::string& strFilename, const std::string& strPath)
{
	CCsvFile csv;
	if (csv.Open(strFilename))
	{
		return false;
	}
	std::string strMyPath = strPath;
	if (strMyPath.size()==0)
	{
		strMyPath = GetParentPath(strFilename);
	}
	while (csv.SeekNextLine())
	{
		const std::string strMaterialName	= csv.GetStr("Name");
		CMaterial& material = getItem(strMaterialName);
		CTextureMgr& TM = GetRenderSystem().GetTextureMgr();
		material.uDiffuse	=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Diffuse")));
		material.uEmissive	=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Emissive")));
		material.uSpecular	=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Specular")));
		material.uBump		=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Normal")));
		material.uReflection=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Reflection")));
		material.uLightMap	=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("LightMap")));
		material.m_fOpacity	=csv.GetFloat("Opacity");
		material.bAlphaTest	=csv.GetBool("IsAlphaTest");
		material.bBlend		=csv.GetBool("IsBlend");
		material.vTexAnim.x	=csv.GetFloat("TexAnimX");
		material.vTexAnim.y	=csv.GetFloat("TexAnimY");
		material.vUVScale.x	=csv.GetFloat("USize");
		material.vUVScale.y	=csv.GetFloat("VSize");

		material.uEffect	=GetRenderSystem().GetShaderMgr().registerItem(getRealFilename(strPath,csv.GetStr("Shader")));
	}
	csv.Close();
	return true;
}