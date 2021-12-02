#pragma once

#include "ZoomEdit.h"

// CCorpasDlg �_�C�A���O

class CCorpasDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCorpasDlg)

public:
	CCorpasDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CCorpasDlg();
	void InitialUpdate();
	void toCorpas( wchar_t *psrc );
	void OnDsplayOutputDlg();



// �_�C�A���O �f�[�^
	enum { IDD = IDD_CORPAS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedCorpasapend();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnDel();
	CZoomEdit<CRichEditCtrl> m_Corpas;
//	CRichEditCtrl m_Corpas;
//	CZoomEdit<CButton> m_CorpasAppend;
//	CZoomEdit<CButton> m_CorpasRef;
//	CZoomEdit<CButton> m_TestAppend;
//	CZoomEdit<CButton> m_TestRef;
//	CZoomEdit<CEdit> m_CorpasFile;
//	CZoomEdit<CEdit> m_TestFile;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCsvserch();
	afx_msg void OnAllclear2();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnCancel();
};
