// ExtendedListBox.cpp : implementation file
//

#include "stdafx.h"
#include "ExtendedListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtendedListBox

CExtendedListBox::CExtendedListBox()
{
}

CExtendedListBox::~CExtendedListBox()
{
}


// default implementation
BOOL CExtendedListBox::IsItemEnabled(UINT nIndex) const
{
	if (nIndex>=(DWORD)GetCount())
		return TRUE;	// whatever

	DWORD uData=GetItemData(nIndex);

	return (uData&1);
}


BEGIN_MESSAGE_MAP(CExtendedListBox, CListBox)
	//{{AFX_MSG_MAP(CExtendedListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtendedListBox message handlers

void CExtendedListBox::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	ASSERT(GetStyle() & LBS_OWNERDRAWFIXED);
	ModifyStyle(0, LBS_HASSTRINGS | LBS_WANTKEYBOARDINPUT, 0);
	
	CListBox::PreSubclassWindow();
}

void CExtendedListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	ASSERT((GetStyle() & (LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_WANTKEYBOARDINPUT)) ==
		(LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_WANTKEYBOARDINPUT));

	CDC* pDC = CDC::FromHandle (lpDrawItemStruct->hDC);

	if (((LONG)(lpDrawItemStruct->itemID) >= 0) &&
		(lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)))
	{
		BOOL fDisabled = !IsWindowEnabled () || !IsItemEnabled(lpDrawItemStruct->itemID);

		COLORREF newTextColor = fDisabled ?
			RGB(0x80, 0x80, 0x80) : GetSysColor (COLOR_WINDOWTEXT);  // light gray

		COLORREF oldTextColor = pDC->SetTextColor (newTextColor);

		COLORREF newBkColor = GetSysColor (COLOR_WINDOW);
		COLORREF oldBkColor = pDC->SetBkColor (newBkColor);

		if (newTextColor == newBkColor)
			newTextColor = RGB(0xC0, 0xC0, 0xC0);   // dark gray

		if (!fDisabled && ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0))
		{
			pDC->SetTextColor (GetSysColor (COLOR_HIGHLIGHTTEXT));
			pDC->SetBkColor (GetSysColor (COLOR_HIGHLIGHT));
		}

		CString strText;
		GetText (lpDrawItemStruct->itemID, strText);

		const RECT &rc=lpDrawItemStruct->rcItem;

		pDC->ExtTextOut(rc.left + 2,
				  rc.top + 2,// + max(0, (cyItem - m_cyText) / 2),
				  ETO_OPAQUE, &rc,
				  strText, strText.GetLength (), NULL);

		pDC->SetTextColor (oldTextColor);
		pDC->SetBkColor (oldBkColor);
	}

	if ((lpDrawItemStruct->itemAction & ODA_FOCUS) != 0)
		pDC->DrawFocusRect(&lpDrawItemStruct->rcItem);
}

void CExtendedListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your code to determine the size of specified item
	UNREFERENCED_PARAMETER(lpMeasureItemStruct);
}
