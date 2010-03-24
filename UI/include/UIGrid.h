#pragma once

#include "UIControl.h"
#include "UIScrollBar.h"
#include "UIEditBox.h"
#include "UIComboBox.h"
#include "UISlider.h"
#include "UIDialog.h"

typedef enum EGridItemType
{
	ITEM_TYPE_UNDEF   = 0,
#ifdef __cplusplus
	ITEM_TYPE_BOOLCPP = 1,
#endif // __cplusplus
	ITEM_TYPE_BOOL8   = 2,
	ITEM_TYPE_BOOL16,
	ITEM_TYPE_BOOL32,
	ITEM_TYPE_CHAR,
	ITEM_TYPE_INT8,
	ITEM_TYPE_UINT8,
	ITEM_TYPE_INT16,
	ITEM_TYPE_UINT16,
	ITEM_TYPE_INT32,
	ITEM_TYPE_UINT32,
	ITEM_TYPE_FLOAT,
	ITEM_TYPE_DOUBLE,
	ITEM_TYPE_COLOR32,        // 32 bits color. Order is RGBA if API is OpenGL or Direct3D10, and inversed if API is Direct3D9 (can be modified by defining 'colorOrder=...', see doc)
	ITEM_TYPE_COLOR3F,        // 3 floats color. Order is RGB.
	ITEM_TYPE_COLOR4F,        // 4 floats color. Order is RGBA.
	ITEM_TYPE_CDSTRING,		// Null-terminated C Dynamic String (pointer to an array of char dynamically allocated with malloc/realloc/strdup)
#ifdef __cplusplus
	ITEM_TYPE_STDSTRING,		// C++ STL string (std::string)
#endif // __cplusplus
};
//-----------------------------------------------------------------------------
// ListBox control
//-----------------------------------------------------------------------------
struct UIGridItem
{
	std::wstring m_wstrName;
	WCHAR strText[256];
	void* m_pData;

	RECT  rcActive;
	bool  bSelected;



	EGridItemType	m_Type;
	void *					m_Ptr;
	//TwSetVarCallback		m_SetCallback;
	//TwGetVarCallback		m_GetCallback;
	void *					m_ClientData;
	bool					m_ReadOnly;
	bool					m_NoSlider;
	template <typename _T>	struct TVal
	{
		_T					m_Val;
		_T					m_Min;
		_T					m_Max;
		_T					m_Step;
		signed char			m_Precision;
		bool				m_Hexa;
	};
	union UVal
	{
		TVal<unsigned char>	m_Char;
		TVal<int8>			m_Int8;
		TVal<uint8>			m_UInt8;
		TVal<int16>			m_Int16;
		TVal<uint16>		m_UInt16;
		TVal<int32>			m_Int32;
		TVal<uint32>		m_UInt32;
		TVal<float>			m_Float32;
		TVal<double>		m_Float64;
		struct CBoolVal
		{
			char *			m_TrueString;
			char *			m_FalseString;
			bool			m_FreeTrueString;
			bool			m_FreeFalseString;
		}					m_Bool;
		struct CEnumVal		// empty -> enum entries are deduced from m_Type
		{
			//typedef std::map<unsigned int, std::string> CEntries;
			//CEntries *	m_Entries;
		}					m_Enum;
		struct CShortcutVal
		{
			int				m_Incr[2];
			int				m_Decr[2];
		}					m_Shortcut;
		struct CHelpStruct
		{
			int				m_StructType;
		}					m_HelpStruct;
		struct CButtonVal
		{
		//	TwButtonCallback m_Callback;
			int				m_Separator;
		}					m_Button;
	};
	UVal					m_Val;
	std::wstring GetValString()
	{
		wchar_t Tmp[1024]={0};
		switch(m_Type)
		{
		case ITEM_TYPE_UNDEF:
			break;
		case ITEM_TYPE_BOOLCPP:
			break;
		case ITEM_TYPE_BOOL8:
			break;
		case ITEM_TYPE_BOOL16:
			break;
		case ITEM_TYPE_BOOL32:
			break;
		case ITEM_TYPE_CHAR:
			break;
		case ITEM_TYPE_INT8:
			break;
		case ITEM_TYPE_UINT8:
			break;
		case ITEM_TYPE_INT16:
			break;
		case ITEM_TYPE_UINT16:
			break;
		case ITEM_TYPE_INT32:
			break;
		case ITEM_TYPE_UINT32:
			{
				uint32 Val = *(uint32 *)m_Ptr;
				if(m_Val.m_UInt32.m_Hexa)
				{
					swprintf(Tmp, L"0x%.8X", Val);
				}
				else
				{
					swprintf(Tmp, L"%u", Val);
				}
			}
			break;
		case ITEM_TYPE_FLOAT:
			{
				float Val = *(float *)m_Ptr;
				if(m_Val.m_Float32.m_Precision<0)
				{
					swprintf(Tmp, L"%g", Val);
				}
				else
				{
					wchar_t Fmt[64];
					swprintf(Fmt, L"%%.%df", (int)m_Val.m_Float32.m_Precision);
					swprintf(Tmp, Fmt, Val);
				}
			}
			break;
		case ITEM_TYPE_DOUBLE:
			break;
		case ITEM_TYPE_COLOR32:
			break;
		case ITEM_TYPE_COLOR3F:
			break;
		case ITEM_TYPE_COLOR4F:
			break;
		case ITEM_TYPE_CDSTRING:
			break;
		case ITEM_TYPE_STDSTRING:
			break;
		default:
		    break;
		}
		return Tmp;
	}
	UIGridItem(const std::wstring& wstrName, EGridItemType type, void* pData=NULL)
	{
		m_wstrName = wstrName;
		m_Type = type;
		SetRect(&rcActive, 0, 0, 0, 0);
		bSelected = false;
		m_pData = pData;
		if (m_pData)
		{
			m_Ptr = m_pData;
		}
		else
		{
			switch(m_Type)
			{
			case ITEM_TYPE_UNDEF:
				break;
			case ITEM_TYPE_BOOLCPP:
				break;
			case ITEM_TYPE_BOOL8:
				break;
			case ITEM_TYPE_BOOL16:
				break;
			case ITEM_TYPE_BOOL32:
				break;
			case ITEM_TYPE_CHAR:
				break;
			case ITEM_TYPE_INT8:
				break;
			case ITEM_TYPE_UINT8:
				break;
			case ITEM_TYPE_INT16:
				break;
			case ITEM_TYPE_UINT16:
				break;
			case ITEM_TYPE_INT32:
				break;
			case ITEM_TYPE_UINT32:
				m_Ptr = &m_Val.m_UInt32.m_Val;
				break;
			case ITEM_TYPE_FLOAT:
				m_Ptr = &m_Val.m_Float32.m_Val;
				break;
			case ITEM_TYPE_DOUBLE:
				break;
			case ITEM_TYPE_COLOR32:
				break;
			case ITEM_TYPE_COLOR3F:
				break;
			case ITEM_TYPE_COLOR4F:
				break;
			case ITEM_TYPE_CDSTRING:
				break;
			case ITEM_TYPE_STDSTRING:
				break;
			default:
				break;
			}
		}
	}
};

