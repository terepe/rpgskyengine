#include "HardwareBufferMgr.h"

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