#include "ModelData.h"
#include "RenderSystem.h"
#include "ModelDataMgr.h"
#include "FileSystem.h"

CModelData::CModelData()
:m_bLoaded(false)
,m_nOrder(0)
{
}

CModelData::~CModelData()
{
}

size_t CModelData::getRenderPassCount()
{
	return m_mapPasses.size();
}

void CModelData::setRenderPass(int nID, int nSubID, const std::string& strMaterialName)
{
	ModelRenderPass& pass = m_mapPasses[nID];
	pass.nSubID = nSubID;
	pass.strMaterialName = strMaterialName;
	//pass.p = modelLod.Geosets[passes[j].nGeosetID].v.z;
}

bool CModelData::getRenderPass(int nID, int& nSubID, std::string& strMaterialName)const
{
	std::map<int,ModelRenderPass>::const_iterator it = m_mapPasses.find(nID);
	if (it==m_mapPasses.end())
	{
		return false;
	}
	nSubID = it->second.nSubID;
	strMaterialName = it->second.strMaterialName;
	return true;
}

bool CModelData::delRenderPass(int nID)
{
	std::map<int,ModelRenderPass>::iterator it=m_mapPasses.find(nID);
	if(it!=m_mapPasses.end())
	{
		m_mapPasses.erase(it);
		return true;
	}
	return false;
}

void CModelData::loadMaterial(const char* szFilename, const char* szParentDir)
{
	GetRenderSystem().getMaterialMgr().load(szFilename,szParentDir);
}

CMaterial& CModelData::getMaterial(const char* szName)
{
	return GetRenderSystem().getMaterialMgr().getItem(szName);
}

#include<iostream>
#include<iomanip>
#include<fstream>
bool CModelData::saveMaterial(const std::string& strFilename)
{
	setlocale(LC_ALL,"Chinese-simplified");
	std::ofstream ofs(strFilename.c_str());
	assert(ofs);
	ofs<<"SubID"<<",Diffuse"<<",Emissive"<<",Specular"<<",Bump"<<",Reflection"<<",LightMap"<<
		",Opacity"<<",IsAlphaTest"<<",IsBlend"<<",TexAnimX"<<",TexAnimY"<<std::endl;
	for (std::map<int,ModelRenderPass>::iterator it=m_mapPasses.begin();it!=m_mapPasses.end();it++)
	{
	/*	CMaterial& material = GetRenderSystem().getMaterialMgr().getItem(it->second.strMaterialName.c_str());
		CTextureMgr& TM = GetRenderSystem().GetTextureMgr();
		ofs<<(it->second.nSubID)<<","<<
			(TM.getItemName(material.uDiffuse).c_str())<<","<<
			(TM.getItemName(material.uEmissive).c_str())<<","<<
			(TM.getItemName(material.uSpecular).c_str())<<","<<
			(TM.getItemName(material.uNormal).c_str())<<","<<
			(TM.getItemName(material.uReflection).c_str())<<","<<
			(TM.getItemName(material.uLightMap).c_str())<<","<<
			(material.m_fOpacity)<<","<<
			(material.bAlphaTest)<<","<<
			(material.bBlend)<<","<<
			(material.vTexAnim.x)<<","<<
			(material.vTexAnim.y)<<std::endl;*/
	}
	ofs.close();
	return true;
}

bool CModelData::initParticleMaterial()
{
	std::string strParticleMaterialName = GetFilename(ChangeExtension(getItemName(),".par"));
	for (size_t i=0;i<m_setParticleData.size();++i)
	{
		std::string strMaterialName = Format("%s%d",strParticleMaterialName.c_str(),i);
		m_setParticleData[i].m_strMaterialName = strMaterialName;
	}
	return true;
}

