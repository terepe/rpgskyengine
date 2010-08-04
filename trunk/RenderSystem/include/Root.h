#pragma once
#include "RenderWindow.h"
#include "RenderSystem.h"

class DLL_EXPORT CRoot
{
public:
	CRoot();
	~CRoot();
public:
	// 创建设备
	// 当direct3d设备被复位后，这个函数立即被调用，这里最好放置D3DPOOL_DEFAULT 资源代码，因为这
	// 这些资源在设备丢失后需要重新装载。在这里创建的资源应该在OnLostDevice 函数中释放

	// 丢失设备
	// 在Direct3D设备进入lost状态后在IDirect3DDevice9::Reset 调用之前调用此函数，在OnResetDevice 
	// 中创建的资源必须在这里释放，通常包括所有的D3DPOOL_DEFAULT 资源，


	// 销毁设备
	//此回调函数在direct3d设备被销毁时调用，通常发生在程序终止，在OnCreateDevice 中创建的资源，要
	//在这里释放，通常包含所有的D3DPOOL_MANAGED资源
	//IDirect3DDevice9::Present 来显示翻转链中下一个缓冲区内容。

	// 初始化
	bool CreateRenderWindow(const std::wstring& wstrWindowTitle, int32 nWidth, int32 nHeight, bool bFullScreen = false);
	//
	virtual void	OnFrameMove(double fTime, float fElapsedTime);

	//此回调函数在每Frame最后被调用，在场景上执行所有的渲染调用，当窗口需要重绘（处理WM_PAINT消
	//息）时此函数也会被调用（此时不调用OnFrameMove），在此函数返回后，DXUT将调用
	//IDirect3DDevice9::Present 来显示翻转链中下一个缓冲区内容。
	virtual void	OnFrameRender(double fTime, float fElapsedTime);
	// 处理消息
	// Handle messages to the application 
	virtual bool	MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)= 0;
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	//virtual void	KeyboardProc(UINT nChar, bool bKeyDown, bool bAltDown);
	//virtual void	MouseProc(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
	//	bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos);
	virtual void Run();
	bool MainUpdate();
	//bool			ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps);
	CRenderWindow* GetRenderWindow(){return m_pRenderWindow;}
	CRenderSystem* GetRenderSystem(){return m_pRenderSystem;}
protected:
	CRenderWindow* m_pRenderWindow;
	CRenderSystem* m_pRenderSystem;
};
DLL_EXPORT void SetRoot(CRoot* pDisPlay);
DLL_EXPORT CRoot& GetRoot();

