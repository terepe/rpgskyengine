#pragma once
#include "HardwareBuffer.h"

class CHardwareVertexBuffer : public CHardwareBuffer
{
protected:

	size_t mNumVertices;
	size_t mVertexSize;

public:
	CHardwareVertexBuffer();
	~CHardwareVertexBuffer();
	/// Should be called by HardwareBufferManager
	virtual bool create(size_t vertexSize, size_t numVertices, CHardwareBuffer::Usage usage, bool useSystemMemory);
	/// Gets the size in bytes of a single vertex in this buffer
	size_t getVertexSize(void) const { return mVertexSize; }
	/// Get the number of vertices in this buffer
	size_t getNumVertices(void) const { return mNumVertices; }
	// NB subclasses should override lock, unlock, readData, writeData
};