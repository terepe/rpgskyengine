#pragma once
#include "UICombo.h"

class DLL_EXPORT CUIDialog : public CUICombo
{
public:
	CUIDialog();
	~CUIDialog();
public:
	virtual bool Create(CUICombo* pParent=NULL);
	virtual bool Create(const std::string& strID, CUICombo* pParent=NULL);

	virtual void XMLParse(TiXmlElement* pControlElement);
	virtual void SetStyle(const std::string& strStyleName);
	virtual void OnChildSize(const RECT& rc);
	virtual void OnSize(const RECT& rc);
	virtual void UpdateRects();
	virtual bool ContainsPoint(POINT pt);
	// my message
	virtual bool postMsg(const std::string& strMsg);
	virtual bool postMsg(uint32 uEvent);
	// Windows消息处理
	virtual bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	CUIDialog* GetChildDialogAtPoint(POINT pt);

	virtual void SetVisible(bool bVisible);
	virtual void SetFocus(bool bFocus=true);
	virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }

	virtual void ClientToScreen(RECT& rc);
	virtual void ScreenToClient(RECT& rc);

	// Attributes

	bool GetMinimized() { return m_bMinimized; }
	void SetMinimized(bool bMinimized) { m_bMinimized = bMinimized; }
	void EnableCaption(bool bEnable) { m_bCaption = bEnable; }
	int GetCaptionHeight() const { return m_nCaptionHeight; }
	void SetCaptionHeight(int nHeight) { m_nCaptionHeight = nHeight; }
	void SetCaptionText(const std::wstring& wstrText) { m_wstrCaption = wstrText; }

	void SetCanMove(bool bCanMove) { m_bCanMove = bCanMove; }
	bool CanMove() { return m_bCanMove; }
	void SetCaptionEnable(bool bCaption) { m_bCaption = bCaption; }
	bool GetCaptionEnable() { return m_bCaption; }

	// Device state notification
	virtual void OnFrameMove(double fTime, float fElapsedTime);
	virtual void OnFrameRender(double fTime, float fElapsedTime);

	// 鼠标响应
	virtual void OnMouseMove(POINT pt);
	virtual void OnMouseWheel(POINT point,short wheelDelta);
	virtual void OnLButtonDblClk(POINT point);
	virtual void OnLButtonDown(POINT point);
	virtual void OnLButtonUp(POINT point);
	virtual void OnRButtonDblClk(POINT point);
	virtual void OnRButtonDown(POINT point);
	virtual void OnRButtonUp(POINT point);
	virtual void OnMButtonDblClk(POINT point);
	virtual void OnMButtonDown(POINT point);
	virtual void OnMButtonUp(POINT point);


	bool RegisterDialog(CUIDialog *pDialog);
	bool UnregisterDialog(CUIDialog *pDialog);
	bool setTopDialog(const CUIDialog *pDialog);
	bool toTop();
protected:
	CUIStyle m_StyleCaption;

	virtual void OnMove(int x, int y);
	bool InitDialog();
	virtual	bool OnInitDialog();

	// Control events
	bool OnCycleFocus(bool bForward);

	bool m_bCaption;
	bool m_bCanMove;
	bool m_bMinimized;
	std::wstring m_wstrCaption;

	RECT	m_rcCaption;

	int m_nCaptionHeight;
	int m_nMouseOriginX;
	int m_nMouseOriginY;

	std::vector<CUIDialog*>		m_Dialogs;            // Dialogs registered
};