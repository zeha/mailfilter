#pragma once
#include "mf_c.h"

// CListCtrlEx

class CListCtrlEx : public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlEx)

public:
	CListCtrlEx();
	virtual ~CListCtrlEx();

protected:
	DECLARE_MESSAGE_MAP()
public:
	// Fills in values from MFConfig
	int FillIn(void);
	int UpdateValue(MF_C_Storage* mfcs);
};


