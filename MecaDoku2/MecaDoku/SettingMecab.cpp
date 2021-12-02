// SettingMecab.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MecaDoku.h"
#include "SettingMecab.h"
#include "afxdialogex.h"

extern wchar_t	szMeCabExe[_MAX_PATH];
extern wchar_t	szUserDicCsv[_MAX_PATH];
extern wchar_t	szUserDictionary[_MAX_PATH];
extern wchar_t	szSysDictionary[_MAX_PATH];
extern wchar_t	szSysDicWork[_MAX_PATH];
extern wchar_t	szTextEditer[_MAX_PATH];
extern wchar_t	szEditOpt[64];
extern wchar_t	szCharSetUserCsv[32];
extern wchar_t	szCharSetUserDic[32];


// CSettingMecab ダイアログ

IMPLEMENT_DYNAMIC(CSettingMecab, CDialogEx)

CSettingMecab::CSettingMecab(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettingMecab::IDD, pParent)
{

}

CSettingMecab::~CSettingMecab()
{
}

void CSettingMecab::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MECAB_DIR, m_MeCabDir);
	DDX_Control(pDX, IDC_FROMCODESETCMB, m_fromCodeSet);
	DDX_Control(pDX, IDC_EDTOR_OPT, m_EditorOption);
	DDX_Control(pDX, IDC_MECAB_SYSDIC, m_MecabSysDicDir);
	DDX_Control(pDX, IDC_SYSDIC_WORK, m_SysDicWork);
	DDX_Control(pDX, IDC_TEXT_EDITER, m_textEditer);
	DDX_Control(pDX, IDC_TOCODESETCMB, m_toCodeSet);
	DDX_Control(pDX, IDC_USER_DICTIONALY, m_UserDictionaly);
	DDX_Control(pDX, IDC_USERDIC_CSV, m_UserDicCsv);
}


BEGIN_MESSAGE_MAP(CSettingMecab, CDialogEx)
	ON_EN_KILLFOCUS(IDC_MECAB_DIR, &CSettingMecab::OnKillfocusMecabDir)
	ON_EN_KILLFOCUS(IDC_MECAB_SYSDIC, &CSettingMecab::OnKillfocusMecabSysdic)
	ON_EN_KILLFOCUS(IDC_USER_DICTIONALY, &CSettingMecab::OnKillfocusUserDictionaly)
	ON_EN_KILLFOCUS(IDC_USERDIC_CSV, &CSettingMecab::OnKillfocusUserdicCsv)
	ON_EN_KILLFOCUS(IDC_SYSDIC_WORK, &CSettingMecab::OnKillfocusSysdicWork)
	ON_EN_KILLFOCUS(IDC_TEXT_EDITER, &CSettingMecab::OnKillfocusTextEditer)
	ON_EN_KILLFOCUS(IDC_EDTOR_OPT, &CSettingMecab::OnKillfocusEdtorOpt)
	ON_BN_CLICKED(IDC_MECABREF, &CSettingMecab::OnClickedMecabref)
	ON_BN_CLICKED(IDC_MECABSYSDICREF, &CSettingMecab::OnClickedMecabsysdicref)
	ON_BN_CLICKED(IDC_REFUSER, &CSettingMecab::OnClickedRefuser)
	ON_BN_CLICKED(IDC_REFUSERCSV, &CSettingMecab::OnClickedRefusercsv)
	ON_BN_CLICKED(IDC_SYSDICWORKREF, &CSettingMecab::OnClickedSysdicworkref)
	ON_BN_CLICKED(IDC_TEXTEDITERREF, &CSettingMecab::OnClickedTextediterref)
	ON_CBN_SELCHANGE(IDC_FROMCODESETCMB, &CSettingMecab::OnSelchangeFromcodesetcmb)
	ON_CBN_SELCHANGE(IDC_TOCODESETCMB, &CSettingMecab::OnSelchangeTocodesetcmb)
END_MESSAGE_MAP()


void CSettingMecab::InitialUpdate()
{	wchar_t szVal[16];

	m_fromCodeSet.AddString(( LPCTSTR )L"UTF-8" );
	m_fromCodeSet.SetCurSel( 0 );
	m_toCodeSet.AddString(( LPCTSTR )L"UTF-8" );;
	m_toCodeSet.SetCurSel( 0 );
//	CComboBox m_fromCodeSet;
//	CComboBox m_toCodeSet;
	//CEdit m_MeCabDir;
	//CEdit m_EditorOption;
	//CEdit m_MecabSysDicDir;
	//CEdit m_SysDicWork;
	//CEdit m_textEditer;
	//CEdit m_UserDictionaly;
	//CEdit m_UserDicCsv;

	m_MeCabDir.SetWindowTextW( szMeCabExe );
	m_UserDicCsv.SetWindowTextW( szUserDicCsv );
	m_UserDictionaly.SetWindowTextW( szUserDictionary );
	m_MecabSysDicDir.SetWindowTextW( szSysDictionary );
	m_textEditer.SetWindowTextW( szTextEditer );
	m_SysDicWork.SetWindowTextW( szSysDicWork );
	m_EditorOption.SetWindowTextW( szEditOpt );

}


// CSettingMecab メッセージ ハンドラー


void CSettingMecab::OnKillfocusMecabDir()
{	m_MeCabDir.GetWindowTextW( &szMeCabExe[0], lengthof( szMeCabExe ));
}


