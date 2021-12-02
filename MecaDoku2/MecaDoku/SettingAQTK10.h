#pragma once


// CSettingAQTK10 ダイアログ

class CSettingAQTK10 : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingAQTK10)

public:
	CSettingAQTK10(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CSettingAQTK10();

// ダイアログ データ
	enum { IDD = IDD_SETTING_AQTK10 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

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
