 // SarchCsv.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MecaDoku.h"
#include "MecaDokuDlg.h"
#include "SarchCsv.h"
#include "afxdialogex.h"
#include "io.h"
#include "mecabdll.h"

extern wchar_t *exchgFileName( wchar_t *lpszPath, wchar_t *lpszName, wchar_t *lpszExt );
extern DWORD execProcess( _TCHAR *processPath, _TCHAR *processOption, _TCHAR *execFolder, DWORD *pdwExitCode, CRichEditCtrl *pRichEditCtrl = NULL );
extern wchar_t szMeCabExe[_MAX_PATH];
extern wchar_t	szSysDictionary[_MAX_PATH];
extern wchar_t	szSysDicWork[_MAX_PATH];
extern wchar_t	szTextEditor[_MAX_PATH];
extern wchar_t	szEditOpt[64];
extern void UniToUTF8( char *pszUni, char *pszUtf8, int lenUtf8 );
extern void UTF8ToUni( char *pszUtf8, char *pszUni, int lenUni );
extern enum PLAYSTATE	iPlayState;
extern BOOL	fgBgBlack;


wchar_t *exchgFileName( wchar_t *pszDest, wchar_t *pszPath, wchar_t *pszfileName )
{	wchar_t	szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFname[_MAX_FNAME], szExt[_MAX_EXT];
	errno_t errno;

	errno = _wsplitpath_s( pszPath, szDrive, lengthof( szDrive ), 
		szDir, lengthof( szDir ), szFname, lengthof( szFname ), szExt, lengthof( szExt ));
	_wmakepath( pszDest, szDrive, szDir, pszfileName, L"" );
	return ( pszDest );
}


// CSarchCsv ダイアログ

IMPLEMENT_DYNAMIC(CSarchCsv, CDialogEx)

CSarchCsv::CSarchCsv(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSarchCsv::IDD, pParent)
{

}

CSarchCsv::~CSarchCsv()
{
}

void CSarchCsv::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OUTPUT2, m_Output);
	DDX_Control(pDX, IDC_SARCHCSV, m_SarchBtn);
	DDX_Control(pDX, IDC_SARCHSTR, m_SarchStr);
	DDX_Control(pDX, IDC_BUILDSYSDIC, m_BuildSysDic);
}


BEGIN_MESSAGE_MAP(CSarchCsv, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_SARCHCSV, &CSarchCsv::OnClickedSarchcsv)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_COPY, &CSarchCsv::OnCopy)
	ON_COMMAND(ID_DEL, &CSarchCsv::OnDel)
	ON_COMMAND(ID_PASTE, &CSarchCsv::OnPaste)
	ON_COMMAND(ID_JUMPEDITOR, &CSarchCsv::OnJumpeditor)
	ON_COMMAND(ID__ALLCLEAR, &CSarchCsv::OnAllclear)
	ON_BN_CLICKED(IDC_BUILDSYSDIC, &CSarchCsv::OnClickedBuildsysdic)
	ON_COMMAND(ID_COPYW, &CSarchCsv::OnCopyw)
	ON_COMMAND(ID_DELW, &CSarchCsv::OnDelw)
	ON_COMMAND(ID_PASTEW, &CSarchCsv::OnPastew)
	ON_MESSAGE(UM_STATE_CHANGE, &CSarchCsv::OnUmStateChange)
END_MESSAGE_MAP()


