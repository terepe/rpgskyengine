#include "HardwareVertexBuffer.h"
#include "RenderSystem.h"

CHardwareVertexBuffer::CHardwareVertexBuffer() 
{
}

CHardwareVertexBuffer::~CHardwareVertexBuffer()
{
	if (m_pHardwareBufferMgr)
	{
		m_pHardwareBufferMgr->_notifyVertexBufferDestroyed(this);
	}
	//HardwareBufferManager* mgr = HardwareBufferManager::getSingletonPtr();
	//if (mgr)
	//{
	//	mgr->_notifyVertexBufferDestroyed(this);
	//}
}

bool CHardwareVertexBuffer::create(size_t vertexSize, size_t numVertices, CHardwareBuffer::Usage usage, bool useSystemMemory) 
{
	mUsage = usage;
	mSystemMemory = useSystemMemory;
	// ----
	mNumVertices = vertexSize;
	mVertexSize = numVertices;
	// Calculate the size of the vertices
	mSizeInBytes = mVertexSize * numVertices;
	return true;
}