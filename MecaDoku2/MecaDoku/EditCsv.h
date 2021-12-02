#pragma once


// CEditCsv

class CEditCsv : public CEdit
{
	DECLARE_DYNAMIC(CEditCsv)

public:
	CEditCsv();
	virtual ~CEditCsv();

protected:
public:
	DECLARE_MESSAGE_MAP()
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


