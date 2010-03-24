#pragma once
#include "Shader.h"
#include "RenderSystemCommon.h"
#include "TextureMgr.h"
#include "MaterialMgr.h"
#include "RenderWindow.h"
#include "HardwareVertexBuffer.h"
#include "HardwareIndexBuffer.h"
#include "HardwareBufferMgr.h"
#include "ShaderMgr.h"
#include "VertexDeclaration.h"
#include "Pos2D.h"


#if defined(_DEBUG)
#pragma comment(lib, "commond.lib")
#pragma comment(lib, "mathd.lib")
#pragma comment(lib, "freetyped.lib")
#else
#pragma comment(lib, "common.lib")
#pragma comment(lib, "math.lib")
#pragma comment(lib, "freetype.lib")
#endif


class DLL_EXPORT CRenderSystem
{
public:
	CRenderSystem();
	virtual ~CRenderSystem();
public:
	virtual CTextureMgr& GetTextureMgr() = 0;
	virtual CMaterialMgr& getMaterialMgr();
	virtual CHardwareBufferMgr& GetHardwareBufferMgr() = 0;
	virtual CShaderMgr& GetShaderMgr() = 0;
	virtual CRenderWindow* CreateRenderWindow(WNDPROC pWndProc, const std::wstring& strWindowTitle, int32 nWidth, int32 nHeight, bool bFullScreen = false) = 0;
public:
	virtual void OnFrameMove() = 0;
	//
	virtual CTexture* GetRenderTarget() = 0;
	virtual void SetRenderTarget(CTexture* pRenderTarget) = 0;
	//
	virtual CTexture* GetDepthStencil() = 0;
	virtual void SetDepthStencil(CTexture* pDepthStencil) = 0;

	
	virtual bool BeginFrame() = 0;	// 帧渲染开始
	virtual void EndFrame() = 0;	// 帧渲染结束

	virtual void setViewport(const CRect<int>& rect) = 0;
	virtual void getViewport(CRect<int>& rect) = 0;
	// 清除缓存
	virtual void ClearBuffer(bool bZBuffer, bool bTarget, Color32 color) = 0;

	// 填充模式
	virtual void SetFillMode(FillMode mode) = 0;

	// set matrix
	virtual void setWorldMatrix(const Matrix& m) = 0;
	virtual void setViewMatrix(const Matrix& m) = 0;
	virtual void setProjectionMatrix(const Matrix& m) = 0;
	virtual void setTextureMatrix(uint8 uTexChannel, TextureTransformFlag flag, const Matrix& m = Matrix::ZERO) = 0;
	// get matrix
	virtual void getWorldMatrix(Matrix& m)const = 0;
	virtual void getViewMatrix(Matrix& m)const = 0;
	virtual void getProjectionMatrix(Matrix& m)const = 0;
	virtual void getTextureMatrix(uint8 uTexChannel, Matrix& m)const = 0;

	// Func
	virtual void SetDepthBufferFunc(bool bDepthTest = true, bool bDepthWrite = true,				// 深度检测
		CompareFunction depthFunction = CMPF_LESS_EQUAL ) = 0;
	virtual void SetAlphaTestFunc(bool bAlphaTest = true, CompareFunction func = CMPF_GREATER_EQUAL,// ALPHA检测
		unsigned char value = 0x80) = 0;
	virtual void SetBlendFunc(bool bBlend = true, SceneBlendOperation op = BLENDOP_ADD,				// 混合参数
		SceneBlendFactor src = SBF_SOURCE_ALPHA, SceneBlendFactor dest = SBF_ONE_MINUS_SOURCE_ALPHA) = 0;

