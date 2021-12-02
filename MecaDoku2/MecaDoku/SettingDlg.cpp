// SettingDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "stdio.h"
#include "MecaDoku.h"
#include "SettingDlg.h"
#include "afxdialogex.h"

#ifdef JTALK
#include "jtalk.h"
#else
#include "AquesTalk.h"
#endif

extern AQTK_VOICE voice;
extern wchar_t szMeCabExe[_MAX_PATH];
extern wchar_t	szUserDicCsv[_MAX_PATH];
extern wchar_t szUserDictionary[_MAX_PATH];
extern wchar_t	szSysDictionary[_MAX_PATH];
extern wchar_t	szTextEditer[_MAX_PATH];
extern wchar_t	szSysDicWork[_MAX_PATH];
extern wchar_t	szEditOpt[64];


// CSettingDlg ダイアログ

IMPLEMENT_DYNAMIC(CSettingDlg, CDialog)

CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingDlg::IDD, pParent)
{

	m_MeCabDir = _T("");
	m_UserDictionaly = _T("");
	m_textEditer = _T("");
	m_MecabSysDicDir = _T("");
	m_UserDicCsv = _T("");
	m_SysDicWork = _T("");
	m_EditorOption = _T("");
}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ACCENTVAL, m_AccentVal);
	DDX_Control(pDX, IDC_ONTEI2VAL, m_Ontei2Val);
	DDX_Control(pDX, IDC_ONTEIVAL, m_OnteiVal);
	DDX_Control(pDX, IDC_SPEEDVAL, m_SpeedVal);
	DDX_Control(pDX, IDC_TAKASAVAL, m_TakasaVal);
	DDX_Control(pDX, IDC_ACCENT, m_Accent);
	DDX_Control(pDX, IDC_ONTEI, m_Ontei);
	DDX_Control(pDX, IDC_ONTEI2, m_Ontei2);
	DDX_Control(pDX, IDC_SPEED, m_Speed);
	DDX_Control(pDX, IDC_TAKASA, m_Takasa);
	DDX_Text(pDX, IDC_MECABDIR, m_MeCabDir);
	DDV_MaxChars(pDX, m_MeCabDir, 512);
	DDX_Text(pDX, IDC_USERDICTIONALY, m_UserDictionaly);
	DDV_MaxChars(pDX, m_UserDictionaly, 512);
	DDX_Control(pDX, IDC_VOICESEL, m_VoiceSel);
	DDX_Control(pDX, IDC_FROMCODESETCMB, m_fromCodeSet);
	DDX_Control(pDX, IDC_TOCODESETCMB, m_toCodeSet);
	DDX_Text(pDX, IDC_TEXTEDITER, m_textEditer);
	DDX_Text(pDX, IDC_MECABSYSDIC, m_MecabSysDicDir);
	DDX_Text(pDX, IDC_USERDICCSV, m_UserDicCsv);
	DDX_Text(pDX, IDC_SYSDICWORK, m_SysDicWork);
	DDX_Control(pDX, IDC_VOLUME, m_Volume);
	DDX_Text(pDX, IDC_EDTOROPT, m_EditorOption);
	DDX_Control(pDX, IDC_VOLUMEVAL, m_VolumeVal);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialog)
	ON_BN_CLICKED(IDC_MECABREF, &CSettingDlg::OnClickedMecabref)
	ON_BN_CLICKED(IDC_REFUSER, &CSettingDlg::OnBnClickedRefuser)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_VOICESEL, &CSettingDlg::OnSelchangeVoicesel)
	ON_BN_CLICKED(IDC_BUILDUSERDICBTN, &CSettingDlg::OnClickedBuilduserdicbtn)
	ON_BN_CLICKED(IDC_MECABSYSDICREF, &CSettingDlg::OnClickedMecabsysdicref)
	ON_BN_CLICKED(IDC_TEXTEDITERREF, &CSettingDlg::OnClickedTextediterref)
	ON_BN_CLICKED(IDC_SYSDICWORKREF, &CSettingDlg::OnClickedsysdicworkref)
	ON_BN_CLICKED(IDC_RUNEDITERBTN, &CSettingDlg::OnClickedRunediterbtn)
	ON_BN_CLICKED(IDC_REFUSERCSV, &CSettingDlg::OnClickedRefusercsv)
	ON_BN_CLICKED(IDOK, &CSettingDlg::OnBnClickedOk)
	ON_EN_KILLFOCUS(IDC_TEXTEDITER, &CSettingDlg::OnKillfocusTextediter)
	ON_EN_KILLFOCUS(IDC_USERDICCSV, &CSettingDlg::OnKillfocusUserdiccsv)
	ON_EN_KILLFOCUS(IDC_USERDICTIONALY, &CSettingDlg::OnKillfocusUserdictionaly)
	ON_EN_KILLFOCUS(IDC_MECABSYSDIC, &CSettingDlg::OnKillfocusMecabsysdic)
	ON_EN_KILLFOCUS(IDC_MECABDIR, &CSettingDlg::OnKillfocusMecabdir)
	ON_EN_KILLFOCUS(IDC_EDTOROPT, &CSettingDlg::OnKillfocusEditOpt)
	ON_EN_KILLFOCUS(IDC_SYSDICWORK, &CSettingDlg::OnKillfocusSysDicWork)
