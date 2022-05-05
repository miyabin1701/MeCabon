#pragma once


// CSetting_Sapi54 ダイアログ

class CSetting_Sapi54 : public CDialogEx
{
	DECLARE_DYNAMIC(CSetting_Sapi54)

public:
	CSetting_Sapi54(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CSetting_Sapi54();
	void InitialUpdate();
	void SetVoiceName();

// ダイアログ データ
	enum { IDD = IDD_SETTING_SAPI54 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelchangeVoicesel();
	CSliderCtrl m_Speed;
	CStatic m_SpeedVal;
	CComboBox m_VoiceSel;
	CSliderCtrl m_Volume;
	CStatic m_VolumeVal;
	CComboBox m_ModeSel;
	afx_msg void OnSelchangeModesel();
	CEdit m_AftMuonms;
	afx_msg void OnChangeAftmuonedit();
	afx_msg void OnClickedVoicevoxref();
	CEdit m_VoiceVoxPath;
};