#include "CSVFile.h"
bool CModelData::loadParticleDatas(const char* szFilename)
{
	CCsvFile csv;
	CTextureMgr& TM = GetRenderSystem().GetTextureMgr();
	if (csv.open(szFilename))
	{
		while (csv.seekNextLine())
		{
			CParticleData	particleData;
			particleData.m_nBoneID = csv.getInt("BoneID",-1);

			particleData.m_vPos.x=csv.getFloat("PosX");
			particleData.m_vPos.y=csv.getFloat("PosY");
			particleData.m_vPos.z=csv.getFloat("PosZ");


			particleData.m_Speed.m_KeyTimes.push_back(0);
			particleData.m_Speed.m_KeyData.push_back(csv.getFloat("Speed"));

			particleData.m_Variation.m_KeyTimes.push_back(0);
			particleData.m_Variation.m_KeyData.push_back(csv.getFloat("Variation"));

			particleData.m_Spread.m_KeyTimes.push_back(0);
			particleData.m_Spread.m_KeyData.push_back(csv.getFloat("Spread"));

			particleData.m_Lat.m_KeyTimes.push_back(0);
			particleData.m_Lat.m_KeyData.push_back(csv.getFloat("Lat"));

			particleData.m_Gravity.m_KeyTimes.push_back(0);
			particleData.m_Gravity.m_KeyData.push_back(csv.getFloat("Gravity"));

			particleData.m_Lifespan.m_KeyTimes.push_back(0);
			particleData.m_Lifespan.m_KeyData.push_back(csv.getFloat("Lifespan"));

			particleData.m_Rate.m_KeyTimes.push_back(0);
			particleData.m_Rate.m_KeyData.push_back(csv.getFloat("Rate"));

			particleData.m_Areal.m_KeyTimes.push_back(0);
			particleData.m_Areal.m_KeyData.push_back(csv.getFloat("Areal"));

			particleData.m_Areaw.m_KeyTimes.push_back(0);
			particleData.m_Areaw.m_KeyData.push_back(csv.getFloat("Areaw"));

			particleData.m_Deacceleration.m_KeyTimes.push_back(0);
			particleData.m_Deacceleration.m_KeyData.push_back(csv.getFloat("Deacceleration"));

			particleData.m_Enabled.m_KeyTimes.push_back(csv.getInt("TimeBegin"));
			particleData.m_Enabled.m_KeyData.push_back(true);
			particleData.m_Enabled.m_KeyTimes.push_back(csv.getInt("TimeEnd"));
			particleData.m_Enabled.m_KeyData.push_back(false);
	
			particleData.m_Colors[0]=csv.getHex("ColorBegin");
			particleData.m_Colors[1]=csv.getHex("ColorMiddle");
			particleData.m_Colors[2]=csv.getHex("ColorEnd");

			particleData.m_Sizes[0]=csv.getFloat("SizeBegin");
			particleData.m_Sizes[1]=csv.getFloat("SizeMiddle");
			particleData.m_Sizes[2]=csv.getFloat("SizeEnd");
	
			particleData.m_fLifeMid=csv.getFloat("LifeMiddle");
			particleData.m_fSlowdown=csv.getFloat("Slowdown");
			particleData.m_fRotation=csv.getFloat("Rotation");

			particleData.m_nOrder=csv.getInt("Order");
			particleData.type=csv.getInt("Type");

			particleData.m_nRows=csv.getInt("Rows");
			particleData.m_nCols=csv.getInt("Cols");

			particleData.m_bBillboard=csv.getBool("IsBillboard");

			m_setParticleData.push_back(particleData);
		}
		csv.close();
	}
	initParticleMaterial();
	return true;
}

void CModelData::Init()
{
	m_Mesh.Init();
	if (m_mapPasses.empty())
	{
		if (m_Mesh.m_Lods.size()>0)
		{
			for (size_t i=0; i<m_Mesh.m_Lods[0].setSubset.size();++i)
			{
				ModelRenderPass& pass = m_mapPasses[i];
				pass.nSubID = i;
			}
		}
	}
	m_nOrder=0;
	for (std::map<int,ModelRenderPass>::iterator it=m_mapPasses.begin();it!=m_mapPasses.end();it++)
	{
		CMaterial& material = GetRenderSystem().getMaterialMgr().getItem(it->second.strMaterialName.c_str());
		m_nOrder+=material.getOrder();
	}
}

