#pragma once
#include "modelheaders.h"
#include "InterfaceModel.h"

class CSkeleton:public iSkeleton
{
public:
	virtual size_t getAnimationCount();
	//virtual void setAnimation(const std::string& strName, long timeCount);
	virtual bool getAnimation(const std::string& strName, long& timeCount)const;
	virtual bool getAnimation(size_t index, std::string& strName, long& timeCount)const;
	virtual bool delAnimation(const std::string& strName);

	uint8 getIDByName(const std::string& strName);
	//void GetBoneMatrix(int nBoneID, int time, bool bRotate=true);
	void calcBonesTree(int nBoneID,std::vector<Matrix>& setBonesMatrix,std::vector<bool>& setCalc)const;
	void CalcBonesMatrix(const std::string& strAnim, int time, std::vector<Matrix>& setBonesMatrix);	// º∆À„∂Øª≠÷°æÿ’Û
	void calcBonesPoint(const std::vector<Matrix>& setBonesMatrix, std::vector<Vec3D>& setBonesPoint)const;
	void Render(const std::vector<Matrix>& setBonesMatrix)const;
	CNodeData* Save(CNodeData& lump, const char* name);
	CNodeData* Load(CNodeData& lump, const char* name);
};