	//
	virtual void SetCullingMode(CullingMode mode) = 0;	// 设置剔除模式
	virtual void SetTextureFactor(Color32 color) = 0;	// 设置纹理因素颜色
	// TextureOP
	virtual void setResultARGToTemp(size_t unit, bool bResultARGToTemp=true) = 0;
	virtual void SetTextureColorOP(size_t unit, TextureBlendOperation op = TBOP_MODULATE,
		TextureBlendSource src1 = TBS_TEXTURE, TextureBlendSource src2 = TBS_DIFFUSE) = 0;
	virtual void SetTextureAlphaOP(size_t unit, TextureBlendOperation op,
		TextureBlendSource src1 = TBS_TEXTURE, TextureBlendSource src2 = TBS_DIFFUSE) = 0;
	//
	virtual void SetSamplerFilter(size_t unit, TextureFilterType MagFilter, TextureFilterType MinFilter, TextureFilterType MipFilter) = 0;
	virtual void SetSamplerAddressUV(size_t unit, AddressUV addressU, AddressUV addressV) = 0;

	// 状态初始化
	virtual void SetupRenderState() = 0;
	// 去色
	virtual void SetTextureStageStateDecolor() = 0;
	// 设置shader
	virtual void SetShader(CShader* pShader) = 0;
	virtual void SetShader(uint32 id) = 0;
	// Light
	virtual void SetDirectionalLight(uint32 uIndex,const DirectionalLight& light) = 0;
	virtual void LightEnable(uint32 Index, bool bEnable) = 0;
	virtual void SetLightingEnabled(bool bEnable) = 0;
	virtual void SetTexCoordIndex(uint32 stage, uint32 index) = 0;

	// 设置纹理
	virtual void SetTexture(uint32 Stage, uint32 TextureID) = 0;
	virtual void SetTexture(uint32 Stage, const CTexture* pTexture) = 0;
	// Get
	virtual CTexture* GetTexture(uint32 Stage) = 0;

	virtual CVertexDeclaration* CreateVertexDeclaration() = 0;
	//
	virtual void SetMaterial(const Vec4D& vAmbient, const Vec4D& vDiffuse) = 0;
	// 顶点
	virtual void SetVB(int nVBID) = 0;
	// 设置FVF顶点格式
	virtual void SetFVF(uint32 FVF) = 0;
	//
	virtual void SetVertexDeclaration(CVertexDeclaration* pDecl) = 0;
	//
	virtual void SetStreamSource(uint32 StreamNumber, CHardwareVertexBuffer* pStreamData,uint32 OffsetInBytes,uint32 Stride) = 0;
	virtual void SetIndices(CHardwareIndexBuffer* pIndexData) = 0;

	// 绘制
	virtual void DrawPrimitive(VertexRenderOperationType PrimitiveType,uint32 StartVertex,uint32 PrimitiveCount) = 0;
	virtual void DrawIndexedPrimitive(VertexRenderOperationType PrimitiveType,int32 BaseVertexIndex,uint32 MinVertexIndex,uint32 NumVertices,uint32 startIndex,uint32 primCount) = 0;
	virtual void DrawPrimitiveUP(VertexRenderOperationType PrimitiveType,uint32 PrimitiveCount,const void* pVertexStreamZeroData,uint32 VertexStreamZeroStride) = 0;
	virtual void DrawIndexedPrimitiveUP(VertexRenderOperationType PrimitiveType,uint32 MinVertexIndex,uint32 NumVertices,uint32 PrimitiveCount,const void* pIndexData,const void* pVertexStreamZeroData,uint32 VertexStreamZeroStride) = 0;

	virtual void drawIndexedSubset(const IndexedSubset& subset) = 0;

	virtual void setFog(const Fog& fog) = 0;
	virtual void setFogEnable(bool bEnable) = 0;

	virtual void StretchRect(CTexture* pSource,const RECT* pSourceRect,CTexture* pDest,const RECT* pDestRect,TextureFilterType filter) = 0;
	//
	void world2Screen(const Vec3D& vWorldPos, Pos2D& posScreen);
	void GetPickRay(Vec3D& vRayPos, Vec3D& vRayDir,int x, int y);

	// set material
	bool prepareMaterial(const std::string& strMaterialName, float fOpacity=1.0f);
	bool prepareMaterial(CMaterial& material, float fOpacity=1.0f);
	void finishMaterial();
private:
	CMaterialMgr m_MaterialMgr;
};

DLL_EXPORT void SetRenderSystem(CRenderSystem* pRenderSystem);
DLL_EXPORT CRenderSystem& GetRenderSystem();