#include "UIEditBox.h"
#include "UIDialog.h"
#include "RenderSystem.h"
#include "tinyxml.h"

bool CUIEditBox::s_bHideCaret;   // If true, we don't render the caret.

// When scrolling, EDITBOX_SCROLLEXTENT is reciprocal of the amount to scroll.
// If EDITBOX_SCROLLEXTENT = 4, then we scroll 1/4 of the control each time.
#define EDITBOX_SCROLLEXTENT 4


CUIEditBox::CUIEditBox()
{
	m_Type = UI_CONTROL_EDITBOX;

	m_bCaretOn = true;
	m_dfBlink = GetCaretBlinkTime() * 0.001f;
	m_dfLastBlink = GetGlobalTimer().GetAbsoluteTime();
	s_bHideCaret = false;
	m_nFirstVisible = 0;
	m_nCaret = m_nSelStart = 0;
	m_bInsertMode = true;

	m_bEditEnabled=true;
}



CUIEditBox::~CUIEditBox()
{
}

void CUIEditBox::XMLParse(TiXmlElement* pControlElement)
{
	CUIControl::XMLParse(pControlElement);
}

void CUIEditBox::SetStyle(const std::string& strStyleName)
{
	m_Style.SetStyle(strStyleName);
	m_StyleSelection.SetStyle(strStyleName+".selection");
	m_StyleCaret.SetStyle(strStyleName+".caret");
}

// PlaceCaret: Set the caret to a character position, and adjust the scrolling if
//             necessary.

void CUIEditBox::PlaceCaret(int nCP)
{
	assert(nCP >= 0 && nCP <= m_Buffer.GetTextSize());
	m_nCaret = nCP;

	// Obtain the X offset of the character.
	int nX1st, nX, nX2;
	m_Buffer.CPtoX(m_nFirstVisible, FALSE, nX1st);  // 1st visible char
	m_Buffer.CPtoX(nCP, FALSE, nX);  // LEAD
	// If nCP is the NULL terminator, get the leading edge instead of trailing.
	if(nCP == m_Buffer.GetTextSize())
		nX2 = nX;
	else
		m_Buffer.CPtoX(nCP, TRUE, nX2);  // TRAIL

	// If the left edge of the char is smaller than the left edge of the 1st visible char,
	// we need to scroll left until this char is visible.
	if(nX < nX1st)
	{
		// Simply make the first visible character the char at the new caret position.
		m_nFirstVisible = nCP;
	}
	else
		// If the right of the character is bigger than the offset of the control's
		// right edge, we need to scroll right to this character.
		if(nX2 > nX1st + RectWidth(m_rcText))
		{
			// Compute the X of the new left-most pixel
			int nXNewLeft = nX2 - RectWidth(m_rcText);

			// Compute the char position of this character
			int nCPNew1st;
			bool bNewTrail;
			m_Buffer.XtoCP(nXNewLeft, nCPNew1st, bNewTrail);

			// If this coordinate is not on a character border,
			// start from the next character so that the caret
			// position does not fall outside the text rectangle.
			int nXNew1st;
			m_Buffer.CPtoX(nCPNew1st, FALSE, nXNew1st);
			if(nXNew1st < nXNewLeft)
				++nCPNew1st;

			m_nFirstVisible = nCPNew1st;
		}
}



void CUIEditBox::ClearText()
{
	m_Buffer.Clear();
	m_nFirstVisible = 0;
	PlaceCaret(0);
	m_nSelStart = 0;
}

void CUIEditBox::SetText(const std::wstring& wstrText)
{
	m_Buffer.SetText(wstrText);
	m_nFirstVisible = 0;
	// Move the caret to the end of the text
	PlaceCaret(m_Buffer.GetTextSize());
	m_nSelStart = m_nCaret;
}

void CUIEditBox::SetText(const std::wstring& wstrText, bool bSelected)
{
	m_Buffer.SetText(wstrText);
	m_nFirstVisible = 0;
	// Move the caret to the end of the text
	PlaceCaret(m_Buffer.GetTextSize());
	m_nSelStart = bSelected ? 0 : m_nCaret;
}

void CUIEditBox::SetSelected(int nSelStart, int nSelEnd)
{
	PlaceCaret(nSelEnd);
	m_nSelStart = nSelStart;
}

void CUIEditBox::SetInt(int val)
{
	SetText(i2ws(val));
}

