#include "c3_key.h"


void C3Key::Clear ()
{
	dwAlphas = 0;
	lpAlphas = 0;

	dwDraws = 0;
	lpDraws = 0;

	dwChangeTexs = 0;
	lpChangeTexs = 0;
}


bool C3Key::ProcessAlpha (uint32 dwFrame, uint32 dwFrames, float* fReturn)
{
	// alpha
	int sindex = -1;
	int eindex = -1;
	for (int n = 0; n < (int)dwAlphas; n++)
	{
		if (lpAlphas[n].nFrame <= (int)dwFrame)
		{
			if (sindex == -1 || n > sindex)
				sindex = n;
		}
		if (lpAlphas[n].nFrame > (int)dwFrame)
		{
			if (eindex == -1 || n < eindex)
				eindex = n;
		}
	}
	if (sindex == -1 && eindex > -1)
	{
		*fReturn = lpAlphas[eindex].fParam[0];
	}
	else
	if (sindex > -1 && eindex == -1)
	{
		*fReturn = lpAlphas[sindex].fParam[0];
	}
	else
	if (sindex > -1 && eindex > -1)
	{
		*fReturn = lpAlphas[sindex].fParam[0] + 
				   ((float)(dwFrame - lpAlphas[sindex].nFrame) / (float)(lpAlphas[eindex].nFrame - lpAlphas[sindex].nFrame)) * 
					(lpAlphas[eindex].fParam[0] - lpAlphas[sindex].fParam[0]);
	}
	else
		return false;
	return true;
}

bool C3Key::ProcessDraw (uint32 dwFrame, bool* bReturn)
{
	// draw
	for (int n = 0; n < (int)dwDraws; n++)
	{
		if (lpDraws[n].nFrame == (int)dwFrame)
		{
			*bReturn = lpDraws[n].bParam[0];
			return true;
		}
	}
	return false;
}

bool C3Key::ProcessChangeTex (uint32 dwFrame, int* nReturn)
{
	// changetex
	for (int n = 0; n < (int)dwChangeTexs; n++)
	{
		if (lpChangeTexs[n].nFrame == (int)dwFrame)
		{
			*nReturn = lpChangeTexs[n].nParam[0];
			return true;
		}
	}
	return false;
}
