#pragma once


// Setting_Other �_�C�A���O

class Setting_Other : public CDialogEx
{
	DECLARE_DYNAMIC(Setting_Other)

public:
	Setting_Other(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~Setting_Other();
	void InitialUpdate();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_SETTING_OTHER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_BgBlack;
	afx_msg void OnClickedBgblack();
};