void CUIEditBox::SetFloat(float val)
{
	//,const int slen,const int alen
	SetText(f2ws(val,0,2));
}

int	CUIEditBox::GetInt()
{
	std::wstring wstr = GetText();
	return _wtoi(wstr.c_str());
}

float CUIEditBox::GetFloat()
{
	std::wstring wstr = GetText();
	return (float)_wtof(wstr.c_str());
}

void CUIEditBox::DeleteSelectionText()
{
	int nFirst = __min(m_nCaret, m_nSelStart);
	int nLast = __max(m_nCaret, m_nSelStart);
	// Update caret and selection
	PlaceCaret(nFirst);
	m_nSelStart = m_nCaret;
	// Remove the characters
	m_Buffer.RemoveChar(nFirst,nLast-nFirst);
}

void CUIEditBox::UpdateRects()
{
	CUIControl::UpdateRects();
}


void CUIEditBox::CopyToClipboard()
{
	// Copy the selection text to the clipboard
	if(m_nCaret != m_nSelStart && OpenClipboard(NULL))
	{
		EmptyClipboard();

		HGLOBAL hBlock = GlobalAlloc(GMEM_MOVEABLE, sizeof(WCHAR) * (m_Buffer.GetTextSize() + 1));
		if(hBlock)
		{
			WCHAR *pwszText = (WCHAR*)GlobalLock(hBlock);
			if(pwszText)
			{
				int nFirst = __min(m_nCaret, m_nSelStart);
				int nLast = __max(m_nCaret, m_nSelStart);
				if(nLast - nFirst > 0)
					CopyMemory(pwszText, m_Buffer.GetBuffer() + nFirst, (nLast - nFirst) * sizeof(WCHAR));
				pwszText[nLast - nFirst] = L'\0';  // Terminate it
				GlobalUnlock(hBlock);
			}
			SetClipboardData(CF_UNICODETEXT, hBlock);
		}
		CloseClipboard();
		// We must not free the object until CloseClipboard is called.
		if(hBlock)
			GlobalFree(hBlock);
	}
}


void CUIEditBox::PasteFromClipboard()
{
	DeleteSelectionText();

	if(OpenClipboard(NULL))
	{
		HANDLE handle = GetClipboardData(CF_UNICODETEXT);
		if(handle)
		{
			// Convert the ANSI string to Unicode, then
			// insert to our buffer.
			WCHAR *pwszText = (WCHAR*)GlobalLock(handle);
			if(pwszText)
			{
				// Copy all characters up to null.
				if(m_Buffer.InsertString(m_nCaret, pwszText))
					PlaceCaret(m_nCaret + lstrlenW(pwszText));
				m_nSelStart = m_nCaret;
				GlobalUnlock(handle);
			}
		}
		CloseClipboard();
	}
}



bool CUIEditBox::HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(!m_bEnabled || !m_bVisible)
		return false;

	bool bHandled = false;

	switch(uMsg)
	{
	case WM_KEYDOWN:
		{
			switch(wParam)
			{
			case VK_TAB:
				// We don't process Tab in case keyboard input is enabled and the user
				// wishes to Tab to other controls.
				break;

			case VK_HOME:
				PlaceCaret(0);
				if(GetKeyState(VK_SHIFT) >= 0)
					// Shift is not down. Update selection
					// start along with the caret.
					m_nSelStart = m_nCaret;
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_END:
				PlaceCaret(m_Buffer.GetTextSize());
				if(GetKeyState(VK_SHIFT) >= 0)
					// Shift is not down. Update selection
					// start along with the caret.
					m_nSelStart = m_nCaret;
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_INSERT:
				if(GetKeyState(VK_CONTROL) < 0)
				{
					// Control Insert. Copy to clipboard
					CopyToClipboard();
				} else
					if(GetKeyState(VK_SHIFT) < 0)
					{
						// Shift Insert. Paste from clipboard
						PasteFromClipboard();
					} else
					{
						// Toggle caret insert mode
						m_bInsertMode = !m_bInsertMode;
					}
					break;

			case VK_DELETE:
				if (m_bEditEnabled)
				{
					// Check if there is a text selection.
					if(m_nCaret != m_nSelStart)
					{
						DeleteSelectionText();
						SendEvent(EVENT_EDITBOX_CHANGE, this);
					}
					else
					{
						// Deleting one character
						if(m_Buffer.RemoveChar(m_nCaret))
							SendEvent(EVENT_EDITBOX_CHANGE, this);
					}
					ResetCaretBlink();
					bHandled = true;
				}
				break;

			case VK_LEFT:
				if(GetKeyState(VK_CONTROL) < 0)
				{
					// Control is down. Move the caret to a new item
					// instead of a character.
					m_Buffer.GetPriorItemPos(m_nCaret, &m_nCaret);
					PlaceCaret(m_nCaret);
				}
				else
					if(m_nCaret > 0)
						PlaceCaret(m_nCaret - 1);
				if(GetKeyState(VK_SHIFT) >= 0)
					// Shift is not down. Update selection
					// start along with the caret.
					m_nSelStart = m_nCaret;
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_RIGHT:
				if(GetKeyState(VK_CONTROL) < 0)
				{
					// Control is down. Move the caret to a new item
					// instead of a character.
					m_Buffer.GetNextItemPos(m_nCaret, &m_nCaret);
					PlaceCaret(m_nCaret);
				}
				else
					if(m_nCaret < m_Buffer.GetTextSize())
						PlaceCaret(m_nCaret + 1);
				if(GetKeyState(VK_SHIFT) >= 0)
					// Shift is not down. Update selection
					// start along with the caret.
					m_nSelStart = m_nCaret;
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_UP:
			case VK_DOWN:
				// Trap up and down arrows so that the dialog
				// does not switch focus to another control.
				bHandled = true;
				break;

			default:
				bHandled = wParam != VK_ESCAPE;  // Let the application handle Esc.
			}
		}
	}
	return bHandled;
}

