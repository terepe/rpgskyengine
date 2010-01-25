#pragma once
#include "Common.h"
#include "HardwareIndexBuffer.h"
#include "HardwareVertexBuffer.h"
#include <set>

class DLL_EXPORT CHardwareBufferMgr
{
public:
	CHardwareBufferMgr() {}
	virtual ~CHardwareBufferMgr() {}
	// ´´½¨VB IB
	virtual CHardwareVertexBuffer* CreateVertexBuffer(size_t numVerts, size_t vertexSize,
		CHardwareBuffer::Usage usage=CHardwareBuffer::HBU_STATIC) = 0;
	virtual CHardwareIndexBuffer* CreateIndexBuffer(size_t numIndexes, CHardwareIndexBuffer::IndexType itype = CHardwareIndexBuffer::IT_16BIT,
		CHardwareBuffer::Usage usage=CHardwareBuffer::HBU_STATIC) = 0;
	virtual void _notifyVertexBufferDestroyed(CHardwareVertexBuffer* pHardwareBuffer)=0;
	virtual void _notifyIndexBufferDestroyed(CHardwareIndexBuffer* pHardwareBuffer)=0;
protected:
	std::set<CHardwareVertexBuffer*>	m_setVertexHardwareBuffer;
	std::set<CHardwareIndexBuffer*>		m_setIndexHardwareBuffer;
};