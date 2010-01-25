#pragma once
#include "UIDialog.h"

class DLL_EXPORT CUIMainDialog : public CUIDialog
{
public:
	CUIMainDialog();
	~CUIMainDialog();
	virtual	bool OnInitDialog();
	virtual void OnFrameRender(double fTime, float fElapsedTime);
	virtual bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
