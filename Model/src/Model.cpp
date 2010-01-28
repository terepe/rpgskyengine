#include "Model.h"
#include "FileSystem.h"
#include "RenderSystem.h"

#include "M2Model.h"
#include "BmdModel.h"

#include "CSVFile.h"
#include "IORead.h"

int globalTime = 0;

CModelMgr& GetModelMgr()
{
	static CModelMgr g_ModelMgr;
	return g_ModelMgr;
}
#include "Timer.h"

E_MODEL_RENDER_TYPE CMaterial::getModelRenderType()
{
	if (uDiffuse)
	{
		return MODEL_RENDER_MESH_GEOMETRY;
	}
	else if (uBump)
	{
		return MODEL_RENDER_MESH_BUMP;
	}
	else
	{
		return MODEL_RENDER_MESH_GLOW;
	}
	return MODEL_RENDER_MESH_GLOW;
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

CModel::CModel():
bLoaded(false)
{
	// Initiate our model variables.
	// 初始化我们的模型的变量

	for (int i=0; i<40; i++) 
		attLookup[i] = -1;
	for (int i=0; i<27; i++) 
		boneLookup[i] = -1;

	globalSequences = NULL;


	m_bHasAlphaTex = false;

	m_ModelType = MT_NORMAL;
	m_nOrder=0;
}

CModel::~CModel()
{
	S_DELS(globalSequences);
}

bool CModel::LoadFile(const std::string& strFilename)
{
	CLumpFile lumpFile;
	if (!lumpFile.LoadFile(strFilename))
	{
		return false;
	}
	// Mesh
	m_Mesh.load(lumpFile);

	// 读入模型包围体
	lumpFile.getVector("BoundV",	m_BoundMesh.pos);
	lumpFile.getVector("BoundI",	m_BoundMesh.indices);

	// Box
	lumpFile.GetVal("BoxMin",		m_bbox.vMin);
	lumpFile.GetVal("BoxMax",		m_bbox.vMax);
	if (m_bbox.vMin==m_bbox.vMax)
	{
		m_bbox = m_Mesh.m_bbox;
	}

	// 纹理通道
	CLumpNode* pTexChannelsNode = lumpFile.firstChild("Texs");
	if (pTexChannelsNode)
	{
		int subID = 0;
		CLumpNode* pChannelNode = pTexChannelsNode->firstChild("Channel");
		while (pChannelNode)
		{
			CLumpNode* pFilenamelNode = pChannelNode->firstChild("file");
			if (pFilenamelNode)
			{
				std::string strTexFileName;
				pFilenamelNode->GetString(strTexFileName);
				strTexFileName = GetParentPath(strFilename) + strTexFileName;
				int nTexID = GetRenderSystem().GetTextureMgr().RegisterTexture(strTexFileName);
				
				ModelRenderPass pass;
				pass.nSubID = subID++;
				pass.material.uDiffuse	=nTexID;
				pass.material.m_fOpacity	=1;
				pass.material.bAlphaTest	=true;
				pass.material.bBlend		=false;
				pass.material.vTexAnim.x	=0;
				pass.material.vTexAnim.y	=0;
				m_Passes.push_back(pass);
			}
			pChannelNode = pChannelNode->nextSibling("Channel");
		}
	}

	//lumpFile.SetVal("TexChannels", m_Lods[0].Geosets.size()*sizeof(ModelRenderPass), &m_Lods[0].Passes[0]);
	// 纹理通道
	//std::vector<TexGroup>		m_TexChannels;
	// 皮肤
	//std::vector<ModelSkin>		m_Skins;

	// ColorAnim
	int nColorAnimCount = 0;
	CLumpNode* pColorAnimsNode = lumpFile.GetInt("ColorAnim", nColorAnimCount);
	if (pColorAnimsNode)
	{
		m_ColorAnims.resize(nColorAnimCount);
		for (uint32 i = 0; i < m_ColorAnims.size(); ++i)
		{
			CLumpNode* pNode = pColorAnimsNode->firstChild(i);
			if (pNode)
			{
				m_ColorAnims[i].color.Load(*pNode, "color");
				m_ColorAnims[i].specular.Load(*pNode, "specular");
				m_ColorAnims[i].opacity.Load(*pNode, "opacity");
			}
		}
	}
	// AnimList
	lumpFile.getVector("AnimList", m_AnimList);
	if (m_AnimList.size()==0)
	{
		ModelAnimation anim;
		anim.animID = 0;
		anim.timeStart=0;
		anim.timeEnd=100000;
		anim.moveSpeed=1;
		anim.loopType = 0;
		anim.flags=0;
		anim.d1=0;
		anim.d2=0;
		anim.playSpeed=1;

		//anim.boxA, boxB;
		//float rad;

		//int16 s[2];
		ModelAnimation* buffer = new ModelAnimation[10];
		ModelAnimation* buffer1 = new ModelAnimation[10];
		m_AnimList.push_back(anim);
	}
	// Skeleton
	m_Skeleton.Load(lumpFile,"Skeleton");
	loadMaterial(ChangeExtension(strFilename,".mat.csv"),GetParentPath(strFilename));
	loadParticleEmitters(ChangeExtension(strFilename,".par.csv"),GetParentPath(strFilename));

	bLoaded=true;
	return true;
}

bool CModel::loadMaterial(const std::string& strFilename,const std::string& strPath)
{
	CCsvFile csv;
	CTextureMgr& TM = GetRenderSystem().GetTextureMgr();
	if (csv.Open(strFilename))
	{
		while (csv.SeekNextLine())
		{
			const size_t uSubID			= csv.GetInt("SubID");
			for (std::vector<ModelRenderPass>::iterator it=m_Passes.begin();it!=m_Passes.end();it++)
			{
				if (uSubID == it->nSubID)
				{
					it->material.uDiffuse	=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Diffuse")));
					it->material.uEmissive	=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Emissive")));
					it->material.uSpecular	=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Specular")));
					it->material.uBump		=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Bump")));
					it->material.uReflection=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Reflection")));
					it->material.uLightMap	=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("LightMap")));
					it->material.m_fOpacity	=csv.GetFloat("Opacity");
					it->material.bAlphaTest	=csv.GetBool("IsAlphaTest");
					it->material.bBlend		=csv.GetBool("IsBlend");
					it->material.vTexAnim.x	=csv.GetFloat("TexAnimX");
					it->material.vTexAnim.y	=csv.GetFloat("TexAnimY");
					continue;
				}
			}
			ModelRenderPass pass;
			pass.nSubID = uSubID;
			pass.material.uDiffuse	=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Diffuse")));
			pass.material.uEmissive	=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Emissive")));
			pass.material.uSpecular	=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Specular")));
			pass.material.uBump		=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Bump")));
			pass.material.uReflection=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Reflection")));
			pass.material.uLightMap	=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("LightMap")));
			pass.material.m_fOpacity	=csv.GetFloat("Opacity");
			pass.material.bAlphaTest	=csv.GetBool("IsAlphaTest");
			pass.material.bBlend		=csv.GetBool("IsBlend");
			pass.material.vTexAnim.x	=csv.GetFloat("TexAnimX");
			pass.material.vTexAnim.y	=csv.GetFloat("TexAnimY");
			m_Passes.push_back(pass);	
		}
		csv.Close();
	}
	return true;
}

