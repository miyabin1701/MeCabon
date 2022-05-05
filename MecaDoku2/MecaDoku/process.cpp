 
// process.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MecaDoku.h"
#include "afxdialogex.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <iostream>
#include <mbstring.h>

void UTF8ToUni( char *pszUtf8, char *pszUni, int lenUni );
extern wchar_t	szVoiceVoxPath[_MAX_PATH];
volatile BOOL	fgVoicevoxRuning = FALSE;


struct stMsgChk
{	PROCESS_INFORMATION *pProcInfo;
	_TCHAR *processPath;
	_TCHAR *processOption;
	_TCHAR *execFolder;
};


//
//	指定されたプロセスを実行し終了ステータスを取得する
//
DWORD execProcess( _TCHAR *processPath, _TCHAR *processOption, _TCHAR *execFolder, DWORD *pdwExitCode, CRichEditCtrl *pRichEditCtrl )
{	STARTUPINFO			tStartupInfo = { 0 };
	PROCESS_INFORMATION	tProcessInfomation = { 0 };
	HRESULT hResult = NOERROR;
	HANDLE readPipe, writePipe;		// パイプの作成
	SECURITY_ATTRIBUTES sa;
	char readBuf[1025], utf8rem[7];
	wchar_t selLine[1025];
	DWORD stat, totalLen, len;
	int l, iLineCnt;
 	CRect rct;
	CPoint point0, point1;
      
	if ( pRichEditCtrl != NULL )
	{	pRichEditCtrl->SetSel( -1, -1 );
		pRichEditCtrl->ReplaceSel(( LPCTSTR )execFolder );
		pRichEditCtrl->SetSel( -1, -1 );
		pRichEditCtrl->ReplaceSel(( LPCTSTR )L"\n" );
		pRichEditCtrl->SetSel( -1, -1 );
		pRichEditCtrl->ReplaceSel(( LPCTSTR )processPath );
		pRichEditCtrl->SetSel( -1, -1 );
		pRichEditCtrl->ReplaceSel(( LPCTSTR )L"\n" );
		pRichEditCtrl->SetSel( -1, -1 );
		pRichEditCtrl->ReplaceSel(( LPCTSTR )processOption );
		pRichEditCtrl->SetSel( -1, -1 );
		pRichEditCtrl->ReplaceSel(( LPCTSTR )L"\n" );
	}
	sa.nLength = sizeof( sa );
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	if ( CreatePipe( &readPipe, &writePipe, &sa, 0 ) == 0 )
	{	return( HRESULT_FROM_WIN32( ::GetLastError()));
	}
	tStartupInfo.cb = sizeof( tStartupInfo );
	tStartupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	tStartupInfo.hStdOutput = writePipe;
	tStartupInfo.hStdError = writePipe;
	tStartupInfo.wShowWindow = SW_HIDE;
	if ( CreateProcess(								//	プロセスの起動
		processPath,		// in LPCTSTR lpApplicationName,
		processOption,		// io LPTSTR lpCommandLine,
		NULL,				// in LPSECURITY_ATTRIBUTES lpProcessAttributes,
		NULL,				// in LPSECURITY_ATTRIBUTES lpThreadAttributes,
		TRUE,				// in BOOL bInheritHandles,
		0,					// in DWORD dwCreationFlags,
		NULL,				// in LPVOID lpEnvironment,
		execFolder,			// in LPCTSTR lpCurrentDirectory,
		&tStartupInfo,		// in LPSTARTUPINFO lpStartupInfo,
		&tProcessInfomation ) == 0 )	// out LPPROCESS_INFORMATION lpProcessInformation
	{	CloseHandle( writePipe );
		CloseHandle( readPipe );
		return( HRESULT_FROM_WIN32( ::GetLastError()));
	}
	for ( utf8rem[0] = '\0'; ; )
	{	stat = WaitForSingleObject( tProcessInfomation.hProcess, 100 );
		if ( PeekNamedPipe( readPipe, NULL, 0, NULL, &totalLen, NULL ) == 0 )
		{	break;
		}
		for ( ; totalLen > 0; totalLen -= len )
		{	strcpy_s( readBuf, sizeof( readBuf ) - 1, utf8rem );
			l = ( int )strlen( readBuf );
			if ( ReadFile( readPipe, &readBuf[l], sizeof( readBuf ) - l - 1, &len, NULL ) == 0 )
			{	::CloseHandle( tProcessInfomation.hProcess );	// ハンドルの解放
				::CloseHandle( tProcessInfomation.hThread );
				CloseHandle( writePipe );
				CloseHandle( readPipe );
				return( HRESULT_FROM_WIN32( ::GetLastError()));
			}
			if ( pRichEditCtrl != NULL )
			{	readBuf[len] = '\0';
				for ( l = len - 1; ; l-- )
				{	if (( readBuf[l] < 0x80 ) || ( 0xbf < readBuf[l] ))	// nbyte code
					{	break;
				}	}
				if (( 0x00 <= readBuf[l] ) && ( readBuf[l] <= 0x7f ))
				{	l++;
				}
				strcpy_s( utf8rem, sizeof( utf8rem ), &readBuf[l]);
				readBuf[l] = '\0';
				UTF8ToUni( readBuf, ( char * )selLine, lengthof( selLine ));
				pRichEditCtrl->SetSel( -1, -1 );
				pRichEditCtrl->ReplaceSel(( LPCTSTR )selLine );
				pRichEditCtrl->GetRect( &rct );
				point0 = pRichEditCtrl->PosFromChar( 0 );
				point1 = pRichEditCtrl->PosFromChar( pRichEditCtrl->LineIndex ( 1 ));
				iLineCnt = pRichEditCtrl->GetLineCount() - (( rct.bottom - rct.top ) / ( point1.y - point0.y  ));
				if ( iLineCnt > 0 )
				{	iLineCnt -= pRichEditCtrl->GetFirstVisibleLine();
					pRichEditCtrl->LineScroll( iLineCnt );
		}	}	}
		if ( stat != WAIT_TIMEOUT )
		{	break;
	}	}
	CloseHandle( writePipe );
	CloseHandle( readPipe );
#if 0
	if ( ::WaitForSingleObject(						//	プロセスの終了を待つ
		tProcessInfomation.hProcess,
		INFINITE ) == WAIT_FAILED )
	{	hResult = HRESULT_FROM_WIN32( ::GetLastError());
	}
	else											//	プロセスの終了コードを取得する
#endif
	{	if ( pdwExitCode != NULL )
		{	if ( ::GetExitCodeProcess( tProcessInfomation.hProcess, pdwExitCode ) == 0 )
			{	hResult = HRESULT_FROM_WIN32( ::GetLastError());
	}	}	}
	::CloseHandle( tProcessInfomation.hProcess );	// ハンドルの解放
	::CloseHandle( tProcessInfomation.hThread );
	return( hResult );
}


