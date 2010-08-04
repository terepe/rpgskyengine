#pragma once
#include "RenderSystemCommon.h"
#include <list>

class DLL_EXPORT CVertexDeclaration
{
public:
	CVertexDeclaration();
	virtual ~CVertexDeclaration();
public:
	void AddElement(uint16 uSource, uint16 uOffset, VertexElementType type, VertexElementSemantic semantic, uint8 uIndex = 0);
	virtual void EndElement()=0;
protected:
	uint32 GetTypeSize(VertexElementType type);
	std::list<VertexElement> m_ElementList;

};