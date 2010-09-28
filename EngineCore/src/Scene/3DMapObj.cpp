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

Matrix C3DMapObj::getShadowMatrix()const
{
	Matrix mLight;
	//mLight.MatrixLookAtLH(Vec3D(0,0,0),Vec3D(-1.0f,-1.0f,-1.0f),Vec3D(0,1,0)));
	Matrix mInvertLight=mLight;
	mInvertLight.Invert();
	Matrix mTransLight(
		1,-1,0,0,
		0,0,0,0,
		0,0,1,0,
		0,0,0,1);

	Matrix mTrans;
	Matrix mRotate;
	Matrix mScale;
	mTrans.translation(getPos());
	mRotate.rotate(getRotate());
	mScale.scale(getScale());

	return mTrans*mTransLight*mRotate*mScale;
}

BBox C3DMapObj::getBBox()const
{
	BBox bbox=CModelComplex::getBBox();
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
	if (!CModelComplex::isCreated())
	{
		CModelComplex::create();
		//m_ObjectTree.delObject((*it));
		//m_ObjectTree.addObject((*it));
	}
	CModelComplex::OnFrameMove(fElapsedTime);
	CModelComplex::updateEmitters(getWorldMatrix(),fElapsedTime);
}

void C3DMapObj::render(int flag)const
{
	GetRenderSystem().setWorldMatrix(getWorldMatrix());
	CModelComplex::render((E_MATERIAL_RENDER_TYPE)flag,(E_MATERIAL_RENDER_TYPE)flag);
}

void C3DMapObj::renderShadow()const
{
	GetRenderSystem().setWorldMatrix(getShadowMatrix());
	CModelComplex::drawMeshWithTexture(MATERIAL_RENDER_GEOMETRY);
	//drawMesh(MATERIAL_RENDER_GEOMETRY);
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

void C3DMapObj::drawWithoutMaterial()const
{
	CModelComplex::drawMesh(MATERIAL_RENDER_NORMAL);
}

bool C3DMapObj::intersect(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax)
{
	if (getBBox().intersect(vRayPos , vRayDir, tmin, tmax))
	{
		Matrix mWorld = getWorldMatrix();
		Vec3D vNewRayPos = vRayPos;
		Vec3D vNewRayDir = vRayDir;
		transformRay(vNewRayPos,vNewRayDir,mWorld);

		if (CModelComplex::getModelData()->m_Mesh.intersect(vNewRayPos , vNewRayDir))
		{
			return true;
		}
		else
		{
			for (std::map<std::string,CModelObject*>::const_iterator it=m_mapSkinModel.begin();it!=m_mapSkinModel.end();it++)
			{
				const CModelData* pModelData = it->second->getModelData();
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
	if (CModelComplex::getModelData())
	{
		return CModelComplex::getModelData()->GetOrder();
	}
	return CMapObj::getOrder();
}

bool C3DMapObj::isSkinMesh()
{
	if (CModelComplex::getModelData())
	{
		return CModelComplex::getModelData()->m_Mesh.m_bSkinMesh;
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
		m_pShaderFocus->setMatrix("g_mWorld",mWorld);
		R.SetShader(m_pShaderFocus);
		drawWithoutMaterial();
		R.SetShader((CShader*)NULL);
	}
}