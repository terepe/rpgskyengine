#pragma once
#include "Common.h"

struct C3Frame
{
	int			nFrame;				// ¹Ø¼üÖ¡Î»ÖÃ
	float		fParam[1];			// float param
	bool		bParam[1];			// bool param
	int			nParam[1];			// int param
};
class C3Key
{
public:
	uint32		dwAlphas;
	C3Frame*	lpAlphas;

	uint32		dwDraws;
	C3Frame*	lpDraws;

	uint32		dwChangeTexs;
	C3Frame*	lpChangeTexs;

	void Clear ();
	bool Load (C3Key **lpKey, char *lpName, uint32 dwIndex);
	bool Save (char *lpName, bool bNew);
	void Unload();

	bool ProcessAlpha (uint32 dwFrame, uint32 dwFrames, float* fReturn);
	bool ProcessDraw (uint32 dwFrame, bool* bReturn);
	bool ProcessChangeTex (uint32 dwFrame, int* nReturn);
};

