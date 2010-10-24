#pragma once
#include "Color.h"
#include "Vec2D.h"
#include "Vec4D.h"
#include <map>

enum E_MATERIAL_RENDER_TYPE
{
	MATERIAL_NONE				= 0,
	MATERIAL_GEOMETRY			= 1<<0,
	MATERIAL_ALPHA				= 1<<1,
	MATERIAL_GLOW				= 1<<2,
	MATERIAL_BUMP				= 1<<3,
	MATERIAL_NORMAL				= MATERIAL_GEOMETRY+MATERIAL_ALPHA+MATERIAL_GLOW,
	MATERIAL_ALL				= MATERIAL_NORMAL+MATERIAL_BUMP,
	MATERIAL_RENDER_ALPHA_TEST	= 1<<4,
	MATERIAL_RENDER_NO_MATERIAL	= 1<<5,
	MATERIAL_TARGET_MESH		= 1<<6,
	MATERIAL_TARGET_PARTICLE	= 1<<7,
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
		bLightingEnabled(false),
		vAmbient(0.6f,0.6f,0.6f,0.6f),
		vDiffuse(1.0f,1.0f,1.0f,1.0f),
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
			return MATERIAL_GEOMETRY;
		}
		else if (uDiffuse==0&&uNormal>0)
		{
			return MATERIAL_BUMP;
		}
		else if (bBlend)
		{
			return MATERIAL_GLOW;
		}
		return MATERIAL_GEOMETRY;
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
	unsigned long	uDiffuse;
	unsigned long	uEmissive;
	unsigned long	uSpecular;
	unsigned long	uNormal;
	unsigned long	uReflection;		// nTexEnvironment
	unsigned long	uLightMap;
	unsigned long	uShader;			// shader

	// color
	Vec4D	vAmbient;
	Vec4D	vDiffuse;

	// other
	bool	bLightingEnabled;
	unsigned char uCull;
	bool	bDepthWrite;
	bool	bBlend;
	bool	bAlphaTest;
	unsigned char uAlphaTestValue;
	Vec2D	vTexAnim;
	float	m_fOpacity;
	Color32 cEmissive;
	Vec2D	vUVScale; // for terrain's tile, temp
};

#include "InterfaceDataPlugsBase.h"
class CMaterialDataPlugBase:public CDataPlugBase
{
public:
	CMaterialDataPlugBase(){};
	virtual ~CMaterialDataPlugBase(){};

	virtual const char * GetTitle()		= 0;
	virtual const char * GetFormat()	= 0;
	virtual int Execute(std::map<std::string, CMaterial>& mapItems, bool bShowDlg, bool bSpecifyFileName) = 0;
	virtual bool importData(std::map<std::string, CMaterial>& mapItems, const char* szFilename, const char* szParentDir)=0;
	virtual bool exportData(std::map<std::string, CMaterial>& mapItems, const char* szFilename, const char* szParentDir)=0;

	virtual DWORD GetExportDataType() = 0;
	virtual DWORD GetImportDataType() = 0;

	virtual void Release() = 0;
};