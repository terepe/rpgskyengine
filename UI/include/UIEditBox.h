#pragma once
#include "UIControl.h"
#include "UniBuffer.h"

class DLL_EXPORT CUIEditBox : public CUIControl
{
public:
	CUIEditBox();
	virtual ~CUIEditBox();

	virtual void XMLParse(TiXmlElement* pControlElement);
	virtual void SetStyle(const std::string& strStyleName);
	virtual bool HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//
	virtual void OnMouseMove(POINT pt);
	virtual void OnLButtonDown(POINT point);
	virtual void OnLButtonUp(POINT point);
	//
	virtual bool MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void UpdateRects();
	virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
	virtual void OnFrameRender(double fTime, float fElapsedTime);
	virtual void OnFocusIn();
	virtual void OnFocusOut();

	void SetInt(int val);
	void SetFloat(float val);
	virtual void SetText(const std::wstring& wstrText);
	void SetText(const std::wstring& wstrText, bool bSelected);
	void SetSelected(int nSelStart, int nSelEnd);
	std::wstring GetText() { return m_Buffer.GetBuffer(); }
	int	GetInt();
	float GetFloat();
	size_t GetTextLength() { return m_Buffer.GetTextSize(); }  // Returns text length in chars excluding NULL.
	int GetFirstVisible() { return m_nFirstVisible; }
	void ClearText();
	//void ParseFloatArray(float *pNumbers, int nCount);
	//void SetTextFloatArray(const float *pNumbers, int nCount);

	virtual void setEditEnabled(bool bEnabled) { m_bEditEnabled = bEnabled; }
	virtual bool getEditEnabled() { return m_bEditEnabled; }
protected:
	void PlaceCaret(int nCP);
	void DeleteSelectionText();
	void ResetCaretBlink();
	void CopyToClipboard();
	void PasteFromClipboard();

	CUIStyle m_StyleSelection;
	CUIStyle m_StyleCaret;

	CUniBuffer m_Buffer;     // Buffer to hold text
	double   m_dfBlink;      // Caret blink time in milliseconds
	double   m_dfLastBlink;  // Last timestamp of caret blink
	bool     m_bCaretOn;     // Flag to indicate whether caret is currently visible
	int      m_nCaret;       // Caret position, in characters
	bool     m_bInsertMode;  // If true, control is in insert mode. Else, overwrite mode.
	int      m_nSelStart;    // Starting position of the selection. The caret marks the end.
	int      m_nFirstVisible;// First visible character in the edit control

	// Static
	static bool s_bHideCaret;   // If true, we don't render the caret.

	bool	m_bEditEnabled;
	RECT	m_rcText;			// Rect of text.
};