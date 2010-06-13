#pragma once
#include "UIComboNumber.h"

class CUIComboVec3D : public CUICombo
{
public:
	CUIComboVec3D();
	~CUIComboVec3D();
public:
	virtual void XMLParse(const TiXmlElement* pControlElement);
	void setExtentSize(float fExtentSize);
	virtual void OnControlRegister();
	Vec3D	getVec3D();
	void	setVec3D(Vec3D color);
	void	OnNumChanged();
private:
	CUIComboNumber	m_NumX;
	CUIComboNumber	m_NumY;
	CUIComboNumber	m_NumZ;
};