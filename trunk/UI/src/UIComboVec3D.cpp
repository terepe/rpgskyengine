#include "UIComboVec3D.h"

CUIComboVec3D::CUIComboVec3D()
{
}

CUIComboVec3D::~CUIComboVec3D()
{	
}

void CUIComboVec3D::setExtentSize(float fExtentSize)
{
	m_NumX.setExtentSize(fExtentSize);
	m_NumY.setExtentSize(fExtentSize);
	m_NumZ.setExtentSize(fExtentSize);
}

void CUIComboVec3D::OnControlRegister()
{
	CUICombo::OnControlRegister();

	RegisterControl("IDC_NUM_X",m_NumX);
	RegisterControl("IDC_NUM_Y",m_NumY);
	RegisterControl("IDC_NUM_Z",m_NumZ);

	RegisterControlEvent("IDC_SLIDER_R",(PEVENT)&CUIComboVec3D::OnNumChanged);
	RegisterControlEvent("IDC_SLIDER_G",(PEVENT)&CUIComboVec3D::OnNumChanged);
	RegisterControlEvent("IDC_SLIDER_B",(PEVENT)&CUIComboVec3D::OnNumChanged);

	setExtentSize(0.01f);
}

Vec3D CUIComboVec3D::getVec3D()
{
	Vec3D vec;
	vec.x=m_NumX.getFloat();
	vec.y=m_NumY.getFloat();
	vec.z=m_NumZ.getFloat();
	return vec;
}

void CUIComboVec3D::setVec3D(Vec3D vec)
{
	m_NumX.setFloat(vec.x);
	m_NumY.setFloat(vec.y);
	m_NumZ.setFloat(vec.z);
}

void CUIComboVec3D::OnNumChanged()
{
	SendEvent(EVENT_DEFAULT, this);
}