class DLL_EXPORT CUIGrid : public CUIControl
{
	struct Cell 
	{
		std::wstring wstrText;
		void* pData;
	};
public:
	CUIGrid();
	virtual ~CUIGrid();
	virtual void	SetParent(CUICombo *pControl);
	virtual void	XMLParse(TiXmlElement* pControlElement);
	virtual bool    CanHaveFocus() { return (m_bVisible && m_bEnabled); }
	virtual bool    HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual bool    HandleMouse(UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam);

	virtual void    OnFrameRender(double fTime, float fElapsedTime);
	virtual void    UpdateRects();

	DWORD	GetListBoxStyle() const { return m_dwStyle; }
	void	SetListBoxStyle(DWORD dwStyle) { m_dwStyle = dwStyle; }
	int		GetScrollBarWidth() const { return m_nSBWidth; }
	void	SetScrollBarWidth(int nWidth) { m_nSBWidth = nWidth; UpdateRects(); }
	void	SetBorder(int nBorder, int nMargin) { m_nBorder = nBorder; m_nMargin = nMargin; }

	uint32	GetLineCount();
	int		AddLine();


	Cell*	GetCell(int nX, int nY);
	std::wstring GetCellString(int nX, int nY);
	void	SetCell(int nX, int nY, const std::wstring&wstrText, void *pData=NULL);

	//void	SetCell(const std::wstring& wstrRow, int nLine, const std::wstring&wstrText, void *pData=NULL);

	void	Clear();

	void	AddRow(const std::wstring& wstrRow);
	int		GetRow(const std::wstring& wstrRow);
	int		GetLine(const std::wstring& wstrLine);
	Cell*	GetCell(const std::wstring& wstrX, const std::wstring& wstrY);
	void	SetCell(const std::wstring& wstrX, const std::wstring& wstrY, const std::wstring&wstrText, void *pData=NULL);

	int				GetSelectedIndex(int nPreviousSelected = -1);
	//UIGridItem*	GetSelectedItem(int nPreviousSelected = -1) { return GetItem(GetSelectedIndex(nPreviousSelected)); }
	//void			SelectItem(int nNewIndex);

	CUIScrollBar&	GetScrollBar(){return m_ScrollBar;}
	//CUIEditBox&		GetEditBox(){return m_EditBox;}
	//CUIComboBox&	GetComboBox(){return m_ComboBox;}
	//CUISlider&		GetSlider(){return m_Slider;}
	enum STYLE { MULTISELECTION = 1 };

protected:
	CRect<int>	m_rcText;      // Text rendering bound
	CRect<int>	m_rcSelection; // Selection box bound
	CUIScrollBar	m_ScrollBar;
	int		m_nSBWidth;
	int		m_nBorder;
	int		m_nMargin;
	int		m_nTextHeight;  // Height of a single line of text
	DWORD	m_dwStyle;    // List box style
	int		m_nSelected;    // Index of the selected item for single selection list box
	bool	m_bDrag;       // Whether the user is dragging the mouse to select

	//CUIEditBox	m_EditBox;
	//CUIComboBox	m_ComboBox;
	//CUISlider	m_Slider;
	std::wstring	m_strBuffer;
	std::vector<std::wstring> m_Row;
	std::vector<Cell> m_Cells;
	//std::vector<std::wstring> m_Row;
	//std::vector<UIGridItem*> m_Items;
};