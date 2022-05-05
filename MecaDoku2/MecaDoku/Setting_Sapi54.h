#pragma once


// CSetting_Sapi54 �_�C�A���O

class CSetting_Sapi54 : public CDialogEx
{
	DECLARE_DYNAMIC(CSetting_Sapi54)

public:
	CSetting_Sapi54(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CSetting_Sapi54();
	void InitialUpdate();
	void SetVoiceName();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_SETTING_SAPI54 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

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
