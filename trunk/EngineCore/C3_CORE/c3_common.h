#pragma once
#ifndef	STRICT
#define	STRICT
#endif

#ifndef	WIN32_LEAN_AND_MEAN
#define	WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <stdio.h>
#include <math.h>
#include <process.h>
#include <signal.h>


#include "IORead.h"
//#include "LodMesh.h"
#include "Common.h"
#include "Matrix.h"
#include "Vec3D.h"

/*--------------------------------------------------------
	Chunk 头结构 (ChunkHeader)
	Examples: typedef ChunkHeader WaveChunkHeader;
--------------------------------------------------------*/
struct ChunkHeader
{
	uint32 dwChunkID;							// Chunk ID
	uint32 dwChunkSize;							// Chunk 大小
};

