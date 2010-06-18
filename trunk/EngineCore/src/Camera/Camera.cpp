#include "Camera.h"

// Constructor
CCamera::CCamera()
{
	m_vTargetPos = Vec3D(0, 0, 0);
	m_fRadius    = 5.0f;
	m_fDefaultRadius = 5.0f;
	m_fMinRadius = 1.0f;
	m_fMaxRadius = 80.0f;
	m_fMinPitchAngle = -PI*0.49f;
	m_fMaxPitchAngle = +PI*0.49f;
}

void CCamera::setTargetPos(Vec3D& vPos)
{
	m_vTargetPos = vPos;
}

void CCamera::MoveTarget()
{
	// ����������� yaw & pitch ������ת����
	Matrix mCameraRot;
	mCameraRot.rotationYawPitchRoll(m_fCameraYawAngle, m_fCameraPitchAngle, 0);
	Vec3D vOffset = mCameraRot * Vec3D(-m_vRotVelocity.x, m_vRotVelocity.y, 0);
	vOffset *= m_fRadius* 0.1f;
	m_vTargetPos += vOffset;
	m_vEye += vOffset;
}

void CCamera::MoveTargetWithPlane()
{
	// ����������� yaw ������ת����
	Matrix mCameraRot;
	mCameraRot.rotationYawPitchRoll(m_fCameraYawAngle, 0, 0);
	Vec3D vOffset = mCameraRot * Vec3D(-m_vRotVelocity.x, 0, m_vRotVelocity.y);
	vOffset *= m_fRadius* 0.1f;
	m_vTargetPos += vOffset;
	m_vEye += vOffset;
}

void CCamera::MoveAroundTarget()
{
	// Update the pitch & yaw angle based on mouse movement
	{
		m_fCameraYawAngle   += m_vRotVelocity.x;
		m_fCameraPitchAngle += m_vRotVelocity.y;
		// ��б�Ƕȼ�⣺ֱ�ϻ�ֱ��
		m_fCameraPitchAngle = __max(m_fMinPitchAngle,  m_fCameraPitchAngle);
		m_fCameraPitchAngle = __min(m_fMaxPitchAngle,  m_fCameraPitchAngle);
	}

	// ����������� yaw & pitch ������ת����
	Matrix mCameraRot;
	mCameraRot.rotationYawPitchRoll(m_fCameraYawAngle, m_fCameraPitchAngle, 0);
	//mCameraRot.rotationYawPitchRoll(m_fCameraYawAngle, 0, 0);

	//m_vLookAt  += vPosDeltaWorld;

	// ����Eye����
	Vec3D vViewDir = mCameraRot * Vec3D(0,0,1);
	m_vEye = m_vTargetPos - vViewDir * m_fRadius;// + vPosDeltaWorld
}

void CCamera::FrameMove(float fElapsedTime)
{
	// Get amount of velocity based on the keyboard input and drag (if any)
	UpdateVelocity(fElapsedTime);

	// Simple euler method to calculate position delta
	//Vec3D vPosDelta = m_vVelocity * fElapsedTime;

	// Change the radius from the camera to the model based on wheel scrolling
	m_fRadius += m_vRotVelocity.z;//max(m_nMouseWheelDelta * m_fRadius * 0.1f,0.1f);
	m_fRadius = __min(m_fMaxRadius, m_fRadius);
	m_fRadius = __max(m_fMinRadius, m_fRadius);
	{
	//	MoveTarget();
	}
	//if (m_nCurrentButtonMask & MOUSE_RIGHT_BUTTON)
	{
	//	MoveTargetWithPlane();
	}
	//else
	{
		MoveAroundTarget();
	}

	m_vLookAt = m_vTargetPos;
	// �����Ӿ���
	Vec3D vUp(0,1,0);
	m_mView.MatrixLookAtLH(m_vEye, m_vLookAt, vUp);
	m_Frustum.Build(m_mProj*m_mView);
}

#include "RenderSystem.h"
void CCamera::GetPickRay(Vec3D& vRayPos, Vec3D& vRayDir, int x, int y,const CRect<int>& rc)
{
	Vec3D v;
	v.x =  (((2.0f * (x-rc.left)) / (rc.right-rc.left)) - 1) / m_mProj._11;
	v.y = -(((2.0f * (y-rc.top)) / (rc.bottom-rc.top)) - 1) / m_mProj._22;
	v.z =  1.0f;

	Matrix mViewInv = m_mView;
	mViewInv.Invert();
	vRayDir = mViewInv*v-m_vEye;
	vRayDir.normalize();
	vRayPos = m_vEye;
}

void CCamera::World2Screen(const Vec3D& vWorldPos, Pos2D& posScreen)
{
	Matrix mProjXView = this->GetProjXView();
	Vec4D vOut = mProjXView*Vec4D(vWorldPos,1);
	float fW = vOut.w;
	posScreen.x = int(m_nSceneWidth*(0.5f+vOut.x*0.5f/fW));
	posScreen.y = int(m_nSceneHeight*(0.5f-vOut.y*0.5f/fW));
}