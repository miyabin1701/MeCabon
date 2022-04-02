
// MecaDokuDlg.h : ヘッダー ファイル
//
//
// 再生中にWM_CLOSEが来た際、クリティカルセクションを削除するが
// その後次の再生テキスト取得メッセージが来てクリティカルセクションにアクセスしエクセプションが発生する対策
//

#pragma once
#include "afxwin.h"
#include "ZoomEdit.h"
#include "afxcmn.h"
#include "SettingTab.h"
//#include "UserDic.h"
#include "PartEdit.h"
#include "CorpasDlg.h"
#include "SarchCsv.h"

#define UM_REQUEST_NEXTLINE		( WM_USER + 1 )
#define UM_REQUEST_SELLINE		( WM_USER + 2 )
#define WM_LOADRECENTFILE		( WM_USER + 3 )
#define UM_STATE_CHANGE			( WM_USER + 4 )
//#define UM_ENABLE_VOLUMESLIDER	( WM_USER + 5 )


	enum WAVEOUTOPENTATE {
		WFNONOPEN	= 0x00,
		WFAQTK10	= 0x01,
		WFSAPI		= 0x02,
//		PAUSE		= 0x03,
	};


	enum PLAYSTATE {
		STOP	= 0x00,
		PRESTOP	= 0x01,
		PLAY	= 0x02,
		PAUSE	= 0x03,
	};


struct LINEBUFS					// playing line string buffer
{	int		iLineNo;			// < 0 then unuse
	size_t	ctLineBuf;
	char	*pszLineBuf;
};


struct PLAYINGSENTENCE			// CEdit select char pos
{	int		iLineNo;
	int		ixs;
	int		ixe;
};


// CMecaDokuDlg ダイアログ
class CMecaDokuDlg : public CDialogEx
{
// コンストラクション
public:
	CMecaDokuDlg(CWnd* pParent = NULL);	// 標準コンストラクター
//	CFont font;

// ダイアログ データ
	enum { IDD = IDD_MECADOKU_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;
	CWinThread *pThread;
	HACCEL m_hAccel;
	BOOL	fgEcxec;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
//	afx_msg UINT OnPowerBroadcast(UINT nPowerEvent, UINT nEventData);
//	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSkipspeak();

public:

//	enum PLAYSTATE iPlayState;

	afx_msg void OnSize(UINT nType, int cx, int cy);
//	CZoomEdit<CEdit> PlayTextEdit;
	CZoomEdit<CPartEdit> PlayTextEdit;
	CZoomEdit<CButton> btnPlay;
	CZoomEdit<CButton> btnStop;
//	CZoomEdit<CButton> btnPause;
//	CZoomEdit<CButton> btnSetting;
//	CZoomEdit<CButton> btnDictionary;
	CZoomEdit<CSliderCtrl> sldVolume;
//	CZoomEdit<CSliderCtrl> sldFontSize;
//	CZoomEdit<CButton> m_toCorpas;
	CZoomEdit<CStatic> m_VolComment;
	CZoomEdit<CButton> m_SarchBtn;
	CZoomEdit<CEdit> m_SarchStr;

	CFont editFont;
	CSettingTab	*pSettingTab;
//	CSettingDlg *pSetting;
//	CCorpasDlg	*pCorpasDlg;
//	CUserDic *pUserDict;

	afx_msg void OnClickedPlay();
	afx_msg void OnClickedPause();
	afx_msg void OnClickedStop();
protected:
	afx_msg LRESULT OnUmRequestNextline(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUmRequestSelline(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUmEnableSlider(WPARAM wParam, LPARAM lParam);
	void LoadPdf( wchar_t *szFile );
	void LoadText( wchar_t *szFile );

public:
//	afx_msg void OnThemechangedVolumeslider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnClickedSetting();
//	afx_msg void OnClickedDictionaly();
	afx_msg void OnDestroy();
//	CSliderCtrl m_sldFontSize;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnClicked2corpas();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnDel();
	afx_msg void OnCorpas();
	afx_msg void OnAllclr();
//	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL DestroyWindow();
	afx_msg void OnClearbef();
	afx_msg void OnPlaythis();
	afx_msg void OnClickedSarch();
	afx_msg void OnUndo();
	afx_msg void OnPausedspoff();
	afx_msg void OnKigoupass();
	afx_msg void OnLoadrecentfile();
protected:
public:
	afx_msg UINT OnPowerBroadcast(UINT nPowerEvent, UINT nEventData);
	afx_msg void OnClose();
};
