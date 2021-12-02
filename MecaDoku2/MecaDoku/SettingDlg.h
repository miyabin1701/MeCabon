#pragma once


// CSettingDlg ダイアログ

class CSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CSettingDlg();
	void InitialUpdate();


// ダイアログ データ
	enum { IDD = IDD_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_AccentVal;
	CStatic m_Ontei2Val;
	CStatic m_OnteiVal;
	CStatic m_SpeedVal;
	CStatic m_TakasaVal;
	CStatic m_VolumeVal;
	CSliderCtrl m_Accent;
	CSliderCtrl m_Ontei;
	CSliderCtrl m_Ontei2;
	CSliderCtrl m_Speed;
	CSliderCtrl m_Takasa;
	CSliderCtrl m_Volume;
	CString m_MeCabDir;
	CString m_UserDictionaly;
	CString m_SysDicWork;
	CString m_EditorOption;
	CComboBox m_VoiceSel;
	afx_msg void OnClickedMecabref();
	afx_msg void OnBnClickedRefuser();
	afx_msg void OnClickedsysdicworkref();

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelchangeVoicesel();
	CComboBox m_fromCodeSet;
	CComboBox m_toCodeSet;
	CString m_textEditer;
	CString m_MecabSysDicDir;
	CString m_UserDicCsv;
	afx_msg void OnClickedBuilduserdicbtn();
	afx_msg void OnClickedMecabsysdicref();
	afx_msg void OnClickedTextediterref();
	afx_msg void OnClickedRunediterbtn();
	afx_msg void OnClickedRefusercsv();
	afx_msg void OnBnClickedOk();
	afx_msg void OnKillfocusTextediter();
	afx_msg void OnKillfocusUserdiccsv();
	afx_msg void OnKillfocusUserdictionaly();
	afx_msg void OnKillfocusMecabsysdic();
	afx_msg void OnKillfocusMecabdir();
	afx_msg void OnKillfocusSysDicWork();
	afx_msg void OnKillfocusEditOpt();
};
