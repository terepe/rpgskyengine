#include "LodMesh.h"
#include "RenderSystem.h"
#include "Intersect.h"
#include "LumpFile.h"

void CBoundMesh::Draw()
{
	if (indices.size())
	{
		CRenderSystem& R = GetRenderSystem();
		R.SetAlphaTestFunc(false);
		R.SetBlendFunc(false);
		R.SetDepthBufferFunc(true, true);
		R.SetLightingEnabled(false);

		R.SetTextureFactor(0xFFFFFFFF);
		R.SetTextureColorOP(0,TBOP_SOURCE1, TBS_TFACTOR);
		R.SetTextureAlphaOP(0,TBOP_DISABLE);

		R.SetFillMode(FILL_WIREFRAME);
		R.DrawIndexedPrimitiveUP(VROT_TRIANGLE_LIST,
			0, indices.size(), indices.size()/3,
			&indices[0], &pos[0], sizeof(Vec3D));
		R.SetFillMode(FILL_SOLID);
	}
}

CLodMesh::CLodMesh():
m_pShareBuffer(NULL),
m_pVertexDeclHardware(NULL),
m_uSkinVertexSize(0),
m_uShareVertexSize(0),
m_bSkinMesh(false),
m_fRad(1.0f)
{
}

CLodMesh::~CLodMesh()
{
	S_DEL(m_pShareBuffer);
	S_DEL(m_pVertexDeclHardware);
}

template <class _T, class _T2>
void  transformRedundance(const std::vector<_T>& setIn, std::vector<_T>& setOut, std::vector<_T2>& index)
{
	std::map<_T,int> mapSort;
	size_t start = setOut.size();
	for (std::vector<_T>::const_iterator it=setIn.begin();it!=setIn.end();it++)
	{
		mapSort[*it];
	}
	int id = setOut.size();
	for (std::map<_T,int>::iterator it=mapSort.begin();it!=mapSort.end();it++)
	{
		setOut.push_back(it->first);
		it->second=id;
		id++;
	}
	for (std::vector<_T>::const_iterator it=setIn.begin();it!=setIn.end();it++)
	{
		index.push_back(mapSort[*it]);
	}
}

