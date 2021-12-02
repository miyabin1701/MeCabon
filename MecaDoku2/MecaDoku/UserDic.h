#pragma once

#include "EditCsv.h"



struct CsvDicInfo
{	int		iLineNo;
	CString LineText;
	CString Hyosou;			// 表層形
	int	lId;				// 左文脈ID
	int	rId;				// 右文脈ID
	int	cost;				// コスト
	CString Hinshi;			// 品詞,品詞細分類1,品詞細分類2,品詞細分類3
	CString Katsuyou;		// 活用形,活用型
	CString Genkei;			// 原形
	CString yomi;			// 読み
	CString hatsuon;		// 発音
};


// CUserDic ダイアログ

class CUserDic : public CDialogEx
{
	DECLARE_DYNAMIC(CUserDic)

public:
	CUserDic(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CUserDic();
	void InitialUpdate();
	BOOL onUserDic( CString TergetWord, struct CsvDicInfo *csvInfo );
	void CostCalc( wchar_t *szLine );

// ダイアログ データ
	enum { IDD = IDD_USERDICUPDATE };
//	struct CsvDicInfo csvInfo;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

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