#include<iostream>
#include<iomanip>
#include<fstream>
bool CModel::saveMaterial(const std::string& strFilename)
{
	setlocale(LC_ALL,"Chinese-simplified");
	std::ofstream ofs(strFilename.c_str());
	assert(ofs);
	ofs<<"SubID"<<",Diffuse"<<",Emissive"<<",Specular"<<",Bump"<<",Reflection"<<",LightMap"<<
		",Opacity"<<",IsAlphaTest"<<",IsBlend"<<",TexAnimX"<<",TexAnimY"<<std::endl;
	for (std::vector<ModelRenderPass>::iterator it=m_Passes.begin();it!=m_Passes.end();it++)
	{
		ofs<<(it->nSubID)<<","<<
			(it->material.strDiffuse.c_str())<<","<<(it->material.strEmissive.c_str())<<","<<(it->material.strSpecular.c_str())<<","<<
			(it->material.strBump.c_str())<<","<<(it->material.strReflection.c_str())<<","<<(it->material.strLightMap.c_str())<<","<<
			(it->material.m_fOpacity)<<","<<(it->material.bAlphaTest)<<","<<(it->material.bBlend)<<","<<
			(it->material.vTexAnim.x)<<","<<(it->material.vTexAnim.y)<<std::endl;
	}
	ofs.close();
	return true;
}

