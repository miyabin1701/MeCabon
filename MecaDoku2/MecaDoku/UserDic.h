#pragma once

#include "EditCsv.h"



struct CsvDicInfo
{	int		iLineNo;
	CString LineText;
	CString Hyosou;			// �\�w�`
	int	lId;				// ������ID
	int	rId;				// �E����ID
	int	cost;				// �R�X�g
	CString Hinshi;			// �i��,�i���ו���1,�i���ו���2,�i���ו���3
	CString Katsuyou;		// ���p�`,���p�^
	CString Genkei;			// ���`
	CString yomi;			// �ǂ�
	CString hatsuon;		// ����
};


// CUserDic �_�C�A���O

class CUserDic : public CDialogEx
{
	DECLARE_DYNAMIC(CUserDic)

public:
	CUserDic(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CUserDic();
	void InitialUpdate();
	BOOL onUserDic( CString TergetWord, struct CsvDicInfo *csvInfo );
	void CostCalc( wchar_t *szLine );

// �_�C�A���O �f�[�^
	enum { IDD = IDD_USERDICUPDATE };
//	struct CsvDicInfo csvInfo;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
//	CListCtrl m_newWord;
//	afx_msg void OnClickedWordbtn();
	afx_msg void OnClickedUpdate();
	afx_msg void OnClickedBuildbtn();
	CEdit m_OutPut;
	CComboBox m_Hinshi;
//	CEdit m_Hatuon;
	CEdit m_SeikiCost;
//	CEdit m_Word;
//	CEdit m_Yomi;
	afx_msg void OnChangeWordedit();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	CEditCsv m_CsvEdit;
	CComboBox m_HinshiCmb;
	CComboBox m_LeftId;
	CComboBox m_RightId;
protected:
	afx_msg LRESULT OnUmUseraction(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnSelchangeLeftid();
	afx_msg void OnEditupdateLeftid();
	afx_msg void OnSelchangeRightid();
	afx_msg void OnSelchangeHinshi();
};
