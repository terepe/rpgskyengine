#include "LodMesh.h"
#include "RenderSystem.h"
#include "Intersect.h"
#include "LumpFile.h"

void CBoundMesh::draw()const
{
	if (indices.size())
	{
		CRenderSystem& R = GetRenderSystem();
		if (R.prepareMaterial("BoundMesh"))
		{
			R.SetFillMode(FILL_WIREFRAME);
			R.DrawIndexedPrimitiveUP(VROT_TRIANGLE_LIST,
				0, indices.size(), indices.size()/3,
				&indices[0], &pos[0], sizeof(Vec3D));
			R.SetFillMode(FILL_SOLID);
			R.finishMaterial();
		}
	}
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

//////////////////////////////////////////////////////////////////////////
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


int CLodMesh::getSubCount()
{
	return m_setSubMesh.size();
}
CSubMesh& CLodMesh::addSubMesh()
{
	size_t size=m_setSubMesh.size();
	m_setSubMesh.resize(size+1);
	return m_setSubMesh[size];
}

CSubMesh* CLodMesh::getSubMesh(size_t n)
{
	if (m_setSubMesh.size()<=n)
	{
		return NULL;
	}
	return &m_setSubMesh[n];
}

const BBox& CLodMesh::getBBox()
{return m_bbox;}


void CLodMesh::Init()
{
	size_t uVertexCount=0;
	std::vector<std::vector<VertexIndex>> setVecVertexIndex;
	if (m_setSubMesh.size()!=0)
	{
		setVecVertexIndex.resize(m_setSubMesh.size());
		m_Lods.resize(1);
		IndexedSubset subset;
		std::vector<uint16> setIndex;
		for (size_t i=0;i<m_setSubMesh.size();++i)
		{
			CSubMesh& subMesh = m_setSubMesh[i];
			std::vector<VertexIndex>& setVertexIndex=setVecVertexIndex[i];

			transformRedundance(subMesh.m_setVertexIndex,setVertexIndex,setIndex);
			subset.vstart += subset.vcount;
			subset.vbase = subset.vstart;
			subset.istart += subset.icount;
			subset.vcount = setVertexIndex.size();
			subset.icount = setIndex.size()-subset.istart;
			m_Lods[0].setSubset.push_back(subset);
			uVertexCount+=setVertexIndex.size();
		}
		transformRedundance(setIndex,m_Lods[0].IndexLookup,m_Lods[0].Indices);
	}
	else
	{
		/*for (size_t i=0; i<pos.size(); ++i)
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
		}*/
	}
	if (m_bSkinMesh)
	{
		for (size_t i=0;i<setVecVertexIndex.size();++i)
		{
			CSubMesh& subMesh=m_setSubMesh[i];
			std::vector<VertexIndex>& setVertexIndex=setVecVertexIndex[i];
			for (size_t n=0;n<setVertexIndex.size();++n)
			{
				VertexIndex& vertexIndex=setVertexIndex[n];
				SkinVertex skinVertex;
				skinVertex.p = subMesh.pos[vertexIndex.p];
				skinVertex.n = subMesh.normal[vertexIndex.n];
				skinVertex.w4 = subMesh.weight[vertexIndex.w];
				skinVertex.b4 = subMesh.bone[vertexIndex.b];
				m_setSkinVertex.push_back(skinVertex);
			}
		}
	}
	bool bPos		= false;
	bool bNormal	= false;
	bool bColor		= false;
	bool bTexCoord	= false;
	bool bTexCoord2	= false;
	for (size_t i=0;i<m_setSubMesh.size();++i)
	{
		CSubMesh& subMesh=m_setSubMesh[i];
		if (subMesh.pos.size()>0)
		{
			bPos=true;
		}
		if (subMesh.normal.size()>0)
		{
			bNormal=true;
		}
		if (subMesh.color.size()>0)
		{
			bColor=true;
		}
		if (subMesh.texcoord.size()>0)
		{
			bTexCoord=true;
		}
		if (subMesh.texcoord2.size()>0)
		{
			bTexCoord2=true;
		}
	}

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
	//// 当为蒙皮时，只创建固定的公用的纹理UV缓存 // 创建刚体网格的公用的VB
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

	// 	m_dwFVF |= bColor?D3DFVF_DIFFUSE:0;
	// 	m_dwFVF |= bTexCoord?D3DFVF_TEX1:0;

	// 当为蒙皮时，只创建固定的公用的纹理UV缓存
	// 创建刚体网格的公用的VB
	if (m_uShareVertexSize > 0)
	{
		m_pShareBuffer = GetRenderSystem().GetHardwareBufferMgr().CreateVertexBuffer(uVertexCount, m_uShareVertexSize);
		if (m_pShareBuffer)
		{
			uint8* pBuffer = (uint8*)m_pShareBuffer->lock(CHardwareBuffer::HBL_NORMAL);
			if (pBuffer)
			{
				for (size_t i=0;i<setVecVertexIndex.size();++i)
				{
					CSubMesh& subMesh=m_setSubMesh[i];
					std::vector<VertexIndex>& setVertexIndex=setVecVertexIndex[i];
					for (size_t n=0;n<setVertexIndex.size();++n)
					{
						VertexIndex& vertexIndex=setVertexIndex[n];
							if (!m_bSkinMesh)
						{
							if (bPos)
							{
								*(Vec3D*)pBuffer = subMesh.pos[vertexIndex.p];
								pBuffer += sizeof(Vec3D);
							}
							if (bNormal)
							{
								*(Vec3D*)pBuffer = subMesh.normal[vertexIndex.n];
								pBuffer += sizeof(Vec3D);
							}
						}
						if (bColor)
						{
							*(Color32*)pBuffer = subMesh.color[vertexIndex.c];
							pBuffer += sizeof(Color32);
						}
						if (bTexCoord)
						{
							*(Vec2D*)pBuffer = subMesh.texcoord[vertexIndex.uv1];
							pBuffer += sizeof(Vec2D);
						}
						if (bTexCoord2)
						{
							*(Vec2D*)pBuffer = subMesh.texcoord2[vertexIndex.uv2];
							pBuffer += sizeof(Vec2D);
						}
					}
				}
				for (uint32 i=0; i<uVertexCount; i++)
				{
					
				}
			}
			m_pShareBuffer->unlock();
		}
	}

	// 填入IB
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

bool CLodMesh::SetMeshSource(int nLodLevel, CHardwareVertexBuffer* pSkinVB)const
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

void CLodMesh::drawSub(size_t uSubID, size_t uLodLevel)const
{
	if (m_Lods.size()<=uLodLevel)
	{
		return;
	}
	if (m_Lods[uLodLevel].setSubset.size()<=uSubID)
	{
		return;
	}
	const IndexedSubset &subset = m_Lods[uLodLevel].setSubset[uSubID];
	GetRenderSystem().drawIndexedSubset(subset);
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

void CLodMesh::skinningMesh(CHardwareVertexBuffer* pVB, std::vector<Matrix>& setBonesMatrix)const
{
	// 运算顶点
	if (pVB)
	{
		//uint32 dwLockFlag = D3DLOCK_NOOVERWRITE;// 不等待 直接写入
		//if (1/*useEnvMapping*/)// 当一个Mesh被叠加多遍渲染的话 Lock 要等待GPU渲染完成或另开内存，否则会出现叠面
		//{
		//	dwLockFlag = 0;//D3DLOCK_DISCARD;// 另开一块内存写入
		//}
		uint16 uCount	= m_setSkinVertex.size();
		//ModelVertex* pVIn		= &[0];
		const SkinVertex* pSkinVertex	= &m_setSkinVertex[0];


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
						v += setBonesMatrix[bone]*pSkinVertex->p*fWeight;
						Matrix mRot=setBonesMatrix[bone];
						mRot._14=0;
						mRot._24=0;
						mRot._34=0;
						n += mRot*pSkinVertex->n*fWeight;
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
	m_setSubMesh.clear();
}

void CLodMesh::update()
{
	InitBBox();
}

void CLodMesh::InitBBox()
{
	if (m_setSubMesh.size()==0)
	{
		return;
	}
	{
		std::vector<Vec3D>::iterator it=m_setSubMesh[0].pos.begin();
		if (it!=m_setSubMesh[0].pos.end())
		{
			m_bbox.vMin = *it;
			m_bbox.vMax = *it;
		}
	}

	for(size_t i=0;i<m_setSubMesh.size();++i)
	{
		std::vector<Vec3D>& pos = m_setSubMesh[i].pos;
		for (std::vector<Vec3D>::iterator it=pos.begin();it!=pos.end();++it)
		{
			m_bbox.vMin.x = min(it->x,m_bbox.vMin.x);
			m_bbox.vMin.y = min(it->y,m_bbox.vMin.y);
			m_bbox.vMin.z = min(it->z,m_bbox.vMin.z);

			m_bbox.vMax.x = max(it->x,m_bbox.vMax.x);
			m_bbox.vMax.y = max(it->y,m_bbox.vMax.y);
			m_bbox.vMax.z = max(it->z,m_bbox.vMax.z);

			//
			float fLenSquared = it->lengthSquared();
			if (fLenSquared > m_fRad)
			{
				m_fRad = fLenSquared;
			}
		}
		if (false==m_bSkinMesh)
		{
			std::vector<uint32>& bone = m_setSubMesh[i].bone;
			for (std::vector<uint32>::iterator it=bone.begin();it!=bone.end();++it)
			{
				if (*it>0)
				{
					m_bSkinMesh = true;
					break;
				}
			}
		}
	}

	if (!m_bSkinMesh)
	{
		//weight.clear();
		//bone.clear();
	}
	m_fRad = sqrtf(m_fRad);
}

void CLodMesh::load(CNodeData& lump)
{
	//lump.getVector("pos",		pos);
//	lump.getVector("normal",	normal);
//	lump.getVector("color",		color);
//	lump.getVector("uv1",		texcoord);
//	lump.getVector("uv2",		texcoord2);
//	lump.getVector("weight",	weight);
//	lump.getVector("bone",		bone);
	// error lump.getVector("face",		m_mapVertexIndex);
	update();
	//m_Lods.resize(1);
}

void CLodMesh::save(CNodeData& lump)
{
//	lump.SetVector("pos",		pos);
//	lump.SetVector("normal",	normal);
//	lump.SetVector("color",		color);
//	lump.SetVector("uv1",		texcoord);
//	lump.SetVector("uv2",		texcoord2);
//	lump.SetVector("weight",	weight);
//	lump.SetVector("bone",		bone);
	// error lump.SetVector("face",		m_setVertexIndex);
}


bool SubMeshIntersect(const CSubMesh& subMesh,const Vec3D& vRayPos , const Vec3D& vRayDir, Vec3D& vOut)
{
	size_t size=subMesh.m_setVertexIndex.size()/3;
	for(size_t i=0;i<size;++i)
	{
		if (IntersectTri(subMesh.pos[subMesh.m_setVertexIndex[i*3].p],subMesh.pos[subMesh.m_setVertexIndex[i*3+1].p],subMesh.pos[subMesh.m_setVertexIndex[i*3+2].p],vRayPos,vRayDir,vOut))
		{
			return true;
		}
	}
	return false;
}

bool CLodMesh::intersect(const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D& vOut, int& nSubID)const
{
	for (size_t i=0;i<m_setSubMesh.size();++i)
	{
		if (SubMeshIntersect(m_setSubMesh[i],vRayPos,vRayDir,vOut))
		{
			nSubID = i;
			return true;
		}
	}
	return false;
}

bool CLodMesh::intersect(const Vec3D& vRayPos , const Vec3D& vRayDir)const
{
	Vec3D vOut;
	int nSubID;
	return intersect(vRayPos, vRayDir, vOut, nSubID);
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
	m_setSubMesh.resize(3);
	const size_t CIRCLE_LINE_COUNT=6;
	m_setSubMesh[0].addPos(Vec3D(1,0,0));
	for(size_t i=0; i<CIRCLE_LINE_COUNT; ++i)
	{
		float fRadian = PI*i*2/CIRCLE_LINE_COUNT;
		Vec3D v(0.75f,sinf(fRadian)*0.05f,cosf(fRadian)*0.05f);
		m_setSubMesh[0].addPos(v);
	}
	m_setSubMesh[1].addPos(Vec3D(0,1,0));
	for (size_t i=0; i<CIRCLE_LINE_COUNT; ++i)
	{
		float fRadian = PI*i*2/CIRCLE_LINE_COUNT;
		Vec3D v(cosf(fRadian)*0.05f,0.75f,sinf(fRadian)*0.05f);
		m_setSubMesh[1].addPos(v);
	}
	m_setSubMesh[2].addPos(Vec3D(0,0,1));
	for (size_t i=0; i<CIRCLE_LINE_COUNT; ++i)
	{
		float fRadian = PI*i*2/CIRCLE_LINE_COUNT;
		Vec3D v(sinf(fRadian)*0.05f,cosf(fRadian)*0.05f,0.75f);
		m_setSubMesh[2].addPos(v);
	}
	//
	m_setSubMesh[0].addColor(0xFFFF0000);
	m_setSubMesh[1].addColor(0xFF00FF00);
	m_setSubMesh[2].addColor(0xFF0000FF);
	m_setSubMesh[0].addColor(0xFF880000);
	m_setSubMesh[1].addColor(0xFF008800);
	m_setSubMesh[2].addColor(0xFF000088);
	//
	for (size_t i=0;i<3;++i)
	{
		size_t start = 0;//i*(CIRCLE_LINE_COUNT+1);
		for (size_t j=0;j<CIRCLE_LINE_COUNT-1;++j)
		{
			VertexIndex vertexIndex;
			vertexIndex.c=0;
			vertexIndex.p=0+start;
			m_setSubMesh[i].m_setVertexIndex.push_back(vertexIndex);
			vertexIndex.p=1+j+start;
			m_setSubMesh[i].m_setVertexIndex.push_back(vertexIndex);
			vertexIndex.p=2+j+start;
			m_setSubMesh[i].m_setVertexIndex.push_back(vertexIndex);
		}
		{
			VertexIndex vertexIndex;
			vertexIndex.c=0;
			vertexIndex.p=0+start;
			m_setSubMesh[i].m_setVertexIndex.push_back(vertexIndex);
			vertexIndex.p=CIRCLE_LINE_COUNT+start;
			m_setSubMesh[i].m_setVertexIndex.push_back(vertexIndex);
			vertexIndex.p=1+start;
			m_setSubMesh[i].m_setVertexIndex.push_back(vertexIndex);
		}
		for (size_t j=0;j<CIRCLE_LINE_COUNT-2;++j)
		{
			VertexIndex vertexIndex;
			vertexIndex.c=1;
			vertexIndex.p=1+start;
			m_setSubMesh[i].m_setVertexIndex.push_back(vertexIndex);
			vertexIndex.p=2+j+start;
			m_setSubMesh[i].m_setVertexIndex.push_back(vertexIndex);
			vertexIndex.p=3+j+start;
			m_setSubMesh[i].m_setVertexIndex.push_back(vertexIndex);
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

	Vec3D vOut;
	int nSubID  = -1;
	if (CLodMesh::intersect(vNewRayPos, vNewRayDir, vOut, nSubID))
	{
		switch(nSubID)
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
	return false;
}

#include "Graphics.h"
void CMeshCoordinate::render(const Vec3D& vCoordShow)
{
	if (m_setSubMesh.size()==0)
	{
		init();
	}
	CRenderSystem& R = GetRenderSystem();
	CGraphics& G=GetGraphics();

	R.setWorldMatrix(getWorldMatrix());

	if (R.prepareMaterial("Coordinate1"))
	{
		SetMeshSource();
		draw();

		G.DrawLine3D(m_CoordLines[CLT_X].vBegin,m_CoordLines[CLT_X].vEnd,vCoordShow.x>0?0xFFFF00:0xFFFF0000);
		G.DrawLine3D(m_CoordLines[CLT_X_Y].vBegin,m_CoordLines[CLT_X_Y].vEnd,(vCoordShow.x>0&&vCoordShow.y>0)?0xFFFF00:0xFFFF0000);
		G.DrawLine3D(m_CoordLines[CLT_X_Z].vBegin,m_CoordLines[CLT_X_Z].vEnd,(vCoordShow.x>0&&vCoordShow.z>0)?0xFFFF00:0xFFFF0000);

		G.DrawLine3D(m_CoordLines[CLT_Y].vBegin,m_CoordLines[CLT_Y].vEnd,vCoordShow.y>0?0xFFFF00:0xFF00FF00);
		G.DrawLine3D(m_CoordLines[CLT_Y_X].vBegin,m_CoordLines[CLT_Y_X].vEnd,(vCoordShow.x>0&&vCoordShow.y>0)?0xFFFF00:0xFF00FF00);
		G.DrawLine3D(m_CoordLines[CLT_Y_Z].vBegin,m_CoordLines[CLT_Y_Z].vEnd,(vCoordShow.y>0&&vCoordShow.z>0)?0xFFFF00:0xFF00FF00);

		G.DrawLine3D(m_CoordLines[CLT_Z].vBegin,m_CoordLines[CLT_Z].vEnd,vCoordShow.z>0?0xFFFF00:0xFF0000FF);
		G.DrawLine3D(m_CoordLines[CLT_Z_X].vBegin,m_CoordLines[CLT_Z_X].vEnd,(vCoordShow.x>0&&vCoordShow.z>0)?0xFFFF00:0xFF0000FF);
		G.DrawLine3D(m_CoordLines[CLT_Z_Y].vBegin,m_CoordLines[CLT_Z_Y].vEnd,(vCoordShow.y>0&&vCoordShow.z>0)?0xFFFF00:0xFF0000FF);

		R.finishMaterial();
	}
	if (R.prepareMaterial("Coordinate2"))
	{
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
		R.finishMaterial();
	}
}

Matrix CMeshCoordinate::getWorldMatrix()const
{
	Matrix mTrans,mScale;
	mTrans.translation(m_vPos);
	mScale.scale(Vec3D(m_fScale,m_fScale,m_fScale));

	return mTrans*mScale;
}