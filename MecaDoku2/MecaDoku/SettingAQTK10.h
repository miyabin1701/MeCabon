#pragma once


// CSettingAQTK10 �_�C�A���O

class CSettingAQTK10 : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingAQTK10)

public:
	CSettingAQTK10(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CSettingAQTK10();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_SETTING_AQTK10 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl m_Speed;
	CSliderCtrl m_Ontei;
	CSliderCtrl m_Ontei2;
	CSliderCtrl m_Takasa;
	CSliderCtrl m_Volume;
	CSliderCtrl m_Accent;
	CStatic m_AccentVal;
	CStatic m_Ontei2Val;
	CStatic m_OnteiVal;
	CStatic m_SpeedVal;
	CStatic m_TakasaVal;
	CComboBox m_VoiceSel;
	CStatic m_VolumeVal;
	CButton m_UseAqtk10;
};