//////////////////////////////////////////////////////////////////////////
bool CModelData::passBegin(const ModelRenderPass& pass, float fOpacity, int nAnimTime)const
{
	Vec4D ocol = Vec4D(1.0f, 1.0f, 1.0f, fOpacity);
	Vec4D ecol = Vec4D(0.0f, 0.0f, 0.0f, 0.0f);

	//float fOpacity = m_fTrans;
	// emissive colors
	if (m_TransAnims.size() > 0)
	{
		// opacity
		if (pass.nTransID!=-1)
		{
			fOpacity *= m_TransAnims[pass.nTransID].trans.getValue(nAnimTime)/32767.0f;
		}
	}
	if (fOpacity<=0.0f)
	{
		return false;
	}
	if (-1 != pass.nColorID)
	{
		Vec4D ecol = m_ColorAnims[pass.nColorID].GetColor(nAnimTime);
		ecol.w = 1;
		GetRenderSystem().getMaterialMgr().getItem(pass.strMaterialName.c_str()).SetEmissiveColor(ocol.getColor());

		//glMaterialfv(GL_FRONT, GL_EMISSION, ecol);
		/*			D3DMATERIAL9 mtrl;
		mtrl.Ambient	= *(D3DXCOLOR*)&ecol;//D3DXCOLOR(0.2,0.2,0.2,0.2);
		mtrl.Diffuse	= *(D3DXCOLOR*)&ecol;//D3DXCOLOR(0.8,0.8,0.8,0.8);
		Vec4D Specular	= Vec4D(m_ColorAnims[pass.nColorID].color.getValue(nAnimTime), 1);
		mtrl.Specular	= *(D3DXCOLOR*)&Specular;
		mtrl.Emissive	= D3DXCOLOR(0,0,0,0);//*(D3DXCOLOR*)&ecol;
		mtrl.Power		= 71;
		R.SetMaterial(&mtrl);
		R.SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);*/
		//R.SetRenderState(D3DRS_SPECULARENABLE, true);
		//R.SetRenderState(D3DRS_LOCALVIEWER, true);
		//R.SetRenderState(D3DRS_NORMALIZENORMALS, true);
		//R.SetRenderState(D3DRS_LOCALVIEWER, false);
	}
	//if(m_bLightmap)
	{
		//	pass.material.uLightMap = m_uLightMapTex;
	}

	// TEXTURE

	// Texture wrapping around the geometry
	//if (swrap)
	//	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	//if (twrap)
	//	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

	// 纹理动画实现
	if (m_TexAnims.size() && pass.nTexanimID !=-1)
	{
		// 纹理动画
		Matrix matTex;
		m_TexAnims[pass.nTexanimID].Calc(nAnimTime, matTex);
		// 在里面设置纹理矩阵
		GetRenderSystem().setTextureMatrix(0, TTF_COUNT2, matTex);
	}
	// color
	//glColor4fv(ocol);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, ocol);

	//if (!pass.bUnlit&&0) 
	//{
	//	//R.SetLightingEnabled(false);
	//	R.SetShader(m_nCartoonShaderID);
	//	static int nCartoonTex = GetRenderSystem().GetTextureMgr().RegisterTexture("toonshade.tga");
	//	R.SetTexture(1 , nCartoonTex, 1);
	//	COLOROP = D3DTOP_SELECTARG1;

	//	R.SetTextureFactor(Color32(176,176,176,176));
	//	R.SetTextureColorOP(1,TBOP_MODULATE, TBS_CURRENT, TBS_TEXTURE);
	//}
	return GetRenderSystem().prepareMaterial(pass.strMaterialName.c_str(),fOpacity);
}

void CModelData::passEnd()const
{
	CRenderSystem& R = GetRenderSystem();
	R.finishMaterial();
	R.setTextureMatrix(0, TTF_DISABLE);
	R.SetTexCoordIndex(0,0);
	R.SetTexCoordIndex(1,0);
}

