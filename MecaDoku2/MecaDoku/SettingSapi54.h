#pragma once


// CSettingSapi54 �_�C�A���O

class CSettingSapi54 : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingSapi54)

public:
	CSettingSapi54(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CSettingSapi54();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_SETTING_SAPI54 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl m_Speed;
	CStatic m_SpeedVal;
	CComboBox m_VoiceSel;
	CSliderCtrl m_Volume;
	CStatic m_VolumeVal;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