void CSarchCsv::InitialUpdate()
{	CMecaDokuApp *lpApp = ( CMecaDokuApp * )AfxGetApp();

	m_Output.setBase( m_Output.normal );
	m_SarchStr.setBase( m_SarchStr.top | m_SarchStr.left );
	m_SarchBtn.setBase( m_SarchBtn.top | m_SarchStr.right );
	m_BuildSysDic.setBase( m_SarchBtn.top | m_SarchStr.right );
	m_Output.LimitText( 32768l );
#if 1
	int	hight;

	if ( fgBgBlack )							// 黒バックに白文字 
	{	COLORREF bkclr = RGB( 0, 0, 0 );
		::SendMessage( m_SarchStr.m_hWnd, EM_SETBKGNDCOLOR, ( WPARAM )0, ( LPARAM )bkclr );
		::SendMessage( m_Output.m_hWnd, EM_SETBKGNDCOLOR, ( WPARAM )0, ( LPARAM )bkclr );
	}
	if (( hight = lpApp->GetProfCharHight()) >= 30 )
	{	CHARFORMAT2 cf;

		memset(( void * )&cf, 0, sizeof( cf ));
		cf.dwMask = CFM_SIZE | CFM_FACE;
		cf.yHeight = hight;
		wcscpy( cf.szFaceName, L"Meiryo UI" );
		m_Output.SetDefaultCharFormat( cf );
		if ( fgBgBlack )							// 黒バックに白文字 
		{	COLORREF chrclr = RGB( 255, 255, 255 );
			cf.dwMask |= CFM_COLOR;
			cf.dwEffects &= ~CFE_AUTOCOLOR;
			cf.crTextColor = chrclr;
		}
		m_SarchStr.SetDefaultCharFormat( cf );
		m_Output.SetDefaultCharFormat( cf );
	}
#endif
}
        //    normal = 0x00   標準の伸縮をサポートします<BR>
        //    top    = 0x01   上を基点として上下には移動のみを行います<BR>
        //    left   = 0x02,  左を基点として左右には移動のみを行います<BR>
        //    bottom = 0x04,  下を基点として上下には移動のみを行います<BR>
        //    right  = 0x08,  右を基点として左右には移動のみを行います<BR>
        //<BR>
        //    top, bottom の何れかが指定された場合、ウィンドウは最初の高さを保持します。<BR>
        //    left, right の何れかが指定された場合、ウィンドウは最初の幅を保持します。<BR>


// CSarchCsv メッセージ ハンドラー


void CSarchCsv::OnSize(UINT nType, int cx, int cy)
{	CDialogEx::OnSize(nType, cx, cy);

	m_Output.Resize();
	m_SarchStr.Resize();
	m_SarchBtn.Resize();
	m_BuildSysDic.Resize();
}


void CSarchCsv::SetSarchStr( wchar_t *psarchstr )
{	m_SarchStr.SetSel( 0, -1 );
	m_SarchStr.ReplaceSel( psarchstr );
	OnClickedSarchcsv();
}


void CSarchCsv::CatUnnounStr( wchar_t *psarchstr )
{	wchar_t	szLineBuf[MAX_LINE];
	int		iLineNo, lineSize, isrcLen;

	if (( isrcLen = ( int )wcslen( psarchstr ) - 1 ) <= 0 )
	{	return;
	}
	for ( iLineNo = 0; iLineNo <= m_Output.GetLineCount(); iLineNo++ )
	{	lineSize = m_Output.GetLine( iLineNo, &( szLineBuf[0]), lengthof( szLineBuf ));
		if ( lineSize <= 0 )
		{	continue;
		}
		szLineBuf[lineSize] = L'\0';
		if (( wcslen( szLineBuf ) - 1 ) != isrcLen )
		{	continue;
		}
		if ( wcsncmp( psarchstr, szLineBuf, isrcLen ) == 0 )		// 既出？ 
		{	return;
	}	}
	m_Output.SetSel( -1, -1 );
	m_Output.ReplaceSel( psarchstr );
	m_Output.PostMessage( WM_VSCROLL, MAKELONG( SB_BOTTOM, NULL ));
}


void CSarchCsv::OnClickedSarchcsv()
{	int len;	//, LineIdx;
	long	nStartChar, nEndChar;
	wchar_t szSysDicDir[_MAX_PATH];
	char szSarchStr[MAX_LINE / 2];
    struct _wfinddata64_t c_file;
	intptr_t fh;

	BeginWaitCursor();
	m_SarchStr.SetSel( 0, -1 );
	m_SarchStr.GetSel( nStartChar, nEndChar );
	if (( len = nEndChar - nStartChar + 2 ) >= lengthof( szSarchStr ))
	{	return;
	}
	{	CString sStr;

		sStr = m_SarchStr.GetSelText();
		memset( szSarchStr, 0, sizeof( szSarchStr ));
		UniToUTF8(( LPSTR )sStr.GetBuffer(), ( LPSTR )szSarchStr, sizeof( szSarchStr ));
	}
//	wcscpy_s( szSysDicDir, lengthof( szSysDicDir ), L"D:\\Mytool\\mecab-0.996\\" );
///	exchgFileName( szSysDicDir, L"work\\seed\\*.csv", NULL );
//	exchgFileName( szSysDicDir, szSysDicDir, L"work\\seed\\*.csv" );
	wcscpy_s( szSysDicDir, lengthof( szSysDicDir ), szSysDicWork );
	wcscat_s( szSysDicDir, lengthof( szSysDicDir ), L"\\" );
	exchgFileName( szSysDicDir, szSysDicDir, L"seed\\*.csv" );
	if (( fh = _wfindfirst64( szSysDicDir, &c_file )) == -1L )
	{	AfxMessageBox( L".csv file not found!" );
		return;
	}
	do
	{	exchgFileName( szSysDicDir, szSysDicDir, c_file.name );
		grep( szSarchStr, szSysDicDir );
	} while ( _wfindnext64( fh, &c_file ) != -1 );
	_findclose( fh );
	DWORD orgLen = m_Output.GetLimitText();
	m_Output.LimitText( orgLen + 1024 );
	EndWaitCursor();
}


void CSarchCsv::grep( char *pszStr, wchar_t *oszfname )
{	FILE *fp;
	char	szLine[MAX_LINE];
	wchar_t	szUni[MAX_LINE];
	int len, iNoLine, lenLineNo;
	BOOL fgFound;

	if (( fp = _wfopen( oszfname, L"rt" )) == NULL )
	{	return;
	}
	len = ( int )strlen( pszStr );
	for ( fgFound = FALSE, iNoLine = 1; fgets( szLine, lengthof( szLine ), fp ) != NULL; iNoLine++ )
	{	if ( strncmp( pszStr, szLine, len ) == 0 )
		{	if ( fgFound == FALSE )
			{	swprintf( szUni, lengthof( szUni ), L"%s\n", oszfname );
				m_Output.SetSel( -1, -1 );
				m_Output.ReplaceSel( szUni );
				fgFound = TRUE;
			}
			swprintf( szUni, lengthof( szUni ), L"%d ", iNoLine );
			lenLineNo = ( int )wcslen( szUni );
			UTF8ToUni( szLine, ( char * )( &szUni[lenLineNo]), lengthof( szUni ) - lenLineNo );
			m_Output.SetSel( -1, -1 );
			m_Output.ReplaceSel( szUni );
	}	}
	fclose( fp );
}


void CSarchCsv::OnContextMenu(CWnd* /*pWnd*/, CPoint point )
{	CRect rc, rcw;
	CMenu cMenu, *menuPopup;

	if (( point.x == -1 ) && ( point.y == -1 ))
	{	LONG start, end;

		m_Output.GetSel( start, end );
		point = m_Output.PosFromChar( end );
		m_Output.ClientToScreen( &point );
	}
	// 座標がリッチエディットコントロール内の場合のみポップアップメニューを表示
	m_Output.GetClientRect( &rc );
	m_Output.ClientToScreen( &rc );
	if ( rc.PtInRect( point ))
	{	cMenu.LoadMenu( IDR_POPUP_SARCH );
		menuPopup = cMenu.GetSubMenu( 0 );
		menuPopup->TrackPopupMenu(
			TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON,
			point.x, point.y, this );
		cMenu.DestroyMenu();	// DestroyMenu( menuPopup );
		//m_p1x = point.x;
		//m_p1y = point.y;
		//m_nRE = 1;
	}
	m_SarchStr.GetClientRect( &rcw );
	m_SarchStr.ClientToScreen( &rcw );
	if ( rcw.PtInRect( point ))
	{	cMenu.LoadMenu( IDR_POPUPSARCHWORD );
		menuPopup = cMenu.GetSubMenu( 0 );
		menuPopup->TrackPopupMenu(
			TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON,
			point.x, point.y, this );
		cMenu.DestroyMenu();	// DestroyMenu( menuPopup );
		//m_p1x = point.x;
		//m_p1y = point.y;
		//m_nRE = 1;
	}
}


void CSarchCsv::OnCopy()
{	m_Output.Copy();
}


void CSarchCsv::OnDel()
{	m_Output.Cut();
}


void CSarchCsv::OnPaste()
{	m_Output.Paste();
}


void CSarchCsv::OnJumpeditor()
{	int LineNo, LineIdx, LineSize, stat;
	long	nStartChar, nEndChar;
	wchar_t selLine[MAX_LINE], option[MAX_LINE], *p, *q;

	m_Output.GetSel( nStartChar, nEndChar );
	LineIdx = m_Output.LineFromChar( nStartChar );
	m_Output.GetLine( LineIdx, &( selLine[0]), lengthof( selLine ) - 1 );
	if ( !iswdigit( selLine[0]))
	{	return;
	}									// 99999 単語,,,,,,,,,,
	LineNo = _wtoi( selLine );
	for ( LineIdx--; LineIdx >= 0; LineIdx-- )
	{	LineSize = m_Output.GetLine( LineIdx, &( selLine[0]), lengthof( selLine ) - 1 );
		if ( !iswdigit( selLine[0]))
		{	break;
	}	}
	if ( LineIdx < 0 )
	{	return;
	}
	selLine[LineSize] = L'\0';
	if (( p = wcschr( selLine, L'\r' )) != NULL )
	{	*p = L'\0';
	}
//	wcscpy_s( option, lengthof( option ) - 1, szEditOpt );
	for ( p = szEditOpt, q = option; *p != EOS; p++ )
	{	if ( *p != L'%' )
		{	*q++ = *p;
			continue;
		}
		p++;
		if ( *p == L'l' )
		{	wchar_t szLineNo[16], *r;

			swprintf( szLineNo, L"%d", LineNo );
			for ( r = szLineNo; *r != EOS; )
			{	*q++ = *r++;
		}	}
		else if ( *p == L'f' )
		{	wchar_t *r;

			for ( r = selLine; *r != EOS; )
			{	*q++ = *r++;
		}	}
		else if ( *p == L'%' )
		{	*q++ = L'%';
	}	}
	*q++ = EOS;
//	wsprintf( option, L"-n%d %s", LineNo, selLine );
	stat = ( int )ShellExecute( NULL,	// 親ウィンドウのハンドル
		_T( "open" ),					// 操作
		szTextEditor,					// 操作対象のファイル
		option,							// 操作のパラメータ
		NULL,							// 既定のディレクトリ
		SW_SHOW	);						// 表示状態
	if ( stat < 32 )
	{	AfxMessageBox( L"エディターを起動できませんでした。！" );
	}
}


void CSarchCsv::SetEditBox( wchar_t *pStr )
{	m_Output.SetSel( -1, -1 );
	m_Output.ReplaceSel( pStr );
	if ( ShowWindow( SW_SHOWNA ) == 0 )
	{
#if 0
		GetWindowRect( &rctThis );			// 画面座標
		GetWindowRect( &rctSetting );		// 画面座標
		MoveWindow( rctThis.left, rctThis.bottom,
			rctSetting.right - rctSetting.left, rctSetting.bottom - rctSetting.top, FALSE );
#endif
		ShowWindow( SW_SHOW );
	}
}


void CSarchCsv::OnAllclear()
{	m_Output.SetSel( 0, -1 );
	m_Output.Cut();
}


extern wchar_t szMeCabExe[_MAX_PATH];

void CSarchCsv::OnClickedBuildsysdic()
{	DWORD stat, exitCode;
	wchar_t szOption[MAX_LINE];
	wchar_t szExecCmd[MAX_LINE];
	wchar_t szExecFolder[MAX_LINE];
//												オプション文字列の1文字目に空白必要 
	static wchar_t *szMecabDictIndexOption1 = L" -f utf-8 -t utf-8 --model=mecab-ipadic-2.7.0-20070801U8.model";
	static wchar_t *szMecabCostTrainOption = L" -c 1.0 --old-model=mecab-ipadic-2.7.0-20070801U8.model corpus model";
	static wchar_t *szMecabDictGenOption = L" -o ..\\final -m model";
	static wchar_t *szMecabDictIndexOption2 = L" -f utf-8 -t utf-8";
	static wchar_t *szMecabTestGenOption = L" < corpus > OrgCorpus.txt";
	static wchar_t *szMecabAnalizeOption = L" -d ..\\final OrgCorpus.txt > OrgCorpus.result";
	static wchar_t *szMecabSysEvalOption = L" OrgCorpus.result corpus";

	if ( iPlayState != STOP )		// STOP状態でないとsys.dicを更新できないので 
	{	AfxMessageBox( IDS_REQUESTSTOP );
		return;
	}
	if ( AfxMessageBox( IDS_CONFARMBUILDSYSDIC, MB_OKCANCEL ) != IDOK )
	{	return;
	}
	CloseMecabDll();
//cd D:\Mytool\mecab-0.996\work\seed
//mecab-dict-index -f utf-8 -t utf-8 --model=mecab-ipadic-2.7.0-20070801U8.model
//mecab-cost-train -c 1.0 --old-model=mecab-ipadic-2.7.0-20070801U8.model corpus model
//mecab-dict-gen -o ../final -m model
//cd D:\Mytool\mecab-0.996\work\final
//mecab-dict-index -f utf-8 -t utf-8
	BeginWaitCursor();
	wcscpy_s( szExecFolder, lengthof( szExecFolder ), szSysDicWork );
	wcscat_s( szExecFolder, lengthof( szExecFolder ), L"\\seed" );
//
//	mecab-dict-index
//
	wcscpy_s( szOption, lengthof( szOption ), szMecabDictIndexOption1 );
	wcscpy_s( szExecCmd, lengthof( szExecCmd ), szMeCabExe );
	exchgFileName( szExecCmd, szExecCmd, L"mecab-dict-index.exe" );
	if (( stat = execProcess( szExecCmd, szOption, szExecFolder, &exitCode, &m_Output ))
		!= NOERROR )
	{	AfxMessageBox( IDS_ERROR_EXECCHILDPUROCESS );
		EndWaitCursor();
		return;
	}
	if ( exitCode != 0 )
	{	AfxMessageBox( IDS_ERROR_RETCODECHILDPUROCESS );
		EndWaitCursor();
		return;
	}
//
//	mecab-cost-train
//
	wcscpy_s( szOption, lengthof( szOption ), szMecabCostTrainOption );
	wcscpy_s( szExecCmd, lengthof( szExecCmd ), szMeCabExe );
	exchgFileName( szExecCmd, szExecCmd, L"mecab-cost-train.exe" );
	if (( stat = execProcess( szExecCmd, szOption, szExecFolder, &exitCode, &m_Output ))
		!= NOERROR )
	{	AfxMessageBox( IDS_ERROR_EXECCHILDPUROCESS );
		EndWaitCursor();
		return;
	}
	if ( exitCode != 0 )
	{	AfxMessageBox( IDS_ERROR_RETCODECHILDPUROCESS );
		EndWaitCursor();
		return;
	}
//
//	mecab-dict-gen
//
	wcscpy_s( szOption, lengthof( szOption ), szMecabDictGenOption );
	wcscpy_s( szExecCmd, lengthof( szExecCmd ), szMeCabExe );
	exchgFileName( szExecCmd, szExecCmd, L"mecab-dict-gen.exe" );
	if (( stat = execProcess( szExecCmd, szOption, szExecFolder, &exitCode, &m_Output ))
		!= NOERROR )
	{	AfxMessageBox( IDS_ERROR_EXECCHILDPUROCESS );
		EndWaitCursor();
		return;
	}
	if ( exitCode != 0 )
	{	AfxMessageBox( IDS_ERROR_RETCODECHILDPUROCESS );
		EndWaitCursor();
		return;
	}
	wcscpy_s( szExecFolder, lengthof( szExecFolder ), szSysDicWork );
	wcscat_s( szExecFolder, lengthof( szExecFolder ), L"\\final" );
//
//	mecab-dict-index
//
	wcscpy_s( szOption, lengthof( szOption ), szMecabDictIndexOption2 );
	wcscpy_s( szExecCmd, lengthof( szExecCmd ), szMeCabExe );
	exchgFileName( szExecCmd, szExecCmd, L"mecab-dict-index.exe" );
	if (( stat = execProcess( szExecCmd, szOption, szExecFolder, &exitCode, &m_Output ))
		!= NOERROR )
	{	AfxMessageBox( IDS_ERROR_EXECCHILDPUROCESS );
		EndWaitCursor();
		return;
	}
	if ( exitCode != 0 )
	{	AfxMessageBox( IDS_ERROR_RETCODECHILDPUROCESS );
		EndWaitCursor();
		return;
	}
#if 0
//
//	mecab-test-gen		以降追加 21/10/26 
//
	wcscpy_s( szOption, lengthof( szOption ), szMecabTestGenOption );
	wcscpy_s( szExecCmd, lengthof( szExecCmd ), szMeCabExe );
	exchgFileName( szExecCmd, szExecCmd, L"mecab-test-gen.exe" );
	if (( stat = execProcess( szExecCmd, szOption, szExecFolder, &exitCode, &m_Output ))
		!= NOERROR )
	{	AfxMessageBox( IDS_ERROR_EXECCHILDPUROCESS );
		EndWaitCursor();
		return;
	}
	if ( exitCode != 0 )
	{	AfxMessageBox( IDS_ERROR_RETCODECHILDPUROCESS );
		EndWaitCursor();
		return;
	}
//
//	mecab
//
	wcscpy_s( szOption, lengthof( szOption ), szMecabAnalizeOption );
	wcscpy_s( szExecCmd, lengthof( szExecCmd ), szMeCabExe );
	if (( stat = execProcess( szExecCmd, szOption, szExecFolder, &exitCode, &m_Output ))
		!= NOERROR )
	{	AfxMessageBox( IDS_ERROR_EXECCHILDPUROCESS );
		EndWaitCursor();
		return;
	}
	if ( exitCode != 0 )
	{	AfxMessageBox( IDS_ERROR_RETCODECHILDPUROCESS );
		EndWaitCursor();
		return;
	}
//
//	mecab-system-eval
//
	wcscpy_s( szOption, lengthof( szOption ), szMecabSysEvalOption );
	wcscpy_s( szExecCmd, lengthof( szExecCmd ), szMeCabExe );
	exchgFileName( szExecCmd, szExecCmd, L"mecab-system-eval.exe" );
	if (( stat = execProcess( szExecCmd, szOption, szExecFolder, &exitCode, &m_Output ))
		!= NOERROR )
	{	AfxMessageBox( IDS_ERROR_EXECCHILDPUROCESS );
		EndWaitCursor();
		return;
	}
	if ( exitCode != 0 )
	{	AfxMessageBox( IDS_ERROR_RETCODECHILDPUROCESS );
		EndWaitCursor();
		return;
	}
#endif
	EndWaitCursor();
}


