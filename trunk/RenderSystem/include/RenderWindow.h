#pragma once
#include "Common.h"
#include <windows.h>

class DLL_EXPORT CRenderWindow
{
public:
	CRenderWindow();
	~CRenderWindow();
public:
	virtual bool Create(WNDPROC pWndProc, const std::wstring& strWindowTitle, int32 nWidth, int32 nHeight, bool bFullScreen = false) = 0;
	virtual void SetFullscreen(bool bFullScreen, uint32 width, uint32 height) {}
	//virtual void Resize(uint32 width, uint32 height) = 0;
	virtual double GetTime()= 0;
	virtual float GetElapsedTime()= 0;
	virtual bool FrameBegin()= 0;
	virtual void FrameEnd()= 0;
	virtual HWND GetHWND()= 0;
	virtual LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)= 0;
private:
};