#pragma once


// CSetting_Aqtk10 �_�C�A���O

class CSetting_Aqtk10 : public CDialogEx
{
	DECLARE_DYNAMIC(CSetting_Aqtk10)

public:
	CSetting_Aqtk10(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CSetting_Aqtk10();
	void InitialUpdate();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_SETTING_AQTK10 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl m_Accent;
	CSliderCtrl m_Ontei;
	CSliderCtrl m_Ontei2;
	CSliderCtrl m_Speed;
	CSliderCtrl m_Takasa;
	CSliderCtrl m_Volume;
	CStatic m_AccentVal;
	CStatic m_Ontei2Val;
	CStatic m_OnteiVal;
	CStatic m_SpeedVal;
	CStatic m_TakasaVal;
	CStatic m_VolumeVal;
	CComboBox m_VoiceSel;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelchangeVoicesel();
};
