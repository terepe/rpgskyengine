#pragma once
#include "Common.h"
#include "Vec2D.h"
#include "Vec3D.h"
#include "Vec4D.h"
#include "Matrix.h"

class DLL_EXPORT CBaseCamera
{
public:
	CBaseCamera();
	virtual void	FrameMove(float fElapsedTime) = 0;

	// Functions to change camera matrices
	virtual void Reset(); 
	virtual void SetViewParams(Vec3D& vEyePt, Vec3D& vLookatPt);
	virtual void SetProjParams(float fFOV, int nWidth, int nHeight, float fNearPlane, float fFarPlane);
	virtual void SetFarClip(float fFarPlane);
	// Functions to change behavior
	virtual void SetDrag(bool bMovementDrag, float fTotalDragTimeToZero = 0.25f) { m_bMovementDrag = bMovementDrag; m_fTotalDragTimeToZero = fTotalDragTimeToZero; }
	virtual void SetScalers(float fRotationScaler = 0.01f, float fMoveScaler = 5.0f)  { m_fRotationScaler = fRotationScaler; m_fMoveScaler = fMoveScaler; }

	// Functions to get state
	virtual const Matrix&  GetViewMatrix() const { return m_mView; }
	virtual const Matrix&  GetProjMatrix() const { return m_mProj; }

	virtual const Vec3D& GetEyePt() const	  { return m_vEye; }
	virtual const Vec3D& GetLookAtPt() const   { return m_vLookAt; }
	virtual float GetNearClip() const { return m_fNearPlane; }
	virtual float GetFarClip() const { return m_fFarPlane; }

	virtual void	addMouseDelta(Vec3D vMouseDelta){m_vMouseDelta+=vMouseDelta;}

	virtual float	getYawAngle()const			{ return m_fCameraYawAngle; }
	virtual float	getPitchAngle()const		{ return m_fCameraPitchAngle; }
	virtual void	setYawAngle(float fAngle) 	{ m_fCameraYawAngle = fAngle; }
	virtual void	setPitchAngle(float fAngle)	{ m_fCameraPitchAngle = fAngle; }
protected:
	// 更新周转率
	virtual void UpdateVelocity(float fElapsedTime);
	Matrix			m_mView;		   // View matrix 
	Matrix			m_mProj;		   // Projection matrix

	POINT			m_ptLastMousePosition;  // Last absolute position of mouse cursor

	Vec3D			m_vMouseDelta;		  // Mouse relative delta smoothed over a few frames

	Vec3D		m_vDefaultEye;		  // Default camera eye position
	Vec3D		m_vDefaultLookAt;	   // Default LookAt position
	Vec3D		m_vEye;				// Camera eye position
	Vec3D		m_vLookAt;		   // LookAt position
	// 摄像机（左右）偏航角度
	float			m_fCameraYawAngle;
	// 摄像机（上下）倾斜角度
	float			m_fCameraPitchAngle;

	// 摄像机的周转率
	Vec3D		m_vVelocity;
	// 是否运动托曳
	bool			m_bMovementDrag;		// If true, then camera movement will slow to a stop otherwise movement is instant
	// 托曳速率
	Vec3D		m_vVelocityDrag;		// Velocity drag force
	// 倒计时去运用托曳
	float			m_fDragTimer;		// Countdown timer to apply drag
	// 托曳总时间
	float			m_fTotalDragTimeToZero; // Time it takes for velocity to go from full to 0

	Vec3D		m_vRotVelocity;		 // Velocity of camera

	float			m_fFOV;				// Field of view
	int				m_nSceneWidth;
	int				m_nSceneHeight;
	float			m_fAspect;		   // Aspect ratio
	float			m_fNearPlane;		// Near plane
	float			m_fFarPlane;		 // Far plane

	float			m_fRotationScaler;	  // Scaler for rotation
	float			m_fMoveScaler;		  // Scaler for movement

	bool			m_bEnableYAxisMovement; // If true, then camera can move in the y-axis
};