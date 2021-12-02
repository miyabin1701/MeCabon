#pragma once


// CSetting_Mecab ダイアログ

class CSetting_Mecab : public CDialogEx
{
	DECLARE_DYNAMIC(CSetting_Mecab)

public:
	CSetting_Mecab(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CSetting_Mecab();
	void InitialUpdate();

// ダイアログ データ
	enum { IDD = IDD_SETTING_MECAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillfocusMecabDir();
	afx_msg void OnKillfocusMecabSysdic();
	afx_msg void OnKillfocusUserDictionaly();
	afx_msg void OnKillfocusUserdicCsv();
	afx_msg void OnKillfocusSysdicWork();
	afx_msg void OnKillfocusTextEditor();
	afx_msg void OnKillfocusEdtorOpt();
	afx_msg void OnClickedMecabref();
	afx_msg void OnClickedMecabsysdicref();
	afx_msg void OnClickedRefuser();
	afx_msg void OnClickedRefusercsv();
	afx_msg void OnClickedSysdicworkref();
	afx_msg void OnClickedTexteditorref();
	afx_msg void OnSelchangeFromcodesetcmb();
	afx_msg void OnSelchangeTocodesetcmb();
	CEdit m_MeCabDir;
	CEdit m_MecabSysDicDir;
	CEdit m_SysDicWork;
	CEdit m_textEditor;
	CEdit m_UserDictionaly;
	CEdit m_UserDicCsv;
	CEdit m_EditorOption;
	CComboBox m_fromCodeSet;
	CComboBox m_toCodeSet;
};