END_MESSAGE_MAP()


// CSettingDlg メッセージ ハンドラー


void CSettingDlg::OnClickedMecabref()
{	CFileDialog Dlg( TRUE, ( LPCTSTR )L"mecab", ( LPCTSTR )L"mecab.exe", OFN_HIDEREADONLY | OFN_EXTENSIONDIFFERENT | OFN_NOCHANGEDIR,
			( LPCTSTR )L"mecab (*.exe)|*.exe|All Files (*.*)|*.*||" );

	Dlg.m_ofn.lpstrFile = ( LPWSTR )&szMeCabExe[0];
	if ( Dlg.DoModal() != IDOK )
	{	return;
	}
	wcscpy_s( &szMeCabExe[0], lengthof( szMeCabExe ), Dlg.m_ofn.lpstrFile );
	m_MeCabDir = Dlg.m_ofn.lpstrFile;
	CWnd *pMeCabDir = ( CWnd * )GetDlgItem( IDC_MECABDIR );
	pMeCabDir->SetWindowTextW( m_MeCabDir );
}


void CSettingDlg::OnBnClickedRefuser()
{	CFileDialog Dlg( FALSE, ( LPCTSTR )L"dic", ( LPCTSTR )L"user.dic", OFN_HIDEREADONLY | OFN_EXTENSIONDIFFERENT | OFN_NOCHANGEDIR,
			( LPCTSTR )L"mecab (*.dic)|*.dic|All Files (*.*)|*.*||" );

	Dlg.m_ofn.lpstrFile = ( LPWSTR )&szUserDictionary[0];
	if ( Dlg.DoModal() != IDOK )
	{	return;
	}
	wcscpy_s( &szUserDictionary[0], lengthof( szUserDictionary ), Dlg.m_ofn.lpstrFile );
	m_UserDictionaly = Dlg.m_ofn.lpstrFile;
	CWnd *pDictionary = ( CWnd * )GetDlgItem( IDC_USERDICTIONALY );
	pDictionary->SetWindowTextW( m_UserDictionaly );
}


void CSettingDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{	CSliderCtrl *pSlider;
	wchar_t szVal[16];

	pSlider = ( CSliderCtrl * )pScrollBar;
	if ( pSlider == &m_Speed )
	{	voice.spd = m_Speed.GetPos();	// 話速 	50-300 default:100
		swprintf( szVal, lengthof( szVal ), L"%3d", voice.spd );
		m_SpeedVal.SetWindowText( szVal );
		return;
	}
	else if ( pSlider == &m_Ontei )
	{	voice.lmd = m_Ontei.GetPos();	// 音程１ 	0-200 default:100
		swprintf( szVal, lengthof( szVal ), L"%3d", voice.lmd );
		m_OnteiVal.SetWindowText( szVal );
		return;
	}
	else if ( pSlider == &m_Takasa )
	{	voice.pit = m_Takasa.GetPos();	// 高さ 	20-200 default:基本素片に依存
		swprintf( szVal, lengthof( szVal ), L"%3d", voice.pit );
		m_TakasaVal.SetWindowText( szVal );
		return;
	}
	else if ( pSlider == &m_Accent )
	{	voice.acc = m_Accent.GetPos();	// アクセント 0-200 default:基本素片に依存
		swprintf( szVal, lengthof( szVal ), L"%3d", voice.acc );
		m_AccentVal.SetWindowText( szVal );
		return;
	}
	else if ( pSlider == &m_Ontei2 )
	{	voice.fsc = m_Ontei2.GetPos();	// 音程２(サンプリング周波数) 50-200 default:100
		swprintf( szVal, lengthof( szVal ), L"%3d", voice.fsc );
		m_Ontei2Val.SetWindowText( szVal );
		return;
	}
	else if ( pSlider == &m_Volume )
	{	voice.vol = m_Volume.GetPos();	// 音程２(サンプリング周波数) 50-200 default:100
		swprintf( szVal, lengthof( szVal ), L"%3d", voice.vol );
		m_Volume.SetWindowText( szVal );
		return;
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CSettingDlg::InitialUpdate()
{	wchar_t szVal[16];

	m_Speed.SetRange(  50, 300, TRUE );
	m_Ontei.SetRange(   0, 200, TRUE );
	m_Takasa.SetRange( 20, 200, TRUE );
	m_Accent.SetRange(  0, 200, TRUE );
	m_Ontei2.SetRange( 50, 200, TRUE );
	m_Volume.SetRange(  0, 300, TRUE );
	m_Speed.SetTicFreq( 5 );
	m_Ontei.SetTicFreq( 5 );
	m_Takasa.SetTicFreq( 5 );
	m_Accent.SetTicFreq( 5 );
	m_Ontei2.SetTicFreq( 5 );
	m_Volume.SetTicFreq( 5 );

	m_Speed.SetPos( voice.spd );	// 話速 	50-300 default:100
	m_Ontei.SetPos( voice.lmd );	// 音程１ 	0-200 default:100
	m_Takasa.SetPos( voice.pit );	// 高さ 	20-200 default:基本素片に依存
	m_Accent.SetPos( voice.acc );	// アクセント 0-200 default:基本素片に依存
	m_Ontei2.SetPos( voice.fsc );	// 音程２(サンプリング周波数) 50-200 default:100
	m_Volume.SetPos( voice.vol );	// 音量 	0-300 default:100

	swprintf( szVal, lengthof( szVal ), L"%3d", voice.spd );
	m_SpeedVal.SetWindowText( szVal );
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.lmd );
	m_OnteiVal.SetWindowText( szVal );
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.pit );
	m_TakasaVal.SetWindowText( szVal );
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.acc );
	m_AccentVal.SetWindowText( szVal );
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.fsc );
	m_Ontei2Val.SetWindowText( szVal );
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.vol );
	m_VolumeVal.SetWindowText( szVal );

	m_VoiceSel.AddString(( LPCTSTR )L"F1E" );
	m_VoiceSel.AddString(( LPCTSTR )L"F2E" );
	m_VoiceSel.AddString(( LPCTSTR )L"F3E" );
	m_VoiceSel.AddString(( LPCTSTR )L"M1E" );
	m_VoiceSel.AddString(( LPCTSTR )L"M2E" );
	m_VoiceSel.AddString(( LPCTSTR )L"R1E" );
	m_VoiceSel.AddString(( LPCTSTR )L"R2E" );

	m_VoiceSel.SetCurSel( voice.bas );
	m_fromCodeSet.AddString(( LPCTSTR )L"UTF-8" );
	m_fromCodeSet.SetCurSel( 0 );
	m_toCodeSet.AddString(( LPCTSTR )L"UTF-8" );;
	m_toCodeSet.SetCurSel( 0 );

	CWnd *pMeCabDir = ( CWnd * )GetDlgItem( IDC_MECABDIR );
	pMeCabDir->SetWindowTextW( szMeCabExe );
	CWnd *pDicCsv = ( CWnd * )GetDlgItem( IDC_USERDICCSV );
	pDicCsv->SetWindowTextW( szUserDicCsv );
	CWnd *pDictionary = ( CWnd * )GetDlgItem( IDC_USERDICTIONALY );
	pDictionary->SetWindowTextW( szUserDictionary );
	CWnd *pSysDic = ( CWnd * )GetDlgItem( IDC_MECABSYSDIC );
	pSysDic->SetWindowTextW( szSysDictionary );
	CWnd *pTextEditer = ( CWnd * )GetDlgItem( IDC_TEXTEDITER );
	pTextEditer->SetWindowTextW( szTextEditer );
	CWnd *pSysDicWork = ( CWnd * )GetDlgItem( IDC_SYSDICWORK );
	pSysDicWork->SetWindowTextW( szSysDicWork );
	CWnd *pEditOpt = ( CWnd * )GetDlgItem( IDC_EDTOROPT );
	pEditOpt->SetWindowTextW( szEditOpt );

}


