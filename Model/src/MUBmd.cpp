#include "MUBmd.h"

void decryptMuBuffer(char* buffer, size_t size)
{
	static const uint8 xorKeys[16] = {
		0xd1, 0x73, 0x52, 0xf6,
		0xd2, 0x9a, 0xcb, 0x27,
		0x3e, 0xaf, 0x59, 0x31,
		0x37, 0xb3, 0xe7, 0xa2
	};
	char key = 0x5E;
	for (size_t i=0; i<size; ++i)
	{
		char encode = *buffer;
		*buffer ^= xorKeys[i%16];
		*buffer -= key;
		key = encode+0x3D;
		buffer++;
	}
}

void CMUBmd::BmdSkeleton::BmdAnim::load(char*& p)
{
	STRUCT_MEMCPY(p,uFrameCount);
	STRUCT_MEMCPY(p,bOffset);
	if (bOffset)
	{
		VECTOR_MEMCPY(p,vOffset,uFrameCount);
	}
}

void CMUBmd::BmdSkeleton::BmdBone::load(char*& p, const std::vector<BmdAnim>& setBmdAnim)
{
	STRUCT_MEMCPY(p,bEmpty);
	if (bEmpty)
	{
		return;
	}

	POINT_MEMCPY(p,szName,32);
	STRUCT_MEMCPY(p,nParent);
	for (size_t i=0; i<setBmdAnim.size();++i)
	{
		for (size_t j=0; j<setBmdAnim[i].uFrameCount;++j)
		{
			Vec3D vTrans;
			STRUCT_MEMCPY(p,vTrans);
			setTrans.push_back(vTrans);
		}
		for (size_t j=0; j<setBmdAnim[i].uFrameCount;++j)
		{
			Vec3D vRotate;
			STRUCT_MEMCPY(p,vRotate);
			setRotate.push_back(vRotate);
		}

		if (setBmdAnim[i].uFrameCount>1)
		{
			setTrans.push_back(setTrans[setTrans.size()-setBmdAnim[i].uFrameCount]);
			setRotate.push_back(setRotate[setRotate.size()-setBmdAnim[i].uFrameCount]);
		}
	}
}

Matrix CMUBmd::BmdSkeleton::getLocalMatrix(uint8 uBoneID)
{
	if (setBmdBone.size()>uBoneID)
	{
		return setBmdBone[uBoneID].mLocal;
	}
	return Matrix::UNIT;
}

Matrix CMUBmd::BmdSkeleton::getRotateMatrix(uint8 uBoneID)
{
	Matrix mRotate=getLocalMatrix(uBoneID);
	mRotate._14=0;mRotate._24=0;mRotate._34=0;
	return mRotate;
}

void CMUBmd::BmdSkeleton::calcLocalMatrix(uint8 uBoneID)
{
	//m_bCalc
	if (setBmdBone.size()<=uBoneID)
	{
		return;
	}
	BmdBone& b = setBmdBone[uBoneID];
	Matrix m = Matrix::UNIT;
	if (b.setTrans.size()>0)
	{
		m *= Matrix::newTranslation(fixCoordSystemPos(b.setTrans[0]));
	}
	if (b.setRotate.size()>0)
	{
		m *= Matrix::newQuatRotate(fixCoordSystemRotate(b.setRotate[0]));
	}
	if (b.nParent==-1)
	{
		b.mLocal = m;
	}
	else
	{
		calcLocalMatrix(b.nParent);
		b.mLocal = getLocalMatrix(b.nParent)*m;
	}
}

void CMUBmd::BmdSkeleton:: load(char*& p, uint16 uBoneCount, uint16 uAnimCount)
	{
		setBmdAnim.resize(uAnimCount);
		setBmdBone.resize(uBoneCount);
		for (size_t i=0; i<setBmdAnim.size();++i)
		{
			setBmdAnim[i].load(p);
		}
		for (size_t i=0; i<setBmdBone.size();++i)
		{
			setBmdBone[i].load(p,setBmdAnim);
		}
	}

void CMUBmd::BmdSub::load(char*& p)
{
	STRUCT_MEMCPY(p,head);

	VECTOR_MEMCPY(p,setVertex,head.uVertexCount);
	VECTOR_MEMCPY(p,setNormal,head.uNormal);
	VECTOR_MEMCPY(p,setUV,head.uUVCount);
	VECTOR_MEMCPY(p,setTriangle,head.uTriangleCount);

	POINT_MEMCPY(p,szTexture,32);
}

bool CMUBmd::LoadFile(const std::string& strFilename)
{
	IOReadBase* pRead = IOReadBase::autoOpen(strFilename);
	if (!pRead)
	{
		return false;
	}
	size_t uFileSize = pRead->GetSize();
	std::vector<char> buffer;
	buffer.resize(uFileSize);
	char* p = &buffer[0];
	pRead->Read(p,uFileSize);
	IOReadBase::autoClose(pRead);

	uint32 uTag;
	STRUCT_MEMCPY(p,uTag);
	if (0x0a444d42==uTag)//BMD.
	{
	}
	else if (0x0c444d42==uTag)//BMD.
	{
		uint32 uEncodeSize;
		STRUCT_MEMCPY(p,uEncodeSize);
		if (uEncodeSize!=uFileSize-8)
		{
			return false;
		}
		decryptMuBuffer(p, uFileSize-8);
	}
	else 
	{
		return false;
	}

	STRUCT_MEMCPY(p,head);
	assert(head.uSubCount<50);
	setBmdSub.resize(head.uSubCount);
	for (size_t i=0; i<setBmdSub.size(); ++i)
	{
		setBmdSub[i].load(p);
	}
	bmdSkeleton.load(p, head.uBoneCount,head.uAnimCount);

	for (size_t i=0;i<bmdSkeleton.setBmdBone.size();++i)
	{
		bmdSkeleton.calcLocalMatrix(i);
	}

	nFrameCount = 0;
	for (size_t i=0; i<head.uAnimCount; ++i)
	{
		nFrameCount+=bmdSkeleton.setBmdAnim[i].uFrameCount;
	}
	return true;
}