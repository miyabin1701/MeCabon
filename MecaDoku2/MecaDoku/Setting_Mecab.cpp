// Setting_Mecab.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MecaDoku.h"
#include "Setting_Mecab.h"
#include "afxdialogex.h"

extern wchar_t	szMeCabExe[_MAX_PATH];
extern wchar_t	szUserDicCsv[_MAX_PATH];
extern wchar_t	szUserDictionary[_MAX_PATH];
extern wchar_t	szSysDictionary[_MAX_PATH];
extern wchar_t	szSysDicWork[_MAX_PATH];
extern wchar_t	szTextEditor[_MAX_PATH];
extern wchar_t	szEditOpt[64];
extern wchar_t	szCharSetUserCsv[32];
extern wchar_t	szCharSetUserDic[32];


// CSetting_Mecab ダイアログ

IMPLEMENT_DYNAMIC(CSetting_Mecab, CDialogEx)

CSetting_Mecab::CSetting_Mecab(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetting_Mecab::IDD, pParent)
{

}

CSetting_Mecab::~CSetting_Mecab()
{
}

void CSetting_Mecab::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MECAB_DIR, m_MeCabDir);
	DDX_Control(pDX, IDC_MECAB_SYSDIC, m_MecabSysDicDir);
	DDX_Control(pDX, IDC_SYSDIC_WORK, m_SysDicWork);
	DDX_Control(pDX, IDC_TEXT_EDITOR, m_textEditor);
	DDX_Control(pDX, IDC_USER_DICTIONALY, m_UserDictionaly);
	DDX_Control(pDX, IDC_USERDIC_CSV, m_UserDicCsv);
	DDX_Control(pDX, IDC_EDTOR_OPT, m_EditorOption);
	DDX_Control(pDX, IDC_FROMCODESETCMB, m_fromCodeSet);
	DDX_Control(pDX, IDC_TOCODESETCMB, m_toCodeSet);
}


BEGIN_MESSAGE_MAP(CSetting_Mecab, CDialogEx)
	ON_EN_KILLFOCUS(IDC_MECAB_DIR, &CSetting_Mecab::OnKillfocusMecabDir)
	ON_EN_KILLFOCUS(IDC_MECAB_SYSDIC, &CSetting_Mecab::OnKillfocusMecabSysdic)
	ON_EN_KILLFOCUS(IDC_USER_DICTIONALY, &CSetting_Mecab::OnKillfocusUserDictionaly)
	ON_EN_KILLFOCUS(IDC_USERDIC_CSV, &CSetting_Mecab::OnKillfocusUserdicCsv)
	ON_EN_KILLFOCUS(IDC_SYSDIC_WORK, &CSetting_Mecab::OnKillfocusSysdicWork)
	ON_EN_KILLFOCUS(IDC_TEXT_EDITOR, &CSetting_Mecab::OnKillfocusTextEditor)
	ON_EN_KILLFOCUS(IDC_EDTOR_OPT, &CSetting_Mecab::OnKillfocusEdtorOpt)
	ON_BN_CLICKED(IDC_MECABREF, &CSetting_Mecab::OnClickedMecabref)
	ON_BN_CLICKED(IDC_MECABSYSDICREF, &CSetting_Mecab::OnClickedMecabsysdicref)
	ON_BN_CLICKED(IDC_REFUSER, &CSetting_Mecab::OnClickedRefuser)
	ON_BN_CLICKED(IDC_REFUSERCSV, &CSetting_Mecab::OnClickedRefusercsv)
	ON_BN_CLICKED(IDC_SYSDICWORKREF, &CSetting_Mecab::OnClickedSysdicworkref)
	ON_BN_CLICKED(IDC_TEXTEDITORREF, &CSetting_Mecab::OnClickedTexteditorref)
	ON_CBN_SELCHANGE(IDC_FROMCODESETCMB, &CSetting_Mecab::OnSelchangeFromcodesetcmb)
	ON_CBN_SELCHANGE(IDC_TOCODESETCMB, &CSetting_Mecab::OnSelchangeTocodesetcmb)
END_MESSAGE_MAP()


void CSetting_Mecab::InitialUpdate()
{	m_fromCodeSet.AddString(( LPCTSTR )L"UTF-8" );
	m_fromCodeSet.SetCurSel( 0 );
	m_toCodeSet.AddString(( LPCTSTR )L"UTF-8" );;
	m_toCodeSet.SetCurSel( 0 );
//	CComboBox m_fromCodeSet;
//	CComboBox m_toCodeSet;
	//CEdit m_MeCabDir;
	//CEdit m_EditorOption;
	//CEdit m_MecabSysDicDir;
	//CEdit m_SysDicWork;
	//CEdit m_textEditor;
	//CEdit m_UserDictionaly;
	//CEdit m_UserDicCsv;

	m_MeCabDir.SetWindowTextW( szMeCabExe );
	m_UserDicCsv.SetWindowTextW( szUserDicCsv );
	m_UserDictionaly.SetWindowTextW( szUserDictionary );
	m_MecabSysDicDir.SetWindowTextW( szSysDictionary );
	m_textEditor.SetWindowTextW( szTextEditor );
	m_SysDicWork.SetWindowTextW( szSysDicWork );
	m_EditorOption.SetWindowTextW( szEditOpt );

}


// CSetting_Mecab メッセージ ハンドラー


void CSetting_Mecab::OnKillfocusMecabDir()
{	m_MeCabDir.GetWindowTextW( &szMeCabExe[0], lengthof( szMeCabExe ));
}


