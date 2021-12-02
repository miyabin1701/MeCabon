// SettingTab.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MecaDoku.h"
#include "SettingTab.h"
#include "afxdialogex.h"

extern TTSNAME	SelectTts;

// CSettingTab ダイアログ

IMPLEMENT_DYNAMIC(CSettingTab, CDialogEx)

CSettingTab::CSettingTab(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettingTab::IDD, pParent)
{
	mp_SettingMecab = NULL;
	mp_SettingSapi54 = NULL;
	mp_SettingAqtk10 = NULL;
	mp_SettingOther = NULL;
}

CSettingTab::~CSettingTab()
{
}

void CSettingTab::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SETTINGS, m_SettingTab);
//	DDX_Control(pDX, IDC_COMBO1, m_TTSSel);
	DDX_Control(pDX, IDC_TTSSEL, m_TtsSelector);
}


BEGIN_MESSAGE_MAP(CSettingTab, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_SETTINGS, &CSettingTab::OnSelchangeSettings)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_TTSSEL, &CSettingTab::OnSelchangeTtssel)
END_MESSAGE_MAP()


// CSettingTab メッセージ ハンドラー

void CSettingTab::InitialUpdate()
{	CMecaDokuApp *lpApp = ( CMecaDokuApp * )AfxGetApp();
	CRect	rct;

	m_SettingTab.InsertItem( 0, L"Mecab" );
	m_SettingTab.InsertItem( 1, L"Sapi5.4" );
	m_SettingTab.InsertItem( 2, L"Aqtk10" );
	m_SettingTab.InsertItem( 3, L"Other" );
	m_TtsSelector.AddString(( LPCTSTR )L"SAPI5.4" );
	m_TtsSelector.AddString(( LPCTSTR )L"AQTK10" );
	m_TtsSelector.SetCurSel( SelectTts );

	mp_SettingMecab = new CSetting_Mecab( &m_SettingTab );
	mp_SettingMecab->Create( CSetting_Mecab::IDD, &m_SettingTab );
//	mp_SettingMecab->Create( CSetting_Mecab::IDD, this );
//	mp_SettingMecab->MoveWindow( &rct );
	mp_SettingMecab->InitialUpdate();

	mp_SettingSapi54 = new CSetting_Sapi54( this );
	mp_SettingSapi54->Create( CSetting_Sapi54::IDD, &m_SettingTab );
//	mp_SettingSapi54->Create( CSetting_Sapi54::IDD, this );
//	mp_SettingSapi54->MoveWindow( &rct );
	mp_SettingSapi54->InitialUpdate();

	mp_SettingAqtk10 = new CSetting_Aqtk10( this );
	mp_SettingAqtk10->Create( CSetting_Aqtk10::IDD, &m_SettingTab );
//	mp_SettingAqtk10->Create( CSetting_Aqtk10::IDD, this );
//	mp_SettingAqtk10->MoveWindow( &rct );
	mp_SettingAqtk10->InitialUpdate();

	mp_SettingOther = new Setting_Other( this );
	mp_SettingOther->Create( Setting_Other::IDD, &m_SettingTab );
	mp_SettingOther->InitialUpdate();

	m_SettingTab.SetCurSel( CurSel = 0 );
	mp_SettingMecab->ShowWindow( SW_SHOW );
	mp_SettingSapi54->ShowWindow( SW_HIDE );
	mp_SettingAqtk10->ShowWindow( SW_HIDE );

	m_SettingTab.GetClientRect( &rct );
	OnSize( 0, rct.right, rct.bottom );
}