void CSettingMecab::OnKillfocusMecabSysdic()
{	m_MecabSysDicDir.GetWindowTextW( &szSysDictionary[0], lengthof( szSysDictionary ));
}


void CSettingMecab::OnKillfocusUserDictionaly()
{	m_UserDictionaly.GetWindowTextW( &szUserDictionary[0], lengthof( szUserDictionary ));
}


void CSettingMecab::OnKillfocusUserdicCsv()
{	m_UserDicCsv.GetWindowTextW( &szUserDicCsv[0], lengthof( szUserDicCsv ));
}


void CSettingMecab::OnKillfocusSysdicWork()
{	m_SysDicWork.GetWindowTextW( &szSysDicWork[0], lengthof( szSysDicWork ));
}


void CSettingMecab::OnKillfocusTextEditer()
{	m_textEditer.GetWindowTextW( &szTextEditer[0], lengthof( szTextEditer ));
}


void CSettingMecab::OnKillfocusEdtorOpt()
{	m_EditorOption.GetWindowTextW( &szEditOpt[0], lengthof( szEditOpt ));
}


void CSettingMecab::OnClickedMecabref()
{	CFileDialog Dlg( TRUE, ( LPCTSTR )L"mecab", ( LPCTSTR )L"mecab.exe", OFN_HIDEREADONLY | OFN_EXTENSIONDIFFERENT | OFN_NOCHANGEDIR,
			( LPCTSTR )L"mecab (*.exe)|*.exe|All Files (*.*)|*.*||" );

	Dlg.m_ofn.lpstrFile = ( LPWSTR )&szMeCabExe[0];
	if ( Dlg.DoModal() != IDOK )
	{	return;
	}
	wcscpy_s( &szMeCabExe[0], lengthof( szMeCabExe ), Dlg.m_ofn.lpstrFile );
	m_MeCabDir.SetWindowTextW( Dlg.m_ofn.lpstrFile );
}


void CSettingMecab::OnClickedMecabsysdicref()
{	CFolderPickerDialog dlg( &szSysDictionary[0]);

	if ( dlg.DoModal() != IDOK )
	{	return;
	}
	CStringW fileNameW = dlg.GetPathName();
	wcscpy_s( &szSysDictionary[0], lengthof( szSysDictionary ), dlg.GetPathName());
	m_MecabSysDicDir.SetWindowTextW( fileNameW );
}


void CSettingMecab::OnClickedRefuser()
{	CFileDialog Dlg( FALSE, ( LPCTSTR )L"dic", ( LPCTSTR )L"user.dic", OFN_HIDEREADONLY | OFN_EXTENSIONDIFFERENT | OFN_NOCHANGEDIR,
			( LPCTSTR )L"mecab (*.dic)|*.dic|All Files (*.*)|*.*||" );

	Dlg.m_ofn.lpstrFile = ( LPWSTR )&szUserDictionary[0];
	if ( Dlg.DoModal() != IDOK )
	{	return;
	}
	wcscpy_s( &szUserDictionary[0], lengthof( szUserDictionary ), Dlg.m_ofn.lpstrFile );
	m_UserDictionaly.SetWindowTextW( Dlg.m_ofn.lpstrFile );
}


void CSettingMecab::OnClickedRefusercsv()
{	CFileDialog Dlg( FALSE, ( LPCTSTR )L"csv", ( LPCTSTR )L"user.csv", OFN_HIDEREADONLY | OFN_EXTENSIONDIFFERENT | OFN_NOCHANGEDIR,
			( LPCTSTR )L"mecab (*.csv)|*.csv|All Files (*.*)|*.*||" );

	Dlg.m_ofn.lpstrFile = ( LPWSTR )&szUserDicCsv[0];
	if ( Dlg.DoModal() != IDOK )
	{	return;
	}
	wcscpy_s( &szUserDicCsv[0], lengthof( szUserDicCsv ), Dlg.m_ofn.lpstrFile );
	m_UserDicCsv.SetWindowTextW( Dlg.m_ofn.lpstrFile );
}


void CSettingMecab::OnClickedSysdicworkref()
{	CFolderPickerDialog dlg( &szSysDicWork[0]);

	if ( dlg.DoModal() != IDOK )
	{	return;
	}
	CStringW fileNameW = dlg.GetPathName();
	wcscpy_s( &szSysDicWork[0], lengthof( szSysDicWork ), dlg.GetPathName());
	m_SysDicWork.SetWindowTextW( dlg.GetPathName());
}


void CSettingMecab::OnClickedTextediterref()
{	CFileDialog Dlg( FALSE, ( LPCTSTR )L"exe", ( LPCTSTR )&( szTextEditer[0]), OFN_HIDEREADONLY | OFN_EXTENSIONDIFFERENT | OFN_NOCHANGEDIR,
			( LPCTSTR )L"mecab (*.exe)|*.exe|All Files (*.*)|*.*||" );

	Dlg.m_ofn.lpstrFile = ( LPWSTR )&szTextEditer[0];
	if ( Dlg.DoModal() != IDOK )
	{	return;
	}
	wcscpy_s( &szTextEditer[0], lengthof( szTextEditer ), Dlg.m_ofn.lpstrFile );
	m_textEditer.SetWindowTextW( Dlg.m_ofn.lpstrFile );
}


void CSettingMecab::OnSelchangeFromcodesetcmb()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
}


void CSettingMecab::OnSelchangeTocodesetcmb()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
}