void CLodMesh::Init()
{
	std::vector<VertexIndex> setVertexIndex;
	if (m_setFaceIndex.size()!=0)
	{
		m_Lods.resize(1);
		std::map<uint16,std::vector<VertexIndex>> mapSubs;
		for (size_t i=0; i<m_setFaceIndex.size(); ++i)
		{
			for (size_t n=0; n<3; ++n)
			{
				VertexIndex vertexIndex;
				vertexIndex.p = m_setFaceIndex[i].v[n];
				vertexIndex.n = m_setFaceIndex[i].n[n];
				vertexIndex.c = m_setFaceIndex[i].c[n];
				vertexIndex.uv1 = m_setFaceIndex[i].uv1[n];
				vertexIndex.uv2 = m_setFaceIndex[i].uv2[n];
				vertexIndex.w = m_setFaceIndex[i].w[n];
				vertexIndex.b = m_setFaceIndex[i].b[n];
				mapSubs[m_setFaceIndex[i].uSubID].push_back(vertexIndex);
			}
		}
		IndexedSubset subset;
		std::vector<uint16> setIndex;
		for (std::map<uint16,std::vector<VertexIndex>>::iterator itSub = mapSubs.begin();
			itSub!=mapSubs.end(); itSub++)
		{
			transformRedundance(itSub->second,setVertexIndex,setIndex);
			subset.vstart += subset.vcount;
			subset.istart += subset.icount;
			subset.vcount = setVertexIndex.size()-subset.vstart;
			subset.icount = setIndex.size()-subset.istart;
			m_Lods[0].setSubset.push_back(subset);
		}
		transformRedundance(setIndex,m_Lods[0].IndexLookup,m_Lods[0].Indices);
	}
	else
	{
		for (size_t i=0; i<pos.size(); ++i)
		{
			VertexIndex vertexIndex;
			if (i<pos.size())
			{
				vertexIndex.p=i;
			}
			if (i<normal.size())
			{
				vertexIndex.n=i;
			}
			if (i<color.size())
			{
				vertexIndex.c=i;
			}
			if (i<texcoord.size())
			{
				vertexIndex.uv1=i;
			}
			if (i<texcoord2.size())
			{
				vertexIndex.uv2=i;
			}
			if (i<weight.size())
			{
				vertexIndex.w=i;
			}
			if (i<bone.size())
			{
				vertexIndex.b=i;
			}
			setVertexIndex.push_back(vertexIndex);
		}
	}
	if (m_bSkinMesh)
	{
		for (size_t i=0; i<setVertexIndex.size(); ++i)
		{
			SkinVertex skinVertex;
			skinVertex.p = pos[setVertexIndex[i].p];
			skinVertex.n = normal[setVertexIndex[i].n];
			skinVertex.w4 = weight[setVertexIndex[i].w];
			skinVertex.b4 = bone[setVertexIndex[i].b];
			m_setSkinVertex.push_back(skinVertex);
		}
	}

	bool bPos		=	pos.size() > 0;
	bool bNormal	=	normal.size() > 0;
	bool bColor		=	color.size() > 0;
	bool bTexCoord	=	texcoord.size() > 0;
	bool bTexCoord2	=	texcoord2.size() > 0;

	m_pVertexDeclHardware = GetRenderSystem().CreateVertexDeclaration();

	uint32 dwFVF = 0;
	uint16 uOffset = 0;
	m_uSkinVertexSize = 0;
	uint16 uStream = 0;
	// pos
	if (bPos)
	{
		m_pVertexDeclHardware->AddElement(uStream, uOffset, VET_FLOAT3, VES_POSITION);
		uOffset += sizeof(Vec3D);
	}
	// normal
	if (bNormal)
	{
		m_pVertexDeclHardware->AddElement(uStream, uOffset, VET_FLOAT3, VES_NORMAL);
		uOffset += sizeof(Vec3D);
	}
	//// ��Ϊ��Ƥʱ��ֻ�����̶��Ĺ��õ�����UV���� // ������������Ĺ��õ�VB
	if (m_bSkinMesh)
	{
		m_uSkinVertexSize = uOffset;
		uOffset = 0; 
		uStream = 1;
	}
	// color
	if (bColor)
	{
		m_pVertexDeclHardware->AddElement(uStream, uOffset, VET_COLOUR, VES_DIFFUSE);
		uOffset += sizeof(uint32);
	}
	// texCoord
	if (bTexCoord)
	{
		m_pVertexDeclHardware->AddElement(uStream, uOffset, VET_FLOAT2, VES_TEXTURE_COORDINATES);
		uOffset += sizeof(Vec2D);
	}
	// texCoord2
	if (bTexCoord2)
	{
		m_pVertexDeclHardware->AddElement(uStream, uOffset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 1);
		uOffset += sizeof(Vec2D);
	}
	//
	m_pVertexDeclHardware->EndElement();
	m_uShareVertexSize = uOffset;

	uint32 uVertexCount = setVertexIndex.size();
	// 	m_dwFVF |= bColor?D3DFVF_DIFFUSE:0;
	// 	m_dwFVF |= bTexCoord?D3DFVF_TEX1:0;

	// ��Ϊ��Ƥʱ��ֻ�����̶��Ĺ��õ�����UV����
	// ������������Ĺ��õ�VB
	if (m_uShareVertexSize > 0)
	{
		m_pShareBuffer = GetRenderSystem().GetHardwareBufferMgr().CreateVertexBuffer(uVertexCount, m_uShareVertexSize);
		if (m_pShareBuffer)
		{
			uint8* pBuffer = (uint8*)m_pShareBuffer->lock(CHardwareBuffer::HBL_NORMAL);
			if (pBuffer)
			{
				for (uint32 i=0; i<uVertexCount; i++)
				{
					if (!m_bSkinMesh)
					{
						if (bPos)
						{
							*(Vec3D*)pBuffer = pos[setVertexIndex[i].p];
							pBuffer += sizeof(Vec3D);
						}
						if (bNormal)
						{
							*(Vec3D*)pBuffer = normal[setVertexIndex[i].n];
							pBuffer += sizeof(Vec3D);
						}
					}
					if (bColor)
					{
						*(Color32*)pBuffer = color[setVertexIndex[i].c];
						pBuffer += sizeof(Color32);
					}
					if (bTexCoord)
					{
						*(Vec2D*)pBuffer = texcoord[setVertexIndex[i].uv1];
						pBuffer += sizeof(Vec2D);
					}
					if (bTexCoord2)
					{
						*(Vec2D*)pBuffer = texcoord2[setVertexIndex[i].uv2];
						pBuffer += sizeof(Vec2D);
					}
				}
			}
			m_pShareBuffer->unlock();
		}
	}

	// ����IB
	for(std::vector<ModelLod>::iterator it = m_Lods.begin(); it!=m_Lods.end(); it++)
	{
		size_t indexSize = it->Indices.size();
		if (indexSize>0)
		{
			it->pIB = GetRenderSystem().GetHardwareBufferMgr().CreateIndexBuffer(indexSize);
			if (it->pIB)
			{
				uint16* indices = (uint16*)it->pIB->lock(CHardwareBuffer::HBL_NORMAL);
				if (indices)
				{
					for (uint32 i = 0; i< indexSize; i++)
					{
						indices[i] = it->IndexLookup[ it->Indices[i] ];
					}
				}
				it->pIB->unlock();
			}
		}
	}
}

