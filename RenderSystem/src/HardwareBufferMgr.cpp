#include "HardwareBufferMgr.h"
#include "RenderSystem.h"

CHardwareVertexBuffer* CHardwareBufferMgr::CreateVertexBuffer(size_t numVerts, size_t vertexSize,
																  CHardwareBuffer::Usage usage)
{
	CHardwareVertexBuffer* vbuf = GetRenderSystem().newHardwareVertexBuffer();
	if (vbuf->create(vertexSize, numVerts, usage, false)==false)
	{
		S_DEL(vbuf);
		return NULL;
	}
	vbuf->setHardwareBufferMgr(this);
	m_setVertexHardwareBuffer.insert(vbuf);
	return vbuf;
}

CHardwareIndexBuffer* CHardwareBufferMgr::CreateIndexBuffer(size_t numIndexes, CHardwareIndexBuffer::IndexType itype,
																CHardwareBuffer::Usage usage)
{
	CHardwareIndexBuffer* idx = GetRenderSystem().newHardwareIndexBuffer();
	if (idx->create(itype, numIndexes, usage, false)==false)
	{
		S_DEL(idx);
		return NULL;
	}
	idx->setHardwareBufferMgr(this);
	m_setIndexHardwareBuffer.insert(idx);
	return idx;
}

void CHardwareBufferMgr::_notifyVertexBufferDestroyed(CHardwareVertexBuffer* pHardwareBuffer)
{
	std::set<CHardwareVertexBuffer*>::iterator it = m_setVertexHardwareBuffer.find(pHardwareBuffer); 
	if (it!=m_setVertexHardwareBuffer.end())
	{
		m_setVertexHardwareBuffer.erase(it);
	}
}

void CHardwareBufferMgr::_notifyIndexBufferDestroyed(CHardwareIndexBuffer* pHardwareBuffer)
{
	std::set<CHardwareIndexBuffer*>::iterator it = m_setIndexHardwareBuffer.find(pHardwareBuffer); 
	if (it!=m_setIndexHardwareBuffer.end())
	{
		m_setIndexHardwareBuffer.erase(it);
	}
}