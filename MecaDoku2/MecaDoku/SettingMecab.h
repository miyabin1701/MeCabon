#pragma once


// CSettingMecab �_�C�A���O

class CSettingMecab : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingMecab)

public:
	CSettingMecab(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CSettingMecab();
	void InitialUpdate();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_SETTING_MECAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_MeCabDir;
	CComboBox m_fromCodeSet;
	CEdit m_EditorOption;
	CEdit m_MecabSysDicDir;
	CEdit m_SysDicWork;
	CEdit m_textEditer;
	CComboBox m_toCodeSet;
	CEdit m_UserDictionaly;
	CEdit m_UserDicCsv;
	afx_msg void OnKillfocusMecabDir();
	afx_msg void OnKillfocusMecabSysdic();
	afx_msg void OnKillfocusUserDictionaly();
	afx_msg void OnKillfocusUserdicCsv();
	afx_msg void OnKillfocusSysdicWork();
	afx_msg void OnKillfocusTextEditer();
	afx_msg void OnKillfocusEdtorOpt();
	afx_msg void OnClickedMecabref();
	afx_msg void OnClickedMecabsysdicref();
	afx_msg void OnClickedRefuser();
	afx_msg void OnClickedRefusercsv();
	afx_msg void OnClickedSysdicworkref();
	afx_msg void OnClickedTextediterref();
	afx_msg void OnSelchangeFromcodesetcmb();
	afx_msg void OnSelchangeTocodesetcmb();
};
