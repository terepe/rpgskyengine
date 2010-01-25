#pragma once

// Mesh 顶点索引缓冲数据,不含有纹理材质等 利于重复利用
#include "GameStdafx.h"
#include "Particle.h"


typedef struct ATTRIBUTERANGE
{
	char  szName[32];
	DWORD AttribId;
	DWORD IndexStart;
	DWORD IndexCount;
	DWORD VertexStart;
	DWORD VertexCount;
	DWORD dwFlags;
};
typedef ATTRIBUTERANGE* LPATTRIBUTERANGE;

class CParticlesMgr
{
public:
	CParticlesMgr(void);
	~CParticlesMgr(void);

	// 创建设备
	HRESULT OnCreateDevice( LPDIRECT3DDEVICE9 pd3dDevice ){ m_pDevice = pd3dDevice; }
	// 重置D3D设备
	void OnResetDevice();
	void OnLostDevice();
	void OnDestroyDevice();

	//
	void Render();
	// 实时检测
	void Update();
public:
	//
	LPDIRECT3DVERTEXBUFFER9 GetVB(){ return m_pVB; }
	//
	LPDIRECT3DINDEXBUFFER9 GetIB(){ return m_pIB; }
public:
	//
	void SetVB( LPDIRECT3DVERTEXBUFFER9 pVB ){ m_pVB = pVB; }
	//
	void SetIB( LPDIRECT3DINDEXBUFFER9 pIB ){ m_pIB = pIB; }
public:
	vector<ATTRIBUTERANGE>		m_Attribs;
protected:
	LPDIRECT3DDEVICE9			m_pDevice;
	LPDIRECT3DVERTEXBUFFER9		m_pVB;			// 顶点缓冲
	LPDIRECT3DINDEXBUFFER9		m_pIB;			// 索引缓冲
};

CParticlesMgr* GetParticlesMgr();