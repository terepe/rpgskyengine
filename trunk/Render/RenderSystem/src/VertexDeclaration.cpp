#include "VertexDeclaration.h"
//#include "RenderSystem.h"

CVertexDeclaration::CVertexDeclaration()
{
}

CVertexDeclaration::~CVertexDeclaration()
{
}

void CVertexDeclaration::AddElement(uint16 uSource, uint16 uOffset, VertexElementType type, VertexElementSemantic semantic, uint8 uIndex)
{
	VertexElement vertexElement = {uSource, uOffset, type, semantic, uIndex};
	m_ElementList.push_back(vertexElement);
}

uint32 CVertexDeclaration::GetTypeSize(VertexElementType type)
{
	switch(type)
	{
	case VET_COLOUR:
	case VET_COLOUR_ABGR:
	case VET_COLOUR_ARGB:
		return sizeof(Color32);
	case VET_FLOAT1:
		return sizeof(float);
	case VET_FLOAT2:
		return sizeof(float)*2;
	case VET_FLOAT3:
		return sizeof(float)*3;
	case VET_FLOAT4:
		return sizeof(float)*4;
	case VET_SHORT1:
		return sizeof(short);
	case VET_SHORT2:
		return sizeof(short)*2;
	case VET_SHORT3:
		return sizeof(short)*3;
	case VET_SHORT4:
		return sizeof(short)*4;
	case VET_UBYTE4:
		return sizeof(unsigned char)*4;
	}
	return 0;
}