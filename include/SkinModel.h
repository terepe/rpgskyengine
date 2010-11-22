#pragma once
#include "ModelObject.h"

class CSkinModel: public CModelObject
{
public:
	virtual void frameMove(const Matrix& mWorld, double fTime, float fElapsedTime);
};