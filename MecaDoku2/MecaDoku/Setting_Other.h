#pragma once


// Setting_Other ダイアログ

class Setting_Other : public CDialogEx
{
	DECLARE_DYNAMIC(Setting_Other)

public:
	Setting_Other(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~Setting_Other();
	void InitialUpdate();

// ダイアログ データ
	enum { IDD = IDD_SETTING_OTHER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_BgBlack;
	afx_msg void OnClickedBgblack();
};