void CUIEditBox::OnMouseMove(POINT pt)
{
	if(IsPressed())
	{
		// Determine the character corresponding to the coordinates.
		int nCP, nX1st;
		bool bTrail;
		m_Buffer.CPtoX(m_nFirstVisible, FALSE, nX1st);  // X offset of the 1st visible char
		if(m_Buffer.XtoCP(pt.x - m_rcText.left + nX1st, nCP, bTrail))
		{
			// Cap at the NULL character.
			if(bTrail && nCP < m_Buffer.GetTextSize())
				PlaceCaret(nCP + 1);
			else
				PlaceCaret(nCP);
		}
	}
}

void CUIEditBox::OnLButtonDown(POINT point)
{
	SetFocus();
	SetPressed(true);
	SetCapture(UIGetHWND());
	// Determine the character corresponding to the coordinates.
	int nCP, nX1st;
	bool bTrail;
	m_Buffer.CPtoX(m_nFirstVisible, FALSE, nX1st);  // X offset of the 1st visible char
	if(m_Buffer.XtoCP(point.x - m_rcText.left + nX1st, nCP, bTrail))
	{
		// Cap at the NULL character.
		if(bTrail && nCP < m_Buffer.GetTextSize())
			PlaceCaret(nCP + 1);
		else
			PlaceCaret(nCP);
		m_nSelStart = m_nCaret;
		ResetCaretBlink();
	}
}

void CUIEditBox::OnLButtonUp(POINT point)
{
	if(IsPressed())
	{
		SetPressed(false);
		ReleaseCapture();
		// Button click
		//OnPressUp();
	}
}

void CUIEditBox::OnFocusIn()
{
	CUIControl::OnFocusIn();
	ResetCaretBlink();
}

void CUIEditBox::OnFocusOut()
{
	CUIControl::OnFocusOut();
	SendEvent(EVENT_DEFAULT, this);
}

