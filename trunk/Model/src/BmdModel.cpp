#include "BmdModel.h"
#include "RenderSystem.h"
#include "MUBmd.h"
#include "CSVFile.h"

CBmdModel::CBmdModel()
{
}

CBmdModel::~CBmdModel()
{
}

bool CBmdModel::LoadFile(const std::string& strFilename)
{
	CMUBmd bmd;
	static CMUBmd* pPlayerBmd;
	bool bIsPlayerPart = false;
	if (!bmd.LoadFile(strFilename))
	{
		return false;
	}
	if (GetFilename(strFilename)!="player.bmd")
	{
		std::string str = GetFilename(GetParentPath(strFilename));
		if (str=="player")
		{
			if (pPlayerBmd)
			{
				bIsPlayerPart= true;
			}
			else
			{
				pPlayerBmd = new CMUBmd;
				bIsPlayerPart = pPlayerBmd->LoadFile(GetParentPath(strFilename)+"player.bmd");
			}
		}
	}



	//////////////////////////////////////////////////////////////////////////
	m_Mesh.m_Lods.resize(1);
	if (bmd.nFrameCount>1)// if there one frame only, free the animlist
	{
		m_AnimList.resize(bmd.head.uAnimCount);
		int nFrameCount = 0;
		for (size_t i=0; i<bmd.head.uAnimCount; ++i)
		{
			m_AnimList[i].timeStart = nFrameCount*MU_BMD_ANIM_FRAME_TIME;
			nFrameCount+=bmd.bmdSkeleton.setBmdAnim[i].uFrameCount+1;
			m_AnimList[i].timeEnd = (nFrameCount-1)*MU_BMD_ANIM_FRAME_TIME;
		}
	}

	for (size_t i=0;  i<bmd.setBmdSub.size(); ++i)
	{
		for(std::vector<CMUBmd::BmdSub::BmdTriangle>::iterator it=bmd.setBmdSub[i].setTriangle.begin(); it!=bmd.setBmdSub[i].setTriangle.end(); it++)
		{
			FaceIndex faceIndex;
			faceIndex.uSubID=i;
			for (size_t j=0; j<3; ++j)
			{
				faceIndex.v[j]	= m_Mesh.pos.size()+it->indexVertex[2-j];
				faceIndex.b[j]		= m_Mesh.bone.size()+it->indexVertex[2-j];
				faceIndex.w[j]	= m_Mesh.weight.size()+it->indexVertex[2-j];

				faceIndex.n[j]	= m_Mesh.normal.size()+it->indexNormal[2-j];
				faceIndex.uv1[j]		= m_Mesh.texcoord.size()+it->indexUV[2-j];
			}
			m_Mesh.m_setFaceIndex.push_back(faceIndex);
		}
		for(std::vector<CMUBmd::BmdSub::BmdPos>::iterator it=bmd.setBmdSub[i].setVertex.begin(); it!=bmd.setBmdSub[i].setVertex.end(); it++)
		{
			Vec3D vPos = fixCoordSystemPos(it->vPos);
			if (bIsPlayerPart)
			{
				vPos = pPlayerBmd->bmdSkeleton.getLocalMatrix(it->uBones)*vPos;
			}
			else
			{
				vPos = bmd.bmdSkeleton.getLocalMatrix(it->uBones)*vPos;
			}
			if (1<bmd.nFrameCount||bIsPlayerPart)
			{
				uint8 uBone = it->uBones&0xFF;
				if (bmd.bmdSkeleton.setBmdBone.size()<=uBone||bmd.bmdSkeleton.setBmdBone[uBone].bEmpty)
				{
					m_Mesh.bone.push_back(0);
				}
				else
				{
					m_Mesh.bone.push_back(it->uBones);
				}
				//assert((it->uBones&0xFFFFFF00)==0);
				m_Mesh.weight.push_back(0x000000FF);
			}
			m_Mesh.pos.push_back(vPos);
		}
		for(std::vector<CMUBmd::BmdSub::BmdNormal>::iterator it=bmd.setBmdSub[i].setNormal.begin(); it!=bmd.setBmdSub[i].setNormal.end(); it++)
		{
			Vec3D n = fixCoordSystemNormal(it->vNormal);
			n = bmd.bmdSkeleton.getRotateMatrix(it->uBones)*n;
			m_Mesh.normal.push_back(n);
		}
		for(std::vector<Vec2D>::iterator it=bmd.setBmdSub[i].setUV.begin(); it!=bmd.setBmdSub[i].setUV.end(); it++)
		{
			m_Mesh.texcoord.push_back(*it);
		}
		{
			ModelRenderPass pass;
			pass.nSubID = i;
			//pass.nColorID = passes[j].nColorID;
			//pass.nBlendMode =0;

			// ÎÆÀíÑ°Ö·
			pass.bSwrap = true;
			pass.bTwrap = true;

			//pass.bNoZWrite = false;
			//pass.bTrans =  (pass.nBlendMode >0);// && (pass.opacity>0);
			//pass.bUnlit = RenderFlag & eMR_Unlit;

			//pass.p = modelLod.Geosets[passes[j].nGeosetID].v.z;
			pass.bUseEnvMap = false;
			pass.bCull = true;
			pass.material.bAlphaTest = true;
			pass.material.uAlphaTestValue = 0x80;
			std::string strTexFileName = GetParentPath(strFilename) + bmd.setBmdSub[i].szTexture;
			{
				std::string strExtension = GetExtension(bmd.setBmdSub[i].szTexture); 
				if (".jpg"==strExtension)
					strTexFileName = ChangeExtension(strTexFileName,".ozj");
				else if (".tga"==strExtension)
					strTexFileName = ChangeExtension(strTexFileName,".ozt");
				else if (".bmp"==strExtension)
					strTexFileName = ChangeExtension(strTexFileName,".ozb");
			}
			pass.material.uDiffuse = GetRenderSystem().GetTextureMgr().RegisterTexture(strTexFileName);

			pass.material.bCull = false;
			m_Passes.push_back(pass);
		}
	}

	m_Skeleton.m_BoneAnims.resize(bmd.head.uBoneCount);

	std::vector<BoneAnim>::iterator itBoneAnim = m_Skeleton.m_BoneAnims.begin();
	for (std::vector<CMUBmd::BmdSkeleton::BmdBone>::iterator itBmdBone=bmd.bmdSkeleton.setBmdBone.begin();itBmdBone!=bmd.bmdSkeleton.setBmdBone.end();itBmdBone++)
	{
		if (!itBmdBone->bEmpty)
		{
			itBoneAnim->strName = itBmdBone->szName;
			Matrix	mInvLocal = itBmdBone->mLocal;
			mInvLocal.Invert();
			itBoneAnim->mSkin = mInvLocal;
			{
				std::vector<uint32>& KeyTimes = itBoneAnim->trans.m_KeyTimes;
				std::vector<Vec3D>& KeyData = itBoneAnim->trans.m_KeyData;
				for (std::vector<Vec3D>::iterator it= itBmdBone->setTrans.begin();
					it!=itBmdBone->setTrans.end(); it++)
				{
					size_t uTime = KeyTimes.size()*MU_BMD_ANIM_FRAME_TIME;
					if (GetFilename(strFilename)=="player.bmd"&&itBmdBone==bmd.bmdSkeleton.setBmdBone.begin())
					{
						if (uTime>m_AnimList[15].timeStart&&uTime<m_AnimList[23].timeStart)
						{
							KeyTimes.push_back(uTime);
							Vec3D vPos = fixCoordSystemPos(*it);
							vPos.x=0;vPos.z=0;
							KeyData.push_back(vPos);
							continue;
						}
					}
					KeyTimes.push_back(uTime);
					KeyData.push_back(fixCoordSystemPos(*it));
				}
			}
			{
				std::vector<uint32>& KeyTimes = itBoneAnim->rot.m_KeyTimes;
				std::vector<Quaternion>& KeyData = itBoneAnim->rot.m_KeyData;
				for (std::vector<Vec3D>::iterator it= itBmdBone->setRotate.begin();
					it!=itBmdBone->setRotate.end(); it++)
				{
					KeyTimes.push_back(KeyTimes.size()*MU_BMD_ANIM_FRAME_TIME);
					KeyData.push_back(fixCoordSystemRotate(*it));
				}
			}
			int nParent = itBmdBone->nParent;
			if (nParent<0||nParent>255)
			{
				nParent = 255;
			}
			itBoneAnim->parent=nParent;	
		}
		itBoneAnim++;
	}

	m_Mesh.Update();

	m_bbox = m_Mesh.m_bbox;
	std::string strMyPath ="Data\\"+GetFilename(GetParentPath(strFilename))+"\\";
	std::string strMatFilename = ChangeExtension(strFilename,".mat.csv");
	std::string strParFilename = ChangeExtension(strFilename,".par.csv");
	if (!IOReadBase::Exists(strMatFilename))
	{
		strMatFilename=strMyPath+ChangeExtension(GetFilename(strFilename),".mat.csv");
	}
	if (!IOReadBase::Exists(strParFilename))
	{
		strParFilename=strMyPath+ChangeExtension(GetFilename(strFilename),".par.csv");
	}

	loadMaterial(strMatFilename,GetParentPath(strFilename));
	loadParticleEmitters(strParFilename,GetParentPath(strFilename));

	bLoaded=true;
	return true;
}