HWND hWndVOICEVOX[4] = { 0 };
wchar_t szRunExe[MAX_PATH] = { 0 };
int	ixVOICEVOX = 0;


BOOL CALLBACK WindowCallBack1( HWND hWnd, LPARAM lParam )
{	wchar_t	szClass[MAX_PATH];

	if ( GetClassName( hWnd,			//< ウィンドウまたはコントロールのハンドル
		&( szClass[0]),					//< テキストバッファ
		sizeof( szClass ) - 1 ) <= 0 )	//< コピーする最大文字数
	{	return ( TRUE );	
	}
//	if ( wcscmp( &( szClass[0]), L"CabinetWClass" ) == 0 )
	if ( wcscmp( &( szClass[0]), L"Chrome_WidgetWin_1" ) == 0 )
	{	SendMessage( hWnd, WM_GETTEXT, sizeof( szClass ) - 1, ( LPARAM )&( szClass[0]));	// キャプション取得 
//		if ( wcscmp( &( szClass[0]), L"VOICEVOX" ) == 0 )
		if ( wcscmp( &( szClass[0]), L"voicevox" ) == 0 )
		{	if ( ixVOICEVOX < lengthof( hWndVOICEVOX ))
			{	hWndVOICEVOX[ixVOICEVOX++] = hWnd;
				return FALSE;
	}	}	}
	else if ( wcscmp( &( szClass[0]), L"ConsoleWindowClass" ) == 0 )
	{	SendMessage( hWnd, WM_GETTEXT, sizeof( szClass ) - 1, ( LPARAM )&( szClass[0]));	// キャプション取得 
		// GetModuleFileNameA( hModule, &( szClass[0]), sizeof( szClass ) - 1 );
		if ( wcscmp( &( szClass[0]), szVoiceVoxPath ) == 0 )
		{	// if ( ixRunExe < lengthof( hWndRunExe ))
			{	// hWndRunExe[ixRunExe++] = hWnd;
				wcscpy( szRunExe, &( szClass[0]));
				return FALSE;
	}	}	}
	return TRUE;
}


