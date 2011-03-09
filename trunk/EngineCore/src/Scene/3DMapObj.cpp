#include "3DMapObj.h"
#include "Intersect.h"
#include "RenderSystem.h"

C3DMapObj::C3DMapObj()
{
	CMaterial& materialFocus = GetRenderSystem().getMaterialMgr().getItem("ObjectFocus");
	materialFocus.setShader("Data\\fx\\ObjectFocus.fx");
	materialFocus.bAlphaTest			= false;
	materialFocus.bBlend				= false;
	materialFocus.bDepthTest			= true;
	materialFocus.bDepthWrite			= false;
	materialFocus.uCull				= CULL_NONE;
	materialFocus.bLightingEnabled	= false;
	// ----
	m_vScale=Vec3D(1.0f,1.0f,1.0f);
}
C3DMapObj::~C3DMapObj()
{
}

Matrix C3DMapObj::getShadowMatrix(const Vec3D& vLight,float fHeight)const
{
	Matrix mLight;
	float fDot=-sqrtf(vLight.x*vLight.x+vLight.z*vLight.z)/vLight.y;
	//MessageBoxW(NULL,f2ws(fDot,6,6).c_str(),L"",0);
	Vec3D vEye(getPos().x,fHeight,getPos().z);
	mLight.MatrixLookAtLH(vEye,vEye+vLight,Vec3D(0,1.0f,0));
	Matrix mInvertLight=mLight;
	mInvertLight.Invert();
	Matrix mTransLight(
		1,0,0,0,
		0,1,0,0,
		0,fDot,0,0,
		0,0,0,1);

	Matrix mTrans;
	Matrix mRotate;
	Matrix mScale;
	mTrans.translation(getPos());
	mRotate.rotate(getRotate());
	mScale.scale(getScale());

	return mInvertLight*mTransLight*mLight*mTrans*mRotate*mScale;
}


void C3DMapObj::renderShadow(const Matrix& mWorld, const Vec3D& vLight,float fHeight)const
{
	Matrix mNewWorld = mWorld*getShadowMatrix(vLight,fHeight);//m_mWorld;
	CModelObject::render(mNewWorld, E_MATERIAL_RENDER_TYPE(MATERIAL_GEOMETRY|MATERIAL_RENDER_ALPHA_TEST));
}

void C3DMapObj::renderFocus()const
{
	renderFocus(0xFFFFFFFF);
}

#include "Graphics.h"
void C3DMapObj::renderDebug()const
{
	GetGraphics().drawBBox(getBBox(),0xFFFF4400);
}

bool C3DMapObj::intersect(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax)
{
	if (getBBox().intersect(vRayPos , vRayDir, tmin, tmax))
	{
		Vec3D vNewRayPos = vRayPos;
		Vec3D vNewRayDir = vRayDir;
		transformRay(vNewRayPos,vNewRayDir,m_mWorldMatrix);

		if (CModelObject::getModelData()->m_Mesh.intersect(vNewRayPos , vNewRayDir))
		{
			return true;
		}
		else
		{
			for (LIST_RENDER_NODE::const_iterator it=m_mapChildObj.begin();it!=m_mapChildObj.end();it++)
			{
				//const CModelData* pModelData = it->second->getModelData();
				//if (pModelData&&pModelData->m_Mesh.intersect(vNewRayPos , vNewRayDir))
				{
				//	return true;
				}
			}
		}
	}
	return false;
}

int C3DMapObj::getOrder()
{
	if (CModelObject::getModelData())
	{
		return CModelObject::getModelData()->getOrder();
	}
	return CMapObj::getOrder();
}

void C3DMapObj::renderFocus(Color32 color)const
{
	if (GetRenderSystem().prepareMaterial("ObjectFocus"))
	{
		//CShader* pShaderFocus = GetRenderSystem().GetShaderMgr().getItem(m_MaterialFocus.uShader);
		//if (pShaderFocus)
		{
		//	pShaderFocus->setVec4D("g_vColorFocus",Vec4D(color));
			CModelObject::render(m_mWorldMatrix, E_MATERIAL_RENDER_TYPE(MATERIAL_GEOMETRY|MATERIAL_RENDER_ALPHA_TEST));
		}
		GetRenderSystem().finishMaterial();
	}
}