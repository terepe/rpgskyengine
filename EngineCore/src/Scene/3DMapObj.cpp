#include "3DMapObj.h"
#include "Intersect.h"
#include "RenderSystem.h"

C3DMapObj::C3DMapObj()
{
	m_vScale=Vec3D(1.0f,1.0f,1.0f);
}
C3DMapObj::~C3DMapObj()
{
}

void C3DMapObj::GetCellPos( Pos2D& posCell )
{
	posCell = m_posCell;
}

void C3DMapObj::SetCellPos( Pos2D& posCell )
{
	m_posCell = posCell;
}

Matrix C3DMapObj::getWorldMatrix()const
{
	Matrix mTrans;
	Matrix mRotate;
	Matrix mScale;
	mTrans.translation(getPos());
	mRotate.rotate(getRotate());
	mScale.scale(getScale());
	return mTrans*mRotate*mScale;
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

BBox C3DMapObj::getBBox()const
{
	BBox bbox=CModelObject::getBBox();
	Matrix mRotate;
	mRotate.rotate(getRotate());

	Vec3D vHalfExtents = (bbox.vMax-bbox.vMin)*0.5f*getScale();
	Vec3D vCenter = (bbox.vMax+bbox.vMin)*0.5f*getScale();
	vCenter=mRotate*vCenter+getPos();

	Vec3D vExtent;
	vExtent.x = Vec3D(abs(mRotate._11),abs(mRotate._12),abs(mRotate._13)).dot(vHalfExtents);
	vExtent.y = Vec3D(abs(mRotate._21),abs(mRotate._22),abs(mRotate._23)).dot(vHalfExtents);
	vExtent.z = Vec3D(abs(mRotate._31),abs(mRotate._32),abs(mRotate._33)).dot(vHalfExtents);

	bbox.vMin = vCenter - vExtent;
	bbox.vMax = vCenter + vExtent;
	return bbox;
}

void C3DMapObj::OnFrameMove(float fElapsedTime)
{
	if (!CModelObject::isCreated())
	{
		CModelObject::create();
		//m_ObjectTree.delObject((*it));
		//m_ObjectTree.addObject((*it));
	}
	CModelObject::OnFrameMove(fElapsedTime);
	CModelObject::updateEmitters(getWorldMatrix(),fElapsedTime);
}

void C3DMapObj::render(int flag)const
{
	GetRenderSystem().setWorldMatrix(getWorldMatrix());
	CModelObject::render((E_MATERIAL_RENDER_TYPE)flag,(E_MATERIAL_RENDER_TYPE)flag);
}

void C3DMapObj::renderShadow(const Vec3D& vLight,float fHeight)const
{
	GetRenderSystem().setWorldMatrix(getShadowMatrix(vLight,fHeight));
	CModelObject::renderMesh(E_MATERIAL_RENDER_TYPE(MATERIAL_GEOMETRY|MATERIAL_RENDER_ALPHA_TEST));
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
		Matrix mWorld = getWorldMatrix();
		Vec3D vNewRayPos = vRayPos;
		Vec3D vNewRayDir = vRayDir;
		transformRay(vNewRayPos,vNewRayDir,mWorld);

		if (CModelObject::getModelData()->m_Mesh.intersect(vNewRayPos , vNewRayDir))
		{
			return true;
		}
		else
		{
			for (std::map<std::string,ChildRenderObj>::const_iterator it=m_mapChildObj.begin();it!=m_mapChildObj.end();it++)
			{
				const CModelData* pModelData = it->second.pChildObj->getModelData();
				if (pModelData&&pModelData->m_Mesh.intersect(vNewRayPos , vNewRayDir))
				{
					return true;
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
		return CModelObject::getModelData()->GetOrder();
	}
	return CMapObj::getOrder();
}

bool C3DMapObj::isSkinMesh()
{
	if (CModelObject::getModelData())
	{
		return CModelObject::getModelData()->m_Mesh.m_bSkinMesh;
	}
	return false;
}

void C3DMapObj::renderFocus(Color32 color)const
{
	CRenderSystem& R = GetRenderSystem();
	R.SetDepthBufferFunc(true,false);
	R.SetAlphaTestFunc(false);
	R.SetBlendFunc(false);
	R.SetCullingMode(CULL_NONE);
	static CShader* m_pShaderFocus=NULL;
	if (m_pShaderFocus==NULL)
	{
		unsigned long uShaderID = R.GetShaderMgr().registerItem("Data\\fx\\ObjectFocus.fx");
		m_pShaderFocus = R.GetShaderMgr().getItem(uShaderID);
	}
	if (m_pShaderFocus)
	{
		m_pShaderFocus->setVec4D("g_vColorFocus",Vec4D(color));
		Matrix mWorld = getWorldMatrix();
		R.setWorldMatrix(mWorld);
		R.SetShader(m_pShaderFocus);
		CModelObject::renderMesh(E_MATERIAL_RENDER_TYPE(MATERIAL_GEOMETRY|MATERIAL_RENDER_ALPHA_TEST));
		R.SetShader((CShader*)NULL);
	}
}