bool CModel::loadParticleEmitters(const std::string& strFilename,const std::string& strPath)
{
	CCsvFile csv;
	CTextureMgr& TM = GetRenderSystem().GetTextureMgr();
	if (csv.Open(strFilename))
	{
		while (csv.SeekNextLine())
		{
			CParticleEmitter	particleEmitter;
			particleEmitter.m_nBoneID = csv.GetInt("BoneID");

			particleEmitter.m_Speed.m_KeyTimes.push_back(0);
			particleEmitter.m_Speed.m_KeyData.push_back(csv.GetFloat("Speed"));

			particleEmitter.m_Variation.m_KeyTimes.push_back(0);
			particleEmitter.m_Variation.m_KeyData.push_back(csv.GetFloat("Variation"));

			particleEmitter.m_Spread.m_KeyTimes.push_back(0);
			particleEmitter.m_Spread.m_KeyData.push_back(csv.GetFloat("Spread"));

			particleEmitter.m_Lat.m_KeyTimes.push_back(0);
			particleEmitter.m_Lat.m_KeyData.push_back(csv.GetFloat("Lat"));

			particleEmitter.m_Gravity.m_KeyTimes.push_back(0);
			particleEmitter.m_Gravity.m_KeyData.push_back(csv.GetFloat("Gravity"));

			particleEmitter.m_Lifespan.m_KeyTimes.push_back(0);
			particleEmitter.m_Lifespan.m_KeyData.push_back(csv.GetFloat("Lifespan"));

			particleEmitter.m_Rate.m_KeyTimes.push_back(0);
			particleEmitter.m_Rate.m_KeyData.push_back(csv.GetFloat("Rate"));

			particleEmitter.m_Areal.m_KeyTimes.push_back(0);
			particleEmitter.m_Areal.m_KeyData.push_back(csv.GetFloat("Areal"));

			particleEmitter.m_Areaw.m_KeyTimes.push_back(0);
			particleEmitter.m_Areaw.m_KeyData.push_back(csv.GetFloat("Areaw"));

			particleEmitter.m_Deacceleration.m_KeyTimes.push_back(0);
			particleEmitter.m_Deacceleration.m_KeyData.push_back(csv.GetFloat("Deacceleration"));

			particleEmitter.m_Enabled.m_KeyTimes.push_back(csv.GetInt("TimeBegin"));
			particleEmitter.m_Enabled.m_KeyData.push_back(true);
			particleEmitter.m_Enabled.m_KeyTimes.push_back(csv.GetInt("TimeEnd"));
			particleEmitter.m_Enabled.m_KeyData.push_back(false);
	
			particleEmitter.m_Colors[0]=csv.GetHex("ColorBegin");
			particleEmitter.m_Colors[1]=csv.GetHex("ColorMiddle");
			particleEmitter.m_Colors[2]=csv.GetHex("ColorEnd");

			particleEmitter.m_Sizes[0]=csv.GetFloat("SizeBegin");
			particleEmitter.m_Sizes[1]=csv.GetFloat("SizeMiddle");
			particleEmitter.m_Sizes[2]=csv.GetFloat("SizeEnd");
	
			particleEmitter.m_fLifeMid=csv.GetFloat("LifeMiddle");
			particleEmitter.m_fSlowdown=csv.GetFloat("Slowdown");
			particleEmitter.m_fRotation=csv.GetFloat("Rotation");

			particleEmitter.m_vPos.x=csv.GetFloat("PosX");
			particleEmitter.m_vPos.y=csv.GetFloat("PosY");
			particleEmitter.m_vPos.z=csv.GetFloat("PosZ");

			particleEmitter.m_nBlend=csv.GetInt("Blend");
			particleEmitter.m_nOrder=csv.GetInt("Order");
			particleEmitter.type=csv.GetInt("Type");

			particleEmitter.m_nRows=csv.GetInt("Rows");
			particleEmitter.m_nCols=csv.GetInt("Cols");

			particleEmitter.m_bBillboard=csv.GetBool("IsBillboard");

			particleEmitter.uTexID=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Tex")));

			m_setParticleEmitter.push_back(particleEmitter);
		}
		csv.Close();
	}
	return true;
}