void CSettingTab::OnSelchangeSettings(NMHDR *pNMHDR, LRESULT *pResult)
{	int chgSel;

	switch ( chgSel = m_SettingTab.GetCurSel())
	{ case 0:		// Mecab
	  default:
		mp_SettingSapi54->ShowWindow( SW_HIDE );
		mp_SettingAqtk10->ShowWindow( SW_HIDE );
		mp_SettingOther->ShowWindow( SW_HIDE );
		mp_SettingMecab->ShowWindow( SW_SHOW );
		break;
	  case 1:		// Sapi54
		if ( SelectTts == 0 )
		{	mp_SettingMecab->ShowWindow( SW_HIDE );
			mp_SettingAqtk10->ShowWindow( SW_HIDE );
			mp_SettingOther->ShowWindow( SW_HIDE );
			mp_SettingSapi54->ShowWindow( SW_SHOW );
		}
		else
		{	m_SettingTab.SetCurSel( chgSel = CurSel );
		}
		break;
	  case 2:		// Aqtk10
		if ( SelectTts == 1 )
		{	mp_SettingMecab->ShowWindow( SW_HIDE );
			mp_SettingSapi54->ShowWindow( SW_HIDE );
			mp_SettingOther->ShowWindow( SW_HIDE );
			mp_SettingAqtk10->ShowWindow( SW_SHOW );
		}
		else
		{	m_SettingTab.SetCurSel( chgSel = CurSel );
		}
		break;
	  case 3:		// Other
		mp_SettingMecab->ShowWindow( SW_HIDE );
		mp_SettingSapi54->ShowWindow( SW_HIDE );
		mp_SettingAqtk10->ShowWindow( SW_HIDE );
		mp_SettingOther->ShowWindow( SW_SHOW );
		break;
	}
	CurSel = chgSel;
	*pResult = 0;
}



void CSettingTab::OnDestroy()
{
	mp_SettingSapi54->DestroyWindow();
	delete mp_SettingSapi54;

	mp_SettingAqtk10->DestroyWindow();
	delete mp_SettingAqtk10;

	mp_SettingMecab->DestroyWindow();
	delete mp_SettingMecab;

	mp_SettingOther->DestroyWindow();
	delete mp_SettingOther;

	CDialogEx::OnDestroy();
}


void CSettingTab::OnSize(UINT nType, int cx, int cy)
{	CRect rct;

	CDialogEx::OnSize(nType, cx, cy);

	if (( cx <= 0 ) || ( cy <= 0 ) ||
		( mp_SettingMecab == NULL ) ||
		( mp_SettingAqtk10 == NULL ) ||
		( mp_SettingSapi54 == NULL ))
	{	return;
	}
//	m_SettingTab.GetClientRect( &rct );
	m_SettingTab.GetWindowRect( &rct );
	ScreenToClient( &rct );
	m_SettingTab.AdjustRect( FALSE, &rct );
//	m_SettingTab.AdjustRect( TRUE, &rct );
//	rct.left += 2;
//	rct.right -= 4;
//	rct.top += 200;
//	rct.bottom -= 4;
	rct.InflateRect( 16, 13 ); 
	mp_SettingMecab->MoveWindow( &rct );
	mp_SettingAqtk10->MoveWindow( &rct );
	mp_SettingSapi54->MoveWindow( &rct );
	mp_SettingOther->MoveWindow( &rct );
}


void CSettingTab::OnSelchangeTtssel()
{	SelectTts = ( TTSNAME )m_TtsSelector.GetCurSel();
#if 0
	switch ( SelectTts )
	{ case 0:
		m_SettingTab.InsertItem( 1, L"Sapi5.4" );
		m_SettingTab.InsertItem( 2, L"Aqtk10" );
		break;
	  case 1:
		m_SettingTab.Remove( 1, L"Sapi5.4" );
		m_SettingTab.InsertItem( 2, L"Aqtk10" );
		break;
	}
#endif
}


BOOL CSettingTab::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN )
	{	if (( pMsg->wParam == VK_RETURN ) || ( pMsg->wParam == VK_ESCAPE ))
		{	return ( FALSE );
	}	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CSettingTab::OnCancel()
{	CDialogEx::OnCancel();
}
