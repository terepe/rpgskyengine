#pragma once
#include "HardwareBuffer.h"

class CHardwareIndexBuffer : public CHardwareBuffer
{
public:
	enum IndexType {
		IT_16BIT,
		IT_32BIT
	};

protected:
	IndexType mIndexType;
	size_t mNumIndexes;
	size_t mIndexSize;

public:
	/// Should be called by HardwareBufferManager
	CHardwareIndexBuffer(IndexType idxType, size_t numIndexes, CHardwareBuffer::Usage usage,
		bool useSystemMemory);
	~CHardwareIndexBuffer();
	/// Get the type of indexes used in this buffer
	IndexType getType(void) const { return mIndexType; }
	/// Get the number of indexes in this buffer
	size_t getNumIndexes(void) const { return mNumIndexes; }
	/// Get the size in bytes of each index
	size_t getIndexSize(void) const { return mIndexSize; }

	// NB subclasses should override lock, unlock, readData, writeData
};