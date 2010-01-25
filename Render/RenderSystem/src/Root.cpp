#include "Root.h"

//#include "Log.h"
//#include "Sound.h"

//#include "Graphics.h"
#include "TextRender.h"
//#include "Shader.h"
//#include "VB.h"

//#include "SceneEffect.h"
//#include "ModelObject.h"
//#include "Particle.h"

static CRoot* g_pDisplay = NULL;

void SetRoot(CRoot* pDisPlay)
{
	g_pDisplay = pDisPlay;
}

CRoot& GetRoot()
{
	//assert(g_pRenderSystem);
	return *g_pDisplay;
}

CRoot::CRoot():
m_pRenderWindow(NULL),
m_pRenderSystem(NULL)
{
	// VBPool
	// 状态管理器
	// 纹理管理器
	// 基本图形处理管理器
	// shader管理器
	// 场景特效管理器
	// 物件管理器
	// 世界
	// UI
//	m_D3DObjectList.push_back(&GetVBPool());
//	m_D3DObjectList.push_back(&GetRenderSystem());
//	m_D3DObjectList.push_back(&GetTextRender());
//	m_D3DObjectList.push_back(&GetGraphics());
//	m_D3DObjectList.push_back(&GetShaderMgr());
//	m_D3DObjectList.push_back(&GetSceneEffect());
//	m_D3DObjectList.push_back(&GetModelMgr());
}

CRoot::~CRoot()
{
}

CRoot* g_pTheRoot = NULL;

LRESULT CALLBACK CRoot::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	{
		bool bNoFurtherProcessing = g_pTheRoot->MsgProc(hWnd, uMsg, wParam, lParam);

		LRESULT nResult = 0;
		if(bNoFurtherProcessing)
			return nResult;
	}
	if (g_pTheRoot->GetRenderWindow())
	{
		return g_pTheRoot->GetRenderWindow()->MsgProc(hWnd, uMsg, wParam, lParam);
	}
	return DefWindowProcW(hWnd, uMsg, wParam, lParam);;
}

bool CRoot::CreateRenderWindow(const std::wstring& wstrWindowTitle, int32 nWidth, int32 nHeight, bool bFullScreen)
{
	g_pTheRoot = this;
	m_pRenderWindow = m_pRenderSystem->CreateRenderWindow(CRoot::WndProc, wstrWindowTitle, nWidth, nHeight, bFullScreen);
	return m_pRenderWindow!=NULL;
}

void CRoot::OnFrameMove(double fTime, float fElapsedTime)
{
	// 图形要啥？
	//GetGraphics().Clear();
	//
	m_pRenderSystem->OnFrameMove();
	// 声音
	//GetSound().FrameMove(fElapsedTime);
	// 字
	GetTextRender().OnFrameMove();
	// 纹理更新 加载等操作
	m_pRenderSystem->GetTextureMgr().Update();	
}

void CRoot::OnFrameRender(double fTime, float fElapsedTime)
{
	//float fShaderTime = fTime;
	//GetShaderMgr().SetValue(PT_TIME,(void*)&fShaderTime);
	m_pRenderSystem->SetupRenderState();

	if(m_pRenderSystem->BeginFrame())
	{
		m_pRenderSystem->EndFrame();
	}
}

//void CRoot::KeyboardProc(UINT nChar, bool bKeyDown, bool bAltDown)
//{
//}
//
//
//void CRoot::MouseProc(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos)
//{
//}


// Rejects any devices that aren't acceptable by returning false

//bool CRoot::IsDeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
//								 D3DFORMAT BackBufferFormat, bool bWindowed)
//{
//	// Typically want to skip backbuffer formats that don't support alpha blending
//	IDirect3D9* pD3D = DXUTGetD3DObject(); 
//	if(FAILED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,
//		AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
//		D3DRTYPE_TEXTURE, BackBufferFormat)))
//		return false;
//
//	return true;
//}
//
//
//
//// Before a device is created, modify the device settings as needed
//
//bool CRoot::ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps)
//{
//	return true;
//}
void CRoot::Run()
{
	while (MainUpdate())
	{
	}
}

bool CRoot::MainUpdate()
{
	MSG  msg;
	while( PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) )
	{

		if (WM_QUIT == msg.message)
		{
			return false;
		}
		else
		// Translate and dispatch the message
		//if(hAccel == NULL || hWnd == NULL || 
		//	0 == TranslateAccelerator(hWnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	double fTime = m_pRenderWindow->GetTime();
	float fElapsedTime = m_pRenderWindow->GetElapsedTime();
	OnFrameMove(fTime, fElapsedTime);

	if (m_pRenderWindow->FrameBegin())
	{
		OnFrameRender(fTime, fElapsedTime);
		m_pRenderWindow->FrameEnd();
	}
	return true;
}