#pragma once

#include "ZoomEdit.h"

// CSarchCsv �_�C�A���O

class CSarchCsv : public CDialogEx
{
	DECLARE_DYNAMIC(CSarchCsv)

public:
	CSarchCsv(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CSarchCsv();
	void InitialUpdate();
	void SetSarchStr( wchar_t *psarchstr );
	void CatUnnounStr( wchar_t *psarchstr );
	void grep( char *pszStr, wchar_t *oszfname );
	void SetEditBox( wchar_t *pStr );


// �_�C�A���O �f�[�^
	enum { IDD = IDD_SARCHCSV };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	CZoomEdit<CRichEditCtrl> m_Output;
	CZoomEdit<CRichEditCtrl> m_SarchStr;
	CZoomEdit<CButton> m_SarchBtn;
	CZoomEdit<CButton> m_BuildSysDic;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickedSarchcsv();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnCopy();
	afx_msg void OnDel();
	afx_msg void OnPaste();
	afx_msg void OnJumpeditor();
	afx_msg void OnAllclear();
	afx_msg void OnClickedBuildsysdic();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnCancel();
	afx_msg void OnCopyw();
	afx_msg void OnDelw();
	afx_msg void OnPastew();
protected:
	afx_msg LRESULT OnUmStateChange(WPARAM wParam, LPARAM lParam);
public:
};
