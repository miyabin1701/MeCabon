 
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
	BOOL loopcntinue;
	DWORD totalLen, len;
	int l, iLineCnt;
 	CRect rct;
	CPoint point0, point1;
      
	if ( pRichEditCtrl != NULL )
	{
		pRichEditCtrl->SetSel( -1, -1 );
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
	for ( utf8rem[0] = '\0', loopcntinue = TRUE; loopcntinue == TRUE; )
	{	if ( WaitForSingleObject( tProcessInfomation.hProcess, 100 ) == WAIT_OBJECT_0 )
		{	loopcntinue = FALSE;
		}
		if ( PeekNamedPipe( readPipe, NULL, 0, NULL, &totalLen, NULL ) == 0 )
		{	break;
		}
		if ( totalLen > 0 )
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
			}	}
			if ( totalLen > len )					// プロセスは終了しているがまだデータがーが残っているので終了を保留
			{	loopcntinue = TRUE;
	}	}	}
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
		{	if ( ::GetExitCodeProcess(
				tProcessInfomation.hProcess,
				pdwExitCode ) == 0 )
			{	hResult = HRESULT_FROM_WIN32( ::GetLastError());
	}	}	}
	::CloseHandle( tProcessInfomation.hProcess );	// ハンドルの解放
	::CloseHandle( tProcessInfomation.hThread );
	return( hResult );
}



