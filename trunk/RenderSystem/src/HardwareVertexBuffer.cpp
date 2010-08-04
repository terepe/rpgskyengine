#include "HardwareVertexBuffer.h"
#include "RenderSystem.h"

CHardwareVertexBuffer::CHardwareVertexBuffer(size_t vertexSize,  
										   size_t numVertices, CHardwareBuffer::Usage usage, 
										   bool useSystemMemory) 
										   : CHardwareBuffer(usage, useSystemMemory), 
										   mNumVertices(numVertices),
										   mVertexSize(vertexSize)
{
	// Calculate the size of the vertices
	mSizeInBytes = mVertexSize * numVertices;
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