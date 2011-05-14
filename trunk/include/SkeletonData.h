#pragma once
#include "InterfaceModel.h"

class CSkeletonData:public iSkeletonData
{
public:
	virtual int		getBoneIDByName(const char* szName);
	virtual size_t	getAnimationCount();
	virtual bool	getAnimation(const std::string& strName, long& timeCount)const;
	virtual bool	getAnimation(size_t index, std::string& strName, long& timeCount)const;
	virtual bool	delAnimation(const std::string& strName);
	void			calcBonesTree(int nBoneID,std::vector<Matrix>& setBonesMatrix,std::vector<bool>& setCalc)const;
	void			CalcBonesMatrix(const std::string& strAnim, int time, std::vector<Matrix>& setBonesMatrix);	// º∆À„∂Øª≠÷°æÿ’Û
};