//	CComboBox m_VoiceSel;
//enum VoiceBase { F1E =0, F2E, M1E }; // 基本素片
// プリセット声種                 bas  spd  vol  pit  acc  lmd  fsc
//const AQTK_VOICE	gVoice_F1 = { F1E, 100, 100, 100, 100, 100, 100}; // 女声 F1
//const AQTK_VOICE	gVoice_F2 = { F2E, 100, 100,  77, 150, 100, 100}; // 女声 F2
//const AQTK_VOICE	gVoice_F3 = { F1E,  80, 100, 100, 100,  61, 148}; // 女声 F3
//const AQTK_VOICE	gVoice_M1 = { M1E, 100, 100,  30, 100, 100, 100}; // 男声 M1
//const AQTK_VOICE	gVoice_M2 = { M1E, 105, 100,  45, 130, 120, 100}; // 男声 M2
//const AQTK_VOICE	gVoice_R1 = { M1E, 100, 100,  30,  20, 190, 100}; // ロボット R1
//const AQTK_VOICE	gVoice_R2 = { F2E,  70, 100,  50,  50,  50, 180}; // ロボット R2


void CSettingDlg::OnSelchangeVoicesel()
{	static const AQTK_VOICE *pBaseVoice[] =
	{	&gVoice_F1, &gVoice_F2, &gVoice_F3,
		&gVoice_M1, &gVoice_M2,
		&gVoice_R1, &gVoice_R2
	};
	wchar_t szVal[16];

	voice.bas = m_VoiceSel.GetCurSel();
	m_Speed.SetPos( voice.spd = pBaseVoice[voice.bas]->spd );	// 話速 	50-300 default:100
	m_Ontei.SetPos( voice.lmd = pBaseVoice[voice.bas]->lmd );	// 音程１ 	0-200 default:100
	m_Takasa.SetPos( voice.pit = pBaseVoice[voice.bas]->pit );	// 高さ 	20-200 default:基本素片に依存
	m_Accent.SetPos( voice.acc = pBaseVoice[voice.bas]->acc );	// アクセント 0-200 default:基本素片に依存
	m_Ontei2.SetPos( voice.fsc = pBaseVoice[voice.bas]->fsc );	// 音程２(サンプリング周波数) 50-200 default:100
	m_Volume.SetPos( voice.vol = pBaseVoice[voice.bas]->vol );	// 音量 	0-300 default:100
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.spd );
	m_SpeedVal.SetWindowText( szVal );
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.lmd );
	m_OnteiVal.SetWindowText( szVal );
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.pit );
	m_TakasaVal.SetWindowText( szVal );
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.acc );
	m_AccentVal.SetWindowText( szVal );
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.fsc );
	m_Ontei2Val.SetWindowText( szVal );
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.vol );
	m_VolumeVal.SetWindowText( szVal );
}


void CSettingDlg::OnClickedBuilduserdicbtn()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
}


void CSettingDlg::OnClickedMecabsysdicref()
{	CFolderPickerDialog dlg( &szSysDictionary[0]);

	if ( dlg.DoModal() != IDOK )
	{	return;
	}
	CStringW fileNameW = dlg.GetPathName();
	wcscpy_s( &szSysDictionary[0], lengthof( szSysDictionary ), dlg.GetPathName());
	m_MecabSysDicDir = dlg.GetPathName();
	CWnd *pSysDic = ( CWnd * )GetDlgItem( IDC_MECABSYSDIC );
	pSysDic->SetWindowTextW( m_MecabSysDicDir );
}


void CSettingDlg::OnClickedTextediterref()
{	CFileDialog Dlg( FALSE, ( LPCTSTR )L"exe", ( LPCTSTR )&( szTextEditer[0]), OFN_HIDEREADONLY | OFN_EXTENSIONDIFFERENT | OFN_NOCHANGEDIR,
			( LPCTSTR )L"mecab (*.exe)|*.exe|All Files (*.*)|*.*||" );

	Dlg.m_ofn.lpstrFile = ( LPWSTR )&szTextEditer[0];
	if ( Dlg.DoModal() != IDOK )
	{	return;
	}
	wcscpy_s( &szTextEditer[0], lengthof( szTextEditer ), Dlg.m_ofn.lpstrFile );
	m_textEditer = Dlg.m_ofn.lpstrFile;
	CWnd *pTextEditer = ( CWnd * )GetDlgItem( IDC_TEXTEDITER );
	pTextEditer->SetWindowTextW( m_textEditer );
}


void CSettingDlg::OnClickedRunediterbtn()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
}


