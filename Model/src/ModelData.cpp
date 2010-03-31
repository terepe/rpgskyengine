#include "ModelData.h"
#include "RenderSystem.h"
#include "ModelDataMgr.h"

int globalTime = 0;

CModelData::CModelData():
bLoaded(false)
{
	for (int i=0; i<40; i++) 
		attLookup[i] = -1;
	for (int i=0; i<27; i++) 
		boneLookup[i] = -1;
	globalSequences = NULL;
	m_nOrder=0;
}

CModelData::~CModelData()
{
	S_DELS(globalSequences);
}

size_t CModelData::getAnimationCount()
{
	return m_AnimList.size();
}

void CModelData::setAnimation(const std::string& strName, long timeStart, long timeEnd)
{
	ModelAnimation& animation=m_AnimList[strName];
	animation.timeStart = timeStart;
	animation.timeEnd = timeEnd;
}

bool CModelData::getAnimation(const std::string& strName, long& timeStart, long& timeEnd)const
{
	std::map<std::string, ModelAnimation>::const_iterator it = m_AnimList.find(strName);
	if (it==m_AnimList.end())
	{
		return false;
	}
	timeStart = it->second.timeStart;
	timeEnd = it->second.timeEnd;
	return true;
}

bool CModelData::getAnimation(size_t index, std::string& strName, long& timeStart, long& timeEnd)const
{
	if (m_AnimList.size()<=index)
	{
		return false;
	}
	std::map<std::string, ModelAnimation>::const_iterator it = m_AnimList.begin();
	advance(it,index);
	strName = it->first;
	timeStart = it->second.timeStart;
	timeEnd = it->second.timeEnd;
	return true;
}

bool CModelData::delAnimation(const std::string& strName)
{
	std::map<std::string, ModelAnimation>::iterator it=m_AnimList.find(strName);
	if(it!=m_AnimList.end())
	{
		m_AnimList.erase(it);
		return true;
	}
	return false;
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

CMaterial& CModelData::getMaterial(const std::string& strMaterialName)
{
	return GetRenderSystem().getMaterialMgr().getItem(strMaterialName);
}

bool CModelData::LoadFile(const std::string& strFilename)
{
	m_strModelFilename = strFilename;
	CModelDataMgr::getInstance().loadModel(*this,strFilename);

	bLoaded=true;
	return true;

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
// 	lumpFile.GetVal("BoxMin",		m_bbox.vMin);
// 	lumpFile.GetVal("BoxMax",		m_bbox.vMax);
// 	if (m_bbox.vMin==m_bbox.vMax)
// 	{
// 		m_bbox = m_Mesh.m_bbox;
// 	}

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
				std::string strMaterialName = Format("%s%d",ChangeExtension(getItemName(),".sub"),subID);
				{
					CMaterial& material = this->getMaterial(strMaterialName);
					material.strDiffuse=strTexFileName;
					material.bAlphaTest=true;
				}
				setRenderPass(subID, subID, strMaterialName );
				subID++;
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
	//lumpFile.getVector("AnimList", m_AnimList);
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
		m_AnimList["0"]=anim;
	}
	// Skeleton
	m_Skeleton.Load(lumpFile,"Skeleton");
	//loadMaterial(ChangeExtension(strFilename,".mat.csv"));
	loadParticleEmitters(ChangeExtension(strFilename,".par.csv"));

	bLoaded=true;
	return true;
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
		CMaterial& material = this->getMaterial(it->second.strMaterialName);
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
			(material.vTexAnim.y)<<std::endl;
	}
	ofs.close();
	return true;
}

bool CModelData::initParticleMaterial()
{
	//if (m_setParticleEmitter.size()==0)
	{
		for (size_t i=0;i<m_setParticleEmitter.size();++i)
		{
			std::string strMaterialName = Format("%s%d",ChangeExtension(getItemName(),".par"),i);
			m_setParticleEmitter[i].m_strMaterialName = strMaterialName;
		}
	}
	return true;
}

#include "CSVFile.h"
bool CModelData::loadParticleEmitters(const std::string& strFilename)
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

			//particleEmitter.m_nBlend=csv.GetInt("Blend");
			particleEmitter.m_nOrder=csv.GetInt("Order");
			particleEmitter.type=csv.GetInt("Type");

			particleEmitter.m_nRows=csv.GetInt("Rows");
			particleEmitter.m_nCols=csv.GetInt("Cols");

			particleEmitter.m_bBillboard=csv.GetBool("IsBillboard");

			//particleEmitter.uTexID=TM.RegisterTexture(getRealFilename(strPath,csv.GetStr("Tex")));

			m_setParticleEmitter.push_back(particleEmitter);
		}
		csv.Close();
	}
	initParticleMaterial();
	return true;
}

bool CModelData::SaveFile(const std::string& strFilename)
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
// 	lumpFile.SetVal("BoxMin", m_bbox.vMin);
// 	lumpFile.SetVal("BoxMax", m_bbox.vMax);

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
	//lumpFile.SetVector("AnimList", m_AnimList);

	// Skeleton
	m_Skeleton.Save(lumpFile,"Skeleton");

	lumpFile.SaveFile(strFilename);
	return true;
}


void CModelData::Init()
{
	m_Mesh.Init();
	if (m_mapPasses.size()==0)
	{
		if (m_Mesh.m_Lods.size()>0)
		{
			for (int i=0; i<m_Mesh.m_Lods[0].setSubset.size();++i)
			{
				ModelRenderPass& pass = m_mapPasses[i];
				pass.nSubID = i;
			}
		}
	}
	m_nOrder=0;
	for (std::map<int,ModelRenderPass>::iterator it=m_mapPasses.begin();it!=m_mapPasses.end();it++)
	{
		CMaterial& material = this->getMaterial(it->second.strMaterialName);
		m_nOrder+=material.getOrder();
	}
}

int CModelData::GetOrder()
{
	return m_nOrder;
}

bool CModelData::isLoaded()
{
	return bLoaded;
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
		GetRenderSystem().getMaterialMgr().getItem(pass.strMaterialName).SetEmissiveColor(ocol.getColor());

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
		//	pass.material.uLightMap = m_idLightMapTex;
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
	return GetRenderSystem().prepareMaterial(pass.strMaterialName,fOpacity);
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
		for (std::map<int,ModelRenderPass>::const_iterator it = m_mapPasses.begin(); it != m_mapPasses.end(); ++it)
		{
			if (GetRenderSystem().getMaterialMgr().getItem(it->second.strMaterialName).getRenderType()&eModelRenderType)
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

void CModelData::drawMesh(E_MATERIAL_RENDER_TYPE eModelRenderType, size_t uLodLevel, CHardwareVertexBuffer* pSkinVB)const
{
	if (m_Mesh.SetMeshSource(uLodLevel,pSkinVB))
	{
		for (std::map<int,ModelRenderPass>::const_iterator it = m_mapPasses.begin(); it != m_mapPasses.end(); ++it)
		{
			if (GetRenderSystem().getMaterialMgr().getItem(it->second.strMaterialName).getRenderType()&eModelRenderType)
			{
				m_Mesh.drawSub(it->second.nSubID,uLodLevel);
			}
		}
	}
}

void CModelData::drawMesh(CHardwareVertexBuffer* pSkinVB)const
{
	if (m_Mesh.SetMeshSource(0,pSkinVB))
	{
		m_Mesh.draw();
	}
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