void CModelData::renderMesh(E_MATERIAL_RENDER_TYPE eModelRenderType, size_t uLodLevel, CHardwareVertexBuffer* pSkinVB, float fOpacity, int nAnimTime)const
{
	if (m_Mesh.SetMeshSource(uLodLevel,pSkinVB))
	{
		if (eModelRenderType==(MATERIAL_ALL|MATERIAL_RENDER_NO_MATERIAL))
		{
			m_Mesh.draw();
		}
		else for (std::map<int,ModelRenderPass>::const_iterator it = m_mapPasses.begin(); it != m_mapPasses.end(); ++it)
		{
			E_MATERIAL_RENDER_TYPE RenderType = GetRenderSystem().getMaterialMgr().getItem(it->second.strMaterialName.c_str()).getRenderType();
			if (RenderType&eModelRenderType)
			{
				if (eModelRenderType&MATERIAL_RENDER_ALPHA_TEST)
				{
					GetRenderSystem().SetTexture(0,GetRenderSystem().getMaterialMgr().getItem(it->second.strMaterialName.c_str()).uTexture[0]);
					m_Mesh.drawSub(it->second.nSubID,uLodLevel);
				}
				else if (eModelRenderType&MATERIAL_RENDER_NO_MATERIAL)
				{
					m_Mesh.drawSub(it->second.nSubID,uLodLevel);
				}
				else
				{
					if (passBegin(it->second,fOpacity,nAnimTime))
					{
						if (it->second.nSubID<0)
						{
							m_Mesh.draw(uLodLevel);
						}
						else
						{
							m_Mesh.drawSub(it->second.nSubID,uLodLevel);
						}
					}
					passEnd();
					//	GetRenderSystem().GetDevice()->SetStreamSourceFreq(0,1);
					//	GetRenderSystem().GetDevice()->SetStreamSourceFreq(1,1);
				}
			}
		}
	}
}

void TexAnim::Calc(int nTime, Matrix& matrix)const
{
	Vec3D tval, rval, sval;
	if (trans.isUsed())
	{
		tval = trans.getValue(nTime);
	}
	if (rot.isUsed())
	{
        rval = rot.getValue(nTime);
	}
	if (scale.isUsed())
	{
        sval = scale.getValue(nTime);
	}
	matrix=Matrix::UNIT;
	// 平移
	if (trans.isUsed())
	{
		matrix.translation(tval);
	}
	// 旋转
	if (rot.isUsed())
	{//glRotatef(rval.x, 0, 0, 1); // this is wrong, I have no idea what I'm doing here ;)  (fuck)
		matrix *= Matrix::newQuatRotate(Quaternion(rval.x, 0, 0, 1));//newQuatRotate(Quaternion(0,1,0,rval.x));
	}
	// 缩放s
	if (scale.isUsed())
	{
		matrix *= Matrix::newScale(sval);
	}
}

void ModelCamera::setup(int time)
{
	//if (!ok) return;

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluPerspective(fov * 34.5f, (GLfloat)gXres/(GLfloat)gYres, nearclip, farclip);

	//Vec3D p = pos + tPos.getValue(0, time);
	//Vec3D t = target + tTarget.getValue(0, time);

	//Vec3D u(0,1,0);

	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//gluLookAt(p.x, p.y, p.z, t.x, t.y, t.z, u.x, u.y, u.z);
	////float roll = rot.getValue(0, time) / PI * 180.0f;
	////glRotatef(roll, 0, 0, 1);
}

void LightAnim::setup(int time, int l)
{
	//Vec4D ambcol(ambColor.getValue(0, time) * ambIntensity.getValue(0, time), 1.0f);
	//Vec4D diffcol(diffColor.getValue(0, time) * diffIntensity.getValue(0, time), 1.0f);
	//Vec4D p;
	//if (type==0) {
	//	// directional
	//	p = Vec4D(tdir, 0.0f);
	//} else {
	//	// point
	//	p = Vec4D(tpos, 1.0f);
	//}
	////gLog("Light %d (%f,%f,%f) (%f,%f,%f) [%f,%f,%f]\n", l-GL_LIGHT4, ambcol.x, ambcol.y, ambcol.z, diffcol.x, diffcol.y, diffcol.z, p.x, p.y, p.z);
	//glLightfv(l, GL_POSITION, p);
	//glLightfv(l, GL_DIFFUSE, diffcol);
	//glLightfv(l, GL_AMBIENT, ambcol);
	//glEnable(l);
}