void CSettingDlg::OnClickedRefusercsv()
{	CFileDialog Dlg( FALSE, ( LPCTSTR )L"csv", ( LPCTSTR )L"user.csv", OFN_HIDEREADONLY | OFN_EXTENSIONDIFFERENT | OFN_NOCHANGEDIR,
			( LPCTSTR )L"mecab (*.csv)|*.csv|All Files (*.*)|*.*||" );

	Dlg.m_ofn.lpstrFile = ( LPWSTR )&szUserDicCsv[0];
	if ( Dlg.DoModal() != IDOK )
	{	return;
	}
	wcscpy_s( &szUserDicCsv[0], lengthof( szUserDicCsv ), Dlg.m_ofn.lpstrFile );
	m_UserDicCsv = Dlg.m_ofn.lpstrFile;
	CWnd *pDictionary = ( CWnd * )GetDlgItem( IDC_USERDICCSV );
	pDictionary->SetWindowTextW( m_UserDicCsv );
}


void CSettingDlg::OnBnClickedOk()
{	CWnd *pDicCsv = ( CWnd * )GetDlgItem( IDC_USERDICCSV );
	CWnd *pTextEditer = ( CWnd * )GetDlgItem( IDC_TEXTEDITER );
	CWnd *pSysDic = ( CWnd * )GetDlgItem( IDC_MECABSYSDIC );
	CWnd *pDictionary = ( CWnd * )GetDlgItem( IDC_USERDICTIONALY );

	pDicCsv->GetWindowTextW( &szUserDicCsv[0], lengthof( szUserDicCsv ));
	pTextEditer->GetWindowTextW( &szTextEditer[0], lengthof( szTextEditer ));
	pSysDic->GetWindowTextW( &szSysDictionary[0], lengthof( szSysDictionary ));
	pDictionary->GetWindowTextW( &szUserDictionary[0], lengthof( szUserDictionary ));

	CDialog::OnOK();
}


void CSettingDlg::OnKillfocusTextediter()
{	CWnd *pTextEditer = ( CWnd * )GetDlgItem( IDC_TEXTEDITER );

	pTextEditer->GetWindowTextW( &szTextEditer[0], lengthof( szTextEditer ));
}


void CSettingDlg::OnKillfocusUserdiccsv()
{	CWnd *pDicCsv = ( CWnd * )GetDlgItem( IDC_USERDICCSV );

	pDicCsv->GetWindowTextW( &szUserDicCsv[0], lengthof( szUserDicCsv ));
}


void CSettingDlg::OnKillfocusUserdictionaly()
{	CWnd *pDictionary = ( CWnd * )GetDlgItem( IDC_USERDICTIONALY );

	pDictionary->GetWindowTextW( &szUserDictionary[0], lengthof( szUserDictionary ));
}


void CSettingDlg::OnKillfocusMecabsysdic()
{	CWnd *pSysDic = ( CWnd * )GetDlgItem( IDC_MECABSYSDIC );

	pSysDic->GetWindowTextW( &szSysDictionary[0], lengthof( szSysDictionary ));
}


void CSettingDlg::OnKillfocusMecabdir()
{	CWnd *pMeCab = ( CWnd * )GetDlgItem( IDC_MECABDIR );

	pMeCab->GetWindowTextW( &szMeCabExe[0], lengthof( szMeCabExe ));
}


void CSettingDlg::OnKillfocusSysDicWork()
{	CWnd *pSysDicWork = ( CWnd * )GetDlgItem( IDC_SYSDICWORK );

	pSysDicWork->GetWindowTextW( &szSysDicWork[0], lengthof( szSysDicWork ));
}


void CSettingDlg::OnKillfocusEditOpt()
{	CWnd *pEditOpt = ( CWnd * )GetDlgItem( IDC_EDTOROPT );

	pEditOpt->GetWindowTextW( &szEditOpt[0], lengthof( szEditOpt ));
}


void CSettingDlg::OnClickedsysdicworkref()
{	CFolderPickerDialog dlg( &szSysDicWork[0]);

	if ( dlg.DoModal() != IDOK )
	{	return;
	}
	CStringW fileNameW = dlg.GetPathName();
	wcscpy_s( &szSysDicWork[0], lengthof( szSysDicWork ), dlg.GetPathName());
	m_SysDicWork = dlg.GetPathName();
	CWnd *pSysDicWrk = ( CWnd * )GetDlgItem( IDC_SYSDICWORK );
	pSysDicWrk->SetWindowTextW( m_SysDicWork );
}