uint32 CLodMesh::GetSkinVertexSize()
{
	return m_uSkinVertexSize;
}

bool CLodMesh::SetMeshSource(int nLodLevel, CHardwareVertexBuffer* pSkinVB)
{
	CRenderSystem& R = GetRenderSystem();
	if (m_bSkinMesh)
	{
		if (pSkinVB)
		{
			R.SetVertexDeclaration(m_pVertexDeclHardware);
			R.SetStreamSource(0, pSkinVB, 0, sizeof(SkinnedVertex));
			if (m_pShareBuffer)
			{
				R.SetStreamSource(1, m_pShareBuffer, 0, m_uShareVertexSize);
			}
			//GetRenderSystem().GetDevice()->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | 1ul);
			//GetRenderSystem().GetDevice()->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INDEXEDDATA | 1ul);
			//GetRenderSystem().GetDevice()->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul);
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (m_pShareBuffer)
		{
			R.SetVertexDeclaration(m_pVertexDeclHardware);
			R.SetStreamSource(0, m_pShareBuffer, 0, m_uShareVertexSize);
		}
		else
		{
			return false;
		}
	}
	if (m_Lods[ nLodLevel ].pIB)
	{
		R.SetIndices(m_Lods[ nLodLevel ].pIB);	
	}
	else
	{
		return false;
	}
	return true;
}

void CLodMesh::DrawSub(int nLodLevel,int nSubID)const
{
	CRenderSystem& R = GetRenderSystem();
	const IndexedSubset &subset = m_Lods[ nLodLevel ].setSubset[nSubID];
	R.drawIndexedSubset(subset);
}

void CLodMesh::draw(size_t uLodLevel)const
{
	if (m_Lods.size()>uLodLevel)
	{
		CRenderSystem& R = GetRenderSystem();
		const std::vector<IndexedSubset>& setSubset = m_Lods[ uLodLevel ].setSubset;
		for (std::vector<IndexedSubset>::const_iterator it=setSubset.begin(); it!=setSubset.end(); it++)
		{
			R.drawIndexedSubset(*it);
		}
	}
}

void CLodMesh::SkinMesh(CHardwareVertexBuffer* pVB, std::vector<CBone>& bones)
{
	// ���㶥��
	if (pVB)
	{
		//uint32 dwLockFlag = D3DLOCK_NOOVERWRITE;// ���ȴ� ֱ��д��
		//if (1/*useEnvMapping*/)// ��һ��Mesh�����Ӷ����Ⱦ�Ļ� Lock Ҫ�ȴ�GPU��Ⱦ��ɻ������ڴ棬�������ֵ���
		//{
		//	dwLockFlag = 0;//D3DLOCK_DISCARD;// ����һ���ڴ�д��
		//}
		uint16 uCount	= m_setSkinVertex.size();
		//ModelVertex* pVIn		= &[0];
		SkinVertex* pSkinVertex	= &m_setSkinVertex[0];


		SkinnedVertex* pVOut = (SkinnedVertex*)pVB->lock(CHardwareBuffer::HBL_NO_OVERWRITE);
		if (pVOut)
		{
			for (size_t i=0; i<uCount; ++i, pSkinVertex++, pVOut++)
			{
				Vec3D v(0,0,0);
				Vec3D n(0,0,0);
				for (uint8 b=0; b<4; b++)
				{
					uint8 bone = pSkinVertex->b[b];
					uint8 weight = pSkinVertex->w[b];
					if (weight!=0)
					{
						float fWeight = weight / 255.0f;
						v += bones[bone].m_mat*pSkinVertex->p*fWeight;
						n += bones[bone].m_mRot*pSkinVertex->n*fWeight;
					}
				}
				pVOut->p = v;
				pVOut->n = n;
			}
		}
		pVB->unlock();
	}
}

void CLodMesh::Clear()
{
	pos.clear();
	normal.clear();
	color.clear();
	texcoord.clear();
	texcoord2.clear();
	weight.clear();
	bone.clear();
}

void CLodMesh::update()
{
	InitBBox();
}

void CLodMesh::InitBBox()
{
	std::vector<Vec3D>::iterator it=pos.begin();
	if (it!=pos.end())
	{
		m_bbox.vMin = *it;
		m_bbox.vMax = *it;
	}
	for (;it!=pos.end(); it++)
	{
		m_bbox.vMin.x = min(it->x,m_bbox.vMin.x);
		m_bbox.vMin.y = min(it->y,m_bbox.vMin.y);
		m_bbox.vMin.z = min(it->z,m_bbox.vMin.z);

		m_bbox.vMax.x = max(it->x,m_bbox.vMax.x);
		m_bbox.vMax.y = max(it->y,m_bbox.vMax.y);
		m_bbox.vMax.z = max(it->z,m_bbox.vMax.z);

	}
	for (uint32 i=0; i<bone.size(); i++)
	{
		if (false==m_bSkinMesh)
		{
			uint8* bones = (uint8*)&(bone[i]);
			for (size_t b=0; b<4; b++)
			{
				if (bones[b]>0)//have many bons!
				{
					m_bSkinMesh = true;
					break;
				}
			}
		}
	}

	for (uint32 i=0; i<pos.size(); i++)
	{
		float fLenSquared = pos[i].lengthSquared();
		if (fLenSquared > m_fRad)
		{
			m_fRad = fLenSquared;
		}
	}
	if (!m_bSkinMesh)
	{
		weight.clear();
		bone.clear();
	}
	m_fRad = sqrtf(m_fRad);
}

void CLodMesh::load(CLumpNode& lump)
{
	lump.getVector("pos",		pos);
	lump.getVector("normal",	normal);
	lump.getVector("color",		color);
	lump.getVector("uv1",		texcoord);
	lump.getVector("uv2",		texcoord2);
	lump.getVector("weight",	weight);
	lump.getVector("bone",		bone);
	lump.getVector("face",		m_setFaceIndex);
	update();
	//m_Lods.resize(1);
}

void CLodMesh::save(CLumpNode& lump)
{
	lump.SetVector("pos",		pos);
	lump.SetVector("normal",	normal);
	lump.SetVector("color",		color);
	lump.SetVector("uv1",		texcoord);
	lump.SetVector("uv2",		texcoord2);
	lump.SetVector("weight",	weight);
	lump.SetVector("bone",		bone);
	lump.SetVector("face",		m_setFaceIndex);
}

bool CLodMesh::intersect(const Vec3D& vRayPos , const Vec3D& vRayDir)
{
	for (std::vector<FaceIndex>::const_iterator it=m_setFaceIndex.begin(); it!=m_setFaceIndex.end(); it++)
	{
		Vec3D vOut;
		if (IntersectTri(pos[it->v[0]],pos[it->v[1]],pos[it->v[2]],vRayPos,vRayDir,vOut))
		{
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
CMeshCoordinate::CMeshCoordinate()
{
	m_CoordLines[CLT_X].vBegin = Vec3D(0.75f,0,0);
	m_CoordLines[CLT_X].vEnd = Vec3D(0.2f,0,0);
	m_CoordLines[CLT_X_Y].vBegin = Vec3D(0.3f,0,0);
	m_CoordLines[CLT_X_Y].vEnd = Vec3D(0.3f,0.3f,0);
	m_CoordLines[CLT_X_Z].vBegin = Vec3D(0.3f,0,0);
	m_CoordLines[CLT_X_Z].vEnd = Vec3D(0.3f,0,0.3f);

	m_CoordLines[CLT_Y].vBegin = Vec3D(0,0.75f,0);
	m_CoordLines[CLT_Y].vEnd = Vec3D(0,0.2f,0);
	m_CoordLines[CLT_Y_X].vBegin = Vec3D(0,0.3f,0);
	m_CoordLines[CLT_Y_X].vEnd = Vec3D(0.3f,0.3f,0);
	m_CoordLines[CLT_Y_Z].vBegin = Vec3D(0,0.3f,0);
	m_CoordLines[CLT_Y_Z].vEnd = Vec3D(0,0.3f,0.3f);

	m_CoordLines[CLT_Z].vBegin = Vec3D(0,0,0.75f);
	m_CoordLines[CLT_Z].vEnd = Vec3D(0,0,0.2f);
	m_CoordLines[CLT_Z_X].vBegin = Vec3D(0,0,0.3f);
	m_CoordLines[CLT_Z_X].vEnd = Vec3D(0.3f,0,0.3f);
	m_CoordLines[CLT_Z_Y].vBegin = Vec3D(0,0,0.3f);
	m_CoordLines[CLT_Z_Y].vEnd = Vec3D(0,0.3f,0.3f);
}


CMeshCoordinate::~CMeshCoordinate()
{
}

void CMeshCoordinate::init()
{
	const size_t CIRCLE_LINE_COUNT=6;
	Vec3D vPos = Vec3D(1,0,0);
	pos.push_back(vPos);
	for(size_t j=0; j<CIRCLE_LINE_COUNT; ++j)
	{
		float fRadian = PI*j*2/CIRCLE_LINE_COUNT;
		Vec3D v(0.75f,sinf(fRadian)*0.05f,cosf(fRadian)*0.05f);
		pos.push_back(v);
	}

	for (size_t i=0; i<=CIRCLE_LINE_COUNT; ++i)
	{
		Vec3D v=pos[i];
		pos.push_back(Vec3D(v.z,v.x,v.y));
	}
	for (size_t i=0; i<=CIRCLE_LINE_COUNT; ++i)
	{
		Vec3D v=pos[i];
		pos.push_back(Vec3D(v.y,v.z,v.x));

	}
	//
	color.push_back(0xFFFF0000);
	color.push_back(0xFF00FF00);
	color.push_back(0xFF0000FF);
	color.push_back(0xFF880000);
	color.push_back(0xFF008800);
	color.push_back(0xFF000088);
	//
	for (size_t i=0;i<3;++i)
	{
		size_t start = i*(CIRCLE_LINE_COUNT+1);
		for (size_t j=0;j<CIRCLE_LINE_COUNT-1;++j)
		{
			FaceIndex faceIndex;
			faceIndex.uSubID = i;
			faceIndex.v[0]=0+start;
			faceIndex.v[1]=1+j+start;
			faceIndex.v[2]=2+j+start;
			faceIndex.c[0]=i;
			faceIndex.c[1]=i;
			faceIndex.c[2]=i;
			m_setFaceIndex.push_back(faceIndex);
		}
		{
			FaceIndex faceIndex;
			faceIndex.uSubID = i;
			faceIndex.v[0]=0+start;
			faceIndex.v[1]=CIRCLE_LINE_COUNT+start;
			faceIndex.v[2]=1+start;
			faceIndex.c[0]=i;
			faceIndex.c[1]=i;
			faceIndex.c[2]=i;
			m_setFaceIndex.push_back(faceIndex);
		}
		for (size_t j=0;j<CIRCLE_LINE_COUNT-2;++j)
		{
			FaceIndex faceIndex;
			faceIndex.uSubID = i;
			faceIndex.v[0]=1+start;
			faceIndex.v[1]=2+j+start;
			faceIndex.v[2]=3+j+start;
			faceIndex.c[0]=i+3;
			faceIndex.c[1]=i+3;
			faceIndex.c[2]=i+3;
			m_setFaceIndex.push_back(faceIndex);
		}
	}

	//
	Init();
	InitBBox();
}

void CMeshCoordinate::setPos(const Vec3D& vPos)
{
	m_vPos = vPos;
}

void CMeshCoordinate::setScale(float fScale)
{
	m_fScale = fScale;
}

bool CMeshCoordinate::intersect(const Vec3D& vRayPos , const Vec3D& vRayDir,Vec3D& vCoord)const
{
	Vec3D vNewRayPos = vRayPos;
	Vec3D vNewRayDir = vRayDir;
	transformRay(vNewRayPos,vNewRayDir,getWorldMatrix());

	float fMin,fMax;
	float fR = 0.05f*m_fScale;
	if(LineCapsuleIntersector(vNewRayPos,vNewRayDir,m_CoordLines[CLT_X].vBegin,m_CoordLines[CLT_X].vEnd,fR,fMin,fMax)>0)
	{
		vCoord = Vec3D(1,0,0);
		return true;
	}
	if(LineCapsuleIntersector(vNewRayPos,vNewRayDir,m_CoordLines[CLT_X_Y].vBegin,m_CoordLines[CLT_X_Y].vEnd,fR,fMin,fMax)>0)
	{
		vCoord = Vec3D(1,1,0);
		return true;
	}
	if(LineCapsuleIntersector(vNewRayPos,vNewRayDir,m_CoordLines[CLT_X_Z].vBegin,m_CoordLines[CLT_X_Z].vEnd,fR,fMin,fMax)>0)
	{
		vCoord = Vec3D(1,0,1);
		return true;
	}

	if(LineCapsuleIntersector(vNewRayPos,vNewRayDir,m_CoordLines[CLT_Y].vBegin,m_CoordLines[CLT_Y].vEnd,fR,fMin,fMax)>0)
	{
		vCoord = Vec3D(0,1,0);
		return true;
	}
	if(LineCapsuleIntersector(vNewRayPos,vNewRayDir,m_CoordLines[CLT_Y_X].vBegin,m_CoordLines[CLT_Y_X].vEnd,fR,fMin,fMax)>0)
	{
		vCoord = Vec3D(1,1,0);
		return true;
	}
	if(LineCapsuleIntersector(vNewRayPos,vNewRayDir,m_CoordLines[CLT_Y_Z].vBegin,m_CoordLines[CLT_Y_Z].vEnd,fR,fMin,fMax)>0)
	{
		vCoord = Vec3D(0,1,1);
		return true;
	}

	if(LineCapsuleIntersector(vNewRayPos,vNewRayDir,m_CoordLines[CLT_Z].vBegin,m_CoordLines[CLT_Z].vEnd,fR,fMin,fMax)>0)
	{
		vCoord = Vec3D(0,0,1);
		return true;
	}
	if(LineCapsuleIntersector(vNewRayPos,vNewRayDir,m_CoordLines[CLT_Z_X].vBegin,m_CoordLines[CLT_Z_X].vEnd,fR,fMin,fMax)>0)
	{
		vCoord = Vec3D(1,0,1);
		return true;
	}
	if(LineCapsuleIntersector(vNewRayPos,vNewRayDir,m_CoordLines[CLT_Z_Y].vBegin,m_CoordLines[CLT_Z_Y].vEnd,fR,fMin,fMax)>0)
	{
		vCoord = Vec3D(0,1,1);
		return true;
	}

	for (std::vector<FaceIndex>::const_iterator it=m_setFaceIndex.begin(); it!=m_setFaceIndex.end(); it++)
	{
		Vec3D vOut;
		if (IntersectTri(pos[it->v[0]],pos[it->v[1]],pos[it->v[2]],vNewRayPos,vNewRayDir,vOut))
		{
			switch(it->uSubID)
			{
			case 0:
				vCoord = Vec3D(1,0,0);
				break;
			case 1:
				vCoord = Vec3D(0,1,0);
				break;
			case 2:
				vCoord = Vec3D(0,0,1);
			    break;
			default:
			    break;
			}
			return true;
		}
	}
	return false;
}

#include "Graphics.h"
void CMeshCoordinate::render(const Vec3D& vCoordShow)
{
	if (pos.size()==0)
	{
		init();
	}
	CRenderSystem& R = GetRenderSystem();

	R.setWorldMatrix(getWorldMatrix());
	R.SetDepthBufferFunc(true,true);
	R.SetAlphaTestFunc(false);
	R.SetBlendFunc(false);
	R.SetLightingEnabled(false);
	R.SetTextureColorOP(0,TBOP_SOURCE1, TBS_DIFFUSE);
	R.SetTextureAlphaOP(0,TBOP_DISABLE);
	SetMeshSource();
	draw();

	CGraphics& G=GetGraphics();
	G.DrawLine3D(m_CoordLines[CLT_X].vBegin,m_CoordLines[CLT_X].vEnd,vCoordShow.x>0?0xFFFF00:0xFFFF0000);
	G.DrawLine3D(m_CoordLines[CLT_X_Y].vBegin,m_CoordLines[CLT_X_Y].vEnd,(vCoordShow.x>0&&vCoordShow.y>0)?0xFFFF00:0xFFFF0000);
	G.DrawLine3D(m_CoordLines[CLT_X_Z].vBegin,m_CoordLines[CLT_X_Z].vEnd,(vCoordShow.x>0&&vCoordShow.z>0)?0xFFFF00:0xFFFF0000);

	G.DrawLine3D(m_CoordLines[CLT_Y].vBegin,m_CoordLines[CLT_Y].vEnd,vCoordShow.y>0?0xFFFF00:0xFF00FF00);
	G.DrawLine3D(m_CoordLines[CLT_Y_X].vBegin,m_CoordLines[CLT_Y_X].vEnd,(vCoordShow.x>0&&vCoordShow.y>0)?0xFFFF00:0xFF00FF00);
	G.DrawLine3D(m_CoordLines[CLT_Y_Z].vBegin,m_CoordLines[CLT_Y_Z].vEnd,(vCoordShow.y>0&&vCoordShow.z>0)?0xFFFF00:0xFF00FF00);

	G.DrawLine3D(m_CoordLines[CLT_Z].vBegin,m_CoordLines[CLT_Z].vEnd,vCoordShow.z>0?0xFFFF00:0xFF0000FF);
	G.DrawLine3D(m_CoordLines[CLT_Z_X].vBegin,m_CoordLines[CLT_Z_X].vEnd,(vCoordShow.x>0&&vCoordShow.z>0)?0xFFFF00:0xFF0000FF);
	G.DrawLine3D(m_CoordLines[CLT_Z_Y].vBegin,m_CoordLines[CLT_Z_Y].vEnd,(vCoordShow.y>0&&vCoordShow.z>0)?0xFFFF00:0xFF0000FF);




	R.SetBlendFunc(true);
	R.SetTextureAlphaOP(0,TBOP_SOURCE1, TBS_DIFFUSE);
	if (vCoordShow.x>0&&vCoordShow.y>0)
	{
		G.fillQuad(Vec3D(0,0,0),m_CoordLines[CLT_X_Y].vBegin,m_CoordLines[CLT_X_Y].vEnd,m_CoordLines[CLT_Y_X].vBegin,0x44FFFF00);
	}
	if (vCoordShow.y>0&&vCoordShow.z>0)
	{
		G.fillQuad(Vec3D(0,0,0),m_CoordLines[CLT_Y_Z].vBegin,m_CoordLines[CLT_Y_Z].vEnd,m_CoordLines[CLT_Z_Y].vBegin,0x44FFFF00);
	}
	if (vCoordShow.z>0&&vCoordShow.x>0)
	{
		G.fillQuad(Vec3D(0,0,0),m_CoordLines[CLT_Z_X].vBegin,m_CoordLines[CLT_Z_X].vEnd,m_CoordLines[CLT_X_Z].vBegin,0x44FFFF00);
	}
}

Matrix CMeshCoordinate::getWorldMatrix()const
{
	Matrix mTrans,mScale;
	mTrans.translation(m_vPos);
	mScale.scale(Vec3D(m_fScale,m_fScale,m_fScale));

	return mTrans*mScale;
}