#include "RenderSystem.h"

CRenderSystem* g_pRenderSystem = NULL;

void SetRenderSystem(CRenderSystem* pRenderSystem)
{
	g_pRenderSystem = pRenderSystem;
}

CRenderSystem& GetRenderSystem()
{
	//assert(g_pRenderSystem);
	return *g_pRenderSystem;
}

CRenderSystem::CRenderSystem()
{
}
CRenderSystem::~CRenderSystem()
{
}

void CRenderSystem::world2Screen(const Vec3D& vWorldPos, Pos2D& posScreen)
{
	Matrix mProj;
	Matrix View;
	getProjectionMatrix(mProj);
	getViewMatrix(View);
	Vec4D vOut = mProj*View*Vec4D(vWorldPos,1);
	float fW = vOut.w;
	RECT rc;
	getViewport(rc);
	posScreen.x = int(rc.left+(rc.right-rc.left)*(0.5f+vOut.x*0.5f/fW));
	posScreen.y = int(rc.top+(rc.bottom-rc.top)*(0.5f-vOut.y*0.5f/fW));
}

void CRenderSystem::GetPickRay(Vec3D& vRayPos, Vec3D& vRayDir,int x, int y)
{
	Matrix mProj;
	getProjectionMatrix(mProj);
	RECT rc;
	getViewport(rc);
	Vec3D v;
	v.x =  (((2.0f * (x-rc.left)) / (rc.right-rc.left)) - 1) / mProj._11;
	v.y = -(((2.0f * (y-rc.top)) / (rc.bottom-rc.top)) - 1) / mProj._22;
	v.z =  1.0f;

	Matrix mView;
	getViewMatrix(mView);
	Matrix mViewInv = mView;
	mViewInv.Invert();
	//vRayDir = mViewInv*v-Vec3D(mView._14,mView._24,mView._34);
	vRayDir.normalize();
	//vRayPos = m_vEye;
}