bool CUIEditBox::MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(!m_bEnabled || !m_bVisible)
		return false;

	switch(uMsg)
	{
		// Make sure that while editing, the keyup and keydown messages associated with 
		// WM_CHAR messages don't go to any non-focused controls or cameras
	case WM_KEYUP:
	case WM_KEYDOWN:
		return true;

	case WM_CHAR:
		{
			switch((WCHAR)wParam)
			{
				// Backspace
			case VK_BACK:
				{
					if (m_bEditEnabled)
					{
						// If there's a selection, treat this
						// like a delete key.
						if(m_nCaret != m_nSelStart)
						{
							DeleteSelectionText();
							SendEvent(EVENT_EDITBOX_CHANGE, this);
						}
						else if(m_nCaret > 0)
						{
							// Move the caret, then delete the char.
							PlaceCaret(m_nCaret - 1);
							m_nSelStart = m_nCaret;
							m_Buffer.RemoveChar(m_nCaret);
							SendEvent(EVENT_EDITBOX_CHANGE, this);
						}
						ResetCaretBlink();
					}
					break;
				}

			case 24:        // Ctrl-X Cut
			case VK_CANCEL: // Ctrl-C Copy
				{
					CopyToClipboard();
					if (m_bEditEnabled)
					{
						// If the key is Ctrl-X, delete the selection too.
						if((WCHAR)wParam == 24)
						{
							DeleteSelectionText();
							SendEvent(EVENT_EDITBOX_CHANGE, this);
						}
					}
					break;
				}

				// Ctrl-V Paste
			case 22:
				{
					if (m_bEditEnabled)
					{
						PasteFromClipboard();
						SendEvent(EVENT_EDITBOX_CHANGE, this);
					}
					break;
				}

				// Ctrl-A Select All
			case 1:
				if(m_nSelStart == m_nCaret)
				{
					m_nSelStart = 0;
					PlaceCaret(m_Buffer.GetTextSize());
				}
				break;

			case VK_RETURN:
				// Invoke the callback when the user presses Enter.
				SendEvent(EVENT_EDITBOX_STRING, this);
				break;

				// Junk characters we don't want in the string
			case 26:  // Ctrl Z
			case 2:   // Ctrl B
			case 14:  // Ctrl N
			case 19:  // Ctrl S
			case 4:   // Ctrl D
			case 6:   // Ctrl F
			case 7:   // Ctrl G
			case 10:  // Ctrl J
			case 11:  // Ctrl K
			case 12:  // Ctrl L
			case 17:  // Ctrl Q
			case 23:  // Ctrl W
			case 5:   // Ctrl E
			case 18:  // Ctrl R
			case 20:  // Ctrl T
			case 25:  // Ctrl Y
			case 21:  // Ctrl U
			case 9:   // Ctrl I
			case 15:  // Ctrl O
			case 16:  // Ctrl P
			case 27:  // Ctrl [
			case 29:  // Ctrl ]
			case 28:  // Ctrl \ 
				break;

			default:
				if (m_bEditEnabled)
				{
					// If there's a selection and the user
					// starts to type, the selection should
					// be deleted.
					if(m_nCaret != m_nSelStart)
						DeleteSelectionText();

					// If we are in overwrite mode and there is already
					// a char at the caret's position, simply replace it.
					// Otherwise, we insert the char as normal.
					if(!m_bInsertMode && m_nCaret < m_Buffer.GetTextSize())
					{
						m_Buffer[m_nCaret] = (WCHAR)wParam;
						PlaceCaret(m_nCaret + 1);
						m_nSelStart = m_nCaret;
					} else
					{
						// Insert the char
						if(m_Buffer.InsertChar(m_nCaret, (WCHAR)wParam))
						{
							PlaceCaret(m_nCaret + 1);
							m_nSelStart = m_nCaret;
						}
					}
					ResetCaretBlink();
					SendEvent(EVENT_EDITBOX_CHANGE, this);
				}
			}
			return true;
		}
	}
	return false;
}



