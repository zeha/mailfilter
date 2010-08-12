// ExtendedListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExtendedListBox window

class CExtendedListBox : public CListBox
{
// Construction
public:
	CExtendedListBox();

public:
	// default implementation uses LSB of item data; override to get other behaviour
	virtual BOOL IsItemEnabled(UINT) const;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtendedListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtendedListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtendedListBox)
	//}}AFX_MSG


	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