bool CModel::SaveFile(const std::string& strFilename)
{
	CLumpFile lumpFile;
	lumpFile.SetName("model");
	lumpFile.SetInt(1);
	CLumpNode* pskinNode = lumpFile.SetBool("skin", true);
	{
		//pNode->Set
	}

	saveMaterial(ChangeExtension(strFilename,".mat.csv"));
	// Mesh
	m_Mesh.save(lumpFile);

	// 写入包围体
	lumpFile.SetVector("BoundV", m_BoundMesh.pos);
	lumpFile.SetVector("BoundI", m_BoundMesh.indices);

	// Box
	lumpFile.SetVal("BoxMin", m_bbox.vMin);
	lumpFile.SetVal("BoxMax", m_bbox.vMax);

	//for (int i = 0; i < m_Lods.size(); i++)
	{
		//lumpFile.SetVal("TexChannels", m_Lods[0].Geosets.size()*sizeof(ModelRenderPass), &m_Lods[0].Passes[0]);

		//std::vector<TexGroup>		m_TexChannels;
		// 皮肤
		//std::vector<ModelSkin>		m_Skins;
	}


	// ColorAnim
	CLumpNode* pColorAnimsNode = lumpFile.SetInt("ColorAnim", m_ColorAnims.size());
	if (pColorAnimsNode)
	{
		for (uint32 i = 0; i < m_ColorAnims.size(); i++)
		{
			CLumpNode* pNode = pColorAnimsNode->AddNode(i);
			if (pNode)
			{
				m_ColorAnims[i].color.Save(*pNode, "color");
				m_ColorAnims[i].specular.Save(*pNode, "specular");
				m_ColorAnims[i].opacity.Save(*pNode, "opacity");
			}
		}
	}

	// AnimList
	lumpFile.SetVector("AnimList", m_AnimList);

	// Skeleton
	m_Skeleton.Save(lumpFile,"Skeleton");

	lumpFile.SaveFile(strFilename);
	return true;
}


void CModel::Init()
{
	m_Mesh.Init();
	if (m_Passes.size()==0)
	{
		if (m_Mesh.m_Lods.size()>0)
		{
			for (int i=0; i<m_Mesh.m_Lods[0].setSubset.size();++i)
			{
				ModelRenderPass pass;
				pass.nSubID = i;
				pass.material.uDiffuse		=1;
				pass.material.m_fOpacity	=1;
				pass.material.bAlphaTest	=true;
				pass.material.bBlend		=false;
				pass.material.vTexAnim.x	=0;
				pass.material.vTexAnim.y	=0;
				m_Passes.push_back(pass);
			}
		}
	}
	m_nOrder=0;
	for (std::vector<ModelRenderPass>::iterator it=m_Passes.begin();it!=m_Passes.end();it++)
	{
		if (it->material.m_fOpacity<1.0f)
		{
			m_nOrder--;
		}
		if (it->material.uDiffuse==0)
		{
			m_nOrder--;
			if (it->material.uEmissive!=0)
			{
				m_nOrder--;
			}
		}
		// alpha test
		if (it->material.bAlphaTest)
		{
			m_bHasAlphaTex = true;
		}
	}
}

int CModel::GetOrder()
{
	return m_nOrder;
}

bool CModel::isLoaded()
{
	return bLoaded;
}
//// Sets up the models attachments
//void CObject::setupAtt(int id)
//{
//	int l = attLookup[id];
//	if (l>-1)
//		atts[l].setup();
//}
//
//// Sets up the models attachments
//void CObject::setupAtt2(int id)
//{
//	int l = attLookup[id];
//	if (l>=0)
//		atts[l].setupParticle();
//}

void TexAnim::Calc(int nTime, Matrix& matrix)
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

void ModelAttachment::setup()
{
	//Matrix m = model->bones[bone].mat;
	//m.transpose();
	//glMultMatrixf(m);
	//glTranslatef(pos.x, pos.y, pos.z);
}

void ModelAttachment::setupParticle()
{
	//Matrix m = model->bones[bone].mat;
	//m.transpose();
	//glMultMatrixf(m);
	//glTranslatef(pos.x, pos.y, pos.z);
}

#include "IORead.h"

uint32 CModelMgr::RegisterModel(const std::string& strFilename)
{
	if (strFilename.length()==0)
	{
		return 0;
	}
	if(!IOReadBase::Exists(strFilename))// 检查文件是否存在
	{
		return 0;
	}
	if (find(strFilename))// 有一样的纹理在了 不用再创建了啦
	{
		return addRef(strFilename);
	}
	// 判断格式--根据文件后缀名
	std::string strExt = GetExtension(strFilename);
	CModel* pModel;
	if (strExt==".sm")
	{
		pModel = new CModel();
	}
	else if (strExt==".m2")
	{
		pModel = new CM2Model();
	}
	else if (strExt==".bmd")
	{
		pModel = new CBmdModel();
	}
	return add(strFilename, pModel);
}

CModel* CModelMgr::GetModel(uint32 uModelID)
{
	return getItem(uModelID);
}