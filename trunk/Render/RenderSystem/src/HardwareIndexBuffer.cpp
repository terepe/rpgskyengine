#include "HardwareIndexBuffer.h"
#include "RenderSystem.h"

CHardwareIndexBuffer::CHardwareIndexBuffer(IndexType idxType, 
										 size_t numIndexes, CHardwareBuffer::Usage usage, 
										 bool useSystemMemory) 
										 : CHardwareBuffer(usage, useSystemMemory), mIndexType(idxType), mNumIndexes(numIndexes)
{
	// Calculate the size of the indexes
	switch (mIndexType)
	{
	case IT_16BIT:
		mIndexSize = sizeof(unsigned short);
		break;
	case IT_32BIT:
		mIndexSize = sizeof(unsigned int);
		break;
	}
	mSizeInBytes = mIndexSize * mNumIndexes;
}
//-----------------------------------------------------------------------------
CHardwareIndexBuffer::~CHardwareIndexBuffer()
{
	GetRenderSystem().GetHardwareBufferMgr()._notifyIndexBufferDestroyed(this);
	//HardwareBufferManager* mgr = HardwareBufferManager::getSingletonPtr();
	//if (mgr)
	//{
	//	mgr->_notifyIndexBufferDestroyed(this);
	//}
}