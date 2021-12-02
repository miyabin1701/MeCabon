#pragma once

#include "Setting_Mecab.h"
#include "Setting_Aqtk10.h"
#include "Setting_Sapi54.h"
#include "Setting_Other.h"



// CSettingTab �_�C�A���O

class CSettingTab : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingTab)

public:
	CSettingTab(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CSettingTab();
	void InitialUpdate();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_SETTINGTAB };
	CSetting_Sapi54	*mp_SettingSapi54;
	CSetting_Aqtk10	*mp_SettingAqtk10;
	CSetting_Mecab	*mp_SettingMecab;
	Setting_Other	*mp_SettingOther;
	int CurSel;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_SettingTab;
	afx_msg void OnSelchangeSettings(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
//	CComboBox m_TTSSel;
	CComboBox m_TtsSelector;
	afx_msg void OnSelchangeTtssel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnCancel();
};