void CUIEditBox::OnFrameRender(double fTime, float fElapsedTime)
{
	if(m_bVisible == false)
		return;

	int nSelStartX = 0, nCaretX = 0;  // Left and right X cordinates of the selection region

	{
		PlaceCaret(m_nCaret);  // Call PlaceCaret now that we have the font info (node),
		// so that scrolling can be handled.
	}

	m_Style.draw(m_rcBoundingBox,m_Buffer.GetBuffer() + m_nFirstVisible,GetState(),fElapsedTime);
	m_rcText = m_Style.m_mapFont[0].rc.getRECT();

	// Compute the X coordinates of the first visible character.
	//
	int nXFirst;
	m_Buffer.CPtoX(m_nFirstVisible, FALSE, nXFirst);

	//
	// Compute the X coordinates of the selection rectangle
	//
	m_Buffer.CPtoX(m_nCaret, FALSE, nCaretX);
	if(m_nCaret != m_nSelStart)
		m_Buffer.CPtoX(m_nSelStart, FALSE, nSelStartX);
	else
		nSelStartX = nCaretX;

	//
	// Render the selection rectangle
	//
	RECT rcSelection;  // Make this available for rendering selected text
	if(m_nCaret != m_nSelStart)
	{
		int nSelLeftX = nCaretX, nSelRightX = nSelStartX;
		// Swap if left is bigger than right
		if(nSelLeftX > nSelRightX)
		{ int nTemp = nSelLeftX; nSelLeftX = nSelRightX; nSelRightX = nTemp; }

		SetRect(&rcSelection, nSelLeftX, m_rcText.top, nSelRightX, m_rcText.bottom);
		OffsetRect(&rcSelection, m_rcText.left - nXFirst, 0);
		IntersectRect(&rcSelection, &m_rcText, &rcSelection);

		int nFirstToRender = __max(m_nFirstVisible, __min(m_nSelStart, m_nCaret));
		int nNumChatToRender = __max(m_nSelStart, m_nCaret) - nFirstToRender;
		//m_Styles[0].m_mapFont = m_SelTextColor;
		///m_Style.Blend(CONTROL_STATE_FOCUS, fElapsedTime, 1);
		std::wstring wstrSelection = m_Buffer.GetBuffer() + nFirstToRender;
		wstrSelection = wstrSelection.substr(0,nNumChatToRender);
		m_StyleSelection.draw(rcSelection,wstrSelection,GetState(),fElapsedTime);
	}

	//
	// Blink the caret
	//
	if(GetGlobalTimer().GetAbsoluteTime() - m_dfLastBlink >= m_dfBlink)
	{
		m_bCaretOn = !m_bCaretOn;
		m_dfLastBlink = GetGlobalTimer().GetAbsoluteTime();
	}

	//
	// Render the caret if this control has the focus
	//
	if(IsFocus() && m_bCaretOn && !s_bHideCaret)
	{
		// Start the rectangle with insert mode caret
		RECT rcCaret = { m_rcText.left - nXFirst + nCaretX - 1, m_rcText.top,
			m_rcText.left - nXFirst + nCaretX + 1, m_rcText.bottom };

		// If we are in overwrite mode, adjust the caret rectangle
		// to fill the entire character.
		if(!m_bInsertMode)
		{
			// Obtain the right edge X coord of the current character
			int nRightEdgeX;
			m_Buffer.CPtoX(m_nCaret, TRUE, nRightEdgeX);
			rcCaret.right = m_rcText.left - nXFirst + nRightEdgeX;
		}
		m_StyleCaret.draw(rcCaret,L"",GetState(),fElapsedTime);
	}
}


//#define IN_FLOAT_CHARSET(c) \
//	((c) == L'-' || (c) == L'.' || ((c) >= L'0' && (c) <= L'9'))
//
//void CUIEditBox::ParseFloatArray(float *pNumbers, int nCount)
//{
//	int nWritten = 0;  // Number of floats written
//	const WCHAR *pToken, *pEnd;
//	WCHAR wszToken[60];
//
//	pToken = m_Buffer.GetBuffer();
//	while(nWritten < nCount && *pToken != L'\0')
//	{
//		// Skip leading spaces
//		while(*pToken == L' ')
//			++pToken;
//
//		if(*pToken == L'\0')
//			break;
//
//		// Locate the end of number
//		pEnd = pToken;
//		while(IN_FLOAT_CHARSET(*pEnd))
//			++pEnd;
//
//		// Copy the token to our buffer
//		int nTokenLen = __min(sizeof(wszToken) / sizeof(wszToken[0]) - 1, int(pEnd - pToken));
//		StringCchCopy(wszToken, nTokenLen, pToken);
//		*pNumbers = (float)wcstod(wszToken, NULL);
//		++nWritten;
//		++pNumbers;
//		pToken = pEnd;
//	}
//}


//void CUIEditBox::SetTextFloatArray(const float *pNumbers, int nCount)
//{
//	WCHAR wszBuffer[512] = {0};
//	WCHAR wszTmp[64];
//
//	if(pNumbers == NULL)
//		return;
//
//	for(int i = 0; i < nCount; ++i)
//	{
//		StringCchPrintf(wszTmp, 64, L"%.4f ", pNumbers[i]);
//		StringCchCat(wszBuffer, 512, wszTmp);
//	}
//
//	// Don't want the last space
//	if(nCount > 0 && wcslen(wszBuffer) > 0)
//		wszBuffer[wcslen(wszBuffer)-1] = 0;
//
//	SetText(wszBuffer);
//}


void CUIEditBox::ResetCaretBlink()
{
	m_bCaretOn = true;
	m_dfLastBlink = GetGlobalTimer().GetAbsoluteTime();
}