BOOL CSarchCsv::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN )
	{	if (( pMsg->wParam == VK_RETURN ) || ( pMsg->wParam == VK_ESCAPE ))
		{	return ( FALSE );
	}	}
	else if ( pMsg->message == WM_MOUSEWHEEL )
	{	int id = FromHandle( pMsg->hwnd )->GetDlgCtrlID();	// フォーカスのコントロールID
		if ( id == IDC_OUTPUT2 )							// フォーカスがエディットコントロール 
		{	unsigned short fwKeys = GET_KEYSTATE_WPARAM( pMsg->wParam );
			if (( fwKeys & MK_CONTROL ) != 0 )
			{	CHARFORMAT2 cf;
				DWORD dwMask;
				short zDelta = GET_WHEEL_DELTA_WPARAM( pMsg->wParam ); // この値は負になることもある

				memset(( void * )&cf, 0, sizeof( cf ));
				dwMask = m_Output.GetDefaultCharFormat( cf );
				cf.yHeight += zDelta / 30;	//120;		// 120単位で変化するらしい 
				if ( cf.yHeight < 30 )
				{	cf.yHeight = 30;
				}
				cf.dwMask = CFM_SIZE;
				m_Output.SetDefaultCharFormat( cf );
				m_SarchStr.SetDefaultCharFormat( cf );
				return ( TRUE );
	}	}	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CSarchCsv::OnCancel()
{	CDialogEx::OnCancel();
}


void CSarchCsv::OnCopyw()
{	m_SarchStr.Copy();
}


void CSarchCsv::OnDelw()
{	m_SarchStr.Cut();
}


void CSarchCsv::OnPastew()
{	m_SarchStr.Paste();
}


afx_msg LRESULT CSarchCsv::OnUmStateChange( WPARAM wParam, LPARAM lParam )
{	m_BuildSysDic.EnableWindow( wParam == STOP );
	return 0;
}