void CSetting_Mecab::OnKillfocusMecabSysdic()
{	m_MecabSysDicDir.GetWindowTextW( &szSysDictionary[0], lengthof( szSysDictionary ));
}


void CSetting_Mecab::OnKillfocusUserDictionaly()
{	m_UserDictionaly.GetWindowTextW( &szUserDictionary[0], lengthof( szUserDictionary ));
}


void CSetting_Mecab::OnKillfocusUserdicCsv()
{	m_UserDicCsv.GetWindowTextW( &szUserDicCsv[0], lengthof( szUserDicCsv ));
}


void CSetting_Mecab::OnKillfocusSysdicWork()
{	m_SysDicWork.GetWindowTextW( &szSysDicWork[0], lengthof( szSysDicWork ));
}


void CSetting_Mecab::OnKillfocusTextEditor()
{	m_textEditor.GetWindowTextW( &szTextEditor[0], lengthof( szTextEditor ));
}


void CSetting_Mecab::OnKillfocusEdtorOpt()
{	m_EditorOption.GetWindowTextW( &szEditOpt[0], lengthof( szEditOpt ));
}


void CSetting_Mecab::OnClickedMecabref()
{	CFileDialog Dlg( TRUE, ( LPCTSTR )L"mecab", ( LPCTSTR )L"mecab.exe", OFN_HIDEREADONLY | OFN_EXTENSIONDIFFERENT | OFN_NOCHANGEDIR,
			( LPCTSTR )L"mecab (*.exe)|*.exe|All Files (*.*)|*.*||" );

	Dlg.m_ofn.lpstrFile = ( LPWSTR )&szMeCabExe[0];
	if ( Dlg.DoModal() != IDOK )
	{	return;
	}
	wcscpy_s( &szMeCabExe[0], lengthof( szMeCabExe ), Dlg.m_ofn.lpstrFile );
	m_MeCabDir.SetWindowTextW( Dlg.m_ofn.lpstrFile );
}


void CSetting_Mecab::OnClickedMecabsysdicref()
{	CFolderPickerDialog dlg( &szSysDictionary[0]);

	if ( dlg.DoModal() != IDOK )
	{	return;
	}
	CStringW fileNameW = dlg.GetPathName();
	wcscpy_s( &szSysDictionary[0], lengthof( szSysDictionary ), dlg.GetPathName());
	m_MecabSysDicDir.SetWindowTextW( fileNameW );
}


void CSetting_Mecab::OnClickedRefuser()
{	CFileDialog Dlg( FALSE, ( LPCTSTR )L"dic", ( LPCTSTR )L"user.dic", OFN_HIDEREADONLY | OFN_EXTENSIONDIFFERENT | OFN_NOCHANGEDIR,
			( LPCTSTR )L"mecab (*.dic)|*.dic|All Files (*.*)|*.*||" );

	Dlg.m_ofn.lpstrFile = ( LPWSTR )&szUserDictionary[0];
	if ( Dlg.DoModal() != IDOK )
	{	return;
	}
	wcscpy_s( &szUserDictionary[0], lengthof( szUserDictionary ), Dlg.m_ofn.lpstrFile );
	m_UserDictionaly.SetWindowTextW( Dlg.m_ofn.lpstrFile );
}


void CSetting_Mecab::OnClickedRefusercsv()
{	CFileDialog Dlg( FALSE, ( LPCTSTR )L"csv", ( LPCTSTR )L"user.csv", OFN_HIDEREADONLY | OFN_EXTENSIONDIFFERENT | OFN_NOCHANGEDIR,
			( LPCTSTR )L"mecab (*.csv)|*.csv|All Files (*.*)|*.*||" );

	Dlg.m_ofn.lpstrFile = ( LPWSTR )&szUserDicCsv[0];
	if ( Dlg.DoModal() != IDOK )
	{	return;
	}
	wcscpy_s( &szUserDicCsv[0], lengthof( szUserDicCsv ), Dlg.m_ofn.lpstrFile );
	m_UserDicCsv.SetWindowTextW( Dlg.m_ofn.lpstrFile );
}


void CSetting_Mecab::OnClickedSysdicworkref()
{	CFolderPickerDialog dlg( &szSysDicWork[0]);

	if ( dlg.DoModal() != IDOK )
	{	return;
	}
	CStringW fileNameW = dlg.GetPathName();
	wcscpy_s( &szSysDicWork[0], lengthof( szSysDicWork ), dlg.GetPathName());
	m_SysDicWork.SetWindowTextW( dlg.GetPathName());
}


void CSetting_Mecab::OnClickedTexteditorref()
{	CFileDialog Dlg( FALSE, ( LPCTSTR )L"exe", ( LPCTSTR )&( szTextEditor[0]), OFN_HIDEREADONLY | OFN_EXTENSIONDIFFERENT | OFN_NOCHANGEDIR,
			( LPCTSTR )L"mecab (*.exe)|*.exe|All Files (*.*)|*.*||" );

	Dlg.m_ofn.lpstrFile = ( LPWSTR )&szTextEditor[0];
	if ( Dlg.DoModal() != IDOK )
	{	return;
	}
	wcscpy_s( &szTextEditor[0], lengthof( szTextEditor ), Dlg.m_ofn.lpstrFile );
	m_textEditor.SetWindowTextW( Dlg.m_ofn.lpstrFile );
}


void CSetting_Mecab::OnSelchangeFromcodesetcmb()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
}


void CSetting_Mecab::OnSelchangeTocodesetcmb()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
}