UINT messageChechThread( LPVOID Patram )
{	struct stMsgChk *pMsgChk = ( struct stMsgChk * )Patram;
	STARTUPINFO			tStartupInfo = { 0 };
//	PROCESS_INFORMATION	tProcessInfomation = { 0 };
	SECURITY_ATTRIBUTES sa;
	HANDLE msgreadPipe, msgwritePipe;		// パイプの作成
	DWORD stat, totalLen, len;
	char readBuf[1025];

	sa.nLength = sizeof( sa );
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	if ( CreatePipe( &msgreadPipe, &msgwritePipe, &sa, 0 ) == 0 )
	{	return( HRESULT_FROM_WIN32( ::GetLastError()));
	}
	tStartupInfo.cb = sizeof( tStartupInfo );
	tStartupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	tStartupInfo.hStdOutput = msgwritePipe;
	tStartupInfo.hStdError = msgwritePipe;
	tStartupInfo.wShowWindow = SW_HIDE;

	if ( CreateProcess(				//	プロセスの起動
		pMsgChk->processPath,		// in LPCTSTR lpApplicationName,
		pMsgChk->processOption,		// io LPTSTR lpCommandLine,
		NULL,						// in LPSECURITY_ATTRIBUTES lpProcessAttributes,
		NULL,						// in LPSECURITY_ATTRIBUTES lpThreadAttributes,
		TRUE,						// in BOOL bInheritHandles,
		0,							// in DWORD dwCreationFlags,
		NULL,						// in LPVOID lpEnvironment,
		pMsgChk->execFolder,		// in LPCTSTR lpCurrentDirectory,
		&tStartupInfo,				// in LPSTARTUPINFO lpStartupInfo,
		pMsgChk->pProcInfo ) == 0 )	// out LPPROCESS_INFORMATION lpProcessInformation
	{	return( HRESULT_FROM_WIN32( ::GetLastError()));
	}
	fgVoicevoxRuning = FALSE;
	while ( 1 )
	{	stat = WaitForSingleObject( pMsgChk->pProcInfo->hProcess, 100 );
		if ( PeekNamedPipe( msgreadPipe, NULL, 0, NULL, &totalLen, NULL ) == 0 )
		{	break;
		}
		for ( ; totalLen > 0; totalLen -= len )
		{	if ( ReadFile( msgreadPipe, &readBuf[0], sizeof( readBuf ) - 1, &len, NULL ) == FALSE )
			{	break;
			}
			if ( fgVoicevoxRuning != TRUE )
			{	//INFO:     Uvicorn running on http://127.0.0.1:50021 (Press CTRL+C to quit)
				if ( strstr( readBuf, "Uvicorn running on http://127.0.0.1:50021" ) != NULL )
				{	fgVoicevoxRuning = TRUE;
					break;
		}	}	}
		if ( stat != WAIT_TIMEOUT )
		{	break;
	}	}
	CloseHandle( msgwritePipe );
	CloseHandle( msgreadPipe );
	return ( 0 );
}

	struct stMsgChk threadArg;

//
//	VOICEVOX の run.exe を起動する 標準出力をフックして準備完了まで待つ 
//
DWORD StartProcess( _TCHAR *processPath, _TCHAR *processOption, _TCHAR *execFolder, PROCESS_INFORMATION *pProcInfo )
{	fgVoicevoxRuning = FALSE;
	szRunExe[0] = L'\0';
	ixVOICEVOX = 0;
	EnumWindows( WindowCallBack1, ( LPARAM )pProcInfo );	// VOICEVOX起動済みかチェック 
	if (( ixVOICEVOX != 0 ) || ( szRunExe[0] != L'\0' ))	// 既に起動していたらリターン 
	{	return ( 0 );
	}
	CWaitCursor wait;

	threadArg.pProcInfo = pProcInfo;
	threadArg.execFolder = execFolder;
	threadArg.processPath = processPath;
	threadArg.processOption =processOption;
	AfxBeginThread( messageChechThread, &threadArg );
	while ( fgVoicevoxRuning == FALSE )
	{	_sleep( 100 );
	}
	return ( 0 );
}


// ウィンドウハンドルを取得しアプリケーションを終了させる。
BOOL CALLBACK WindowCallBack2( HWND hWnd, LPARAM lParam )
{	PROCESS_INFORMATION* pi = ( PROCESS_INFORMATION* )lParam;
	DWORD ProcessId = 0;

	::GetWindowThreadProcessId( hWnd, &ProcessId );		// ウインドウのプロセスIDを取得 
	if ( pi->dwProcessId == ProcessId )					// プロセスIDが同じ場合、WM_CLOSEをポスト
	{	::PostMessage( hWnd, WM_CLOSE, 0, 0 );
		return FALSE;
	}
	return TRUE;
}


//
//	指定されたプロセスを終了する
//
void EndProcess( PROCESS_INFORMATION *pProcInfo )
{
	if (( ixVOICEVOX != 0 ) || ( szRunExe[0] != L'\0' ))	// 既に起動していてStartProcessでは起動していないなら 
	{	return;
	}
	EnumWindows( WindowCallBack2, ( LPARAM )pProcInfo );
	if ( ::WaitForSingleObject( pProcInfo->hProcess, 10000 ) == WAIT_TIMEOUT )
	{	TerminateProcess( pProcInfo->hProcess, 0 );
	}
	::CloseHandle( pProcInfo->hThread );
	::CloseHandle( pProcInfo->hProcess );
}


