// pdf2txt.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

//#include <afxwin.h>
#include "stdafx.h"
#include "PdfRdTxt.h"
#include <mlang.h>
#include <io.h>
#include <wchar.h>
#include <fcntl.h>


#pragma comment(lib, "comctl32.lib")
//#pragma comment(lib, "Mlang.lib")
PdfReadText	PdfRead;
void UniToUTF8( char *pszUni, char *pszUtf8, int lenUtf8 );


static int nCodePage = 0;



int CodePageToUni( UINT codepage, char *pszCPstr, int lenCPstr, char *pszUni, int lenUni )
{	IMultiLanguage2 *pIML2;		// IMultiLanguage オブジェクトを作成	暫定　毎度クリエイトするのは無駄 
	
	CoInitialize( NULL );
//	HRESULT hr = CoCreateInstance( CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &pIML2 ));
	HRESULT hr = CoCreateInstance( CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, IID_IMultiLanguage2, ( void ** )( &pIML2 ));
	if ( !SUCCEEDED( hr ))
	{	CoUninitialize();
		return ( -1 );
	}
	UINT pcSrcSize = lenCPstr;	//( lstrlen(( LPCTSTR )pszCPstr ) + 1 ) * sizeof( TCHAR );		// バイト数で格納するバッファへのポインタ
	UINT pcDestSize = lenUni;														// 文字列のバイト数
																					// コードページから Unicode 文字列に変換
	pIML2->ConvertStringToUnicode( 0, codepage, ( CHAR * )pszCPstr, &pcSrcSize, ( WCHAR * )pszUni, &pcDestSize );
	pIML2->Release();
	CoUninitialize();
	*((( WCHAR * )pszUni ) + pcDestSize ) = L'\0';
	return (( pcDestSize <= 0 )? 0: pcDestSize );
}


#if 0
void UniToUTF8( char *pszUni, char *pszUtf8, int lenUtf8 )
{
#if 0
	IMultiLanguage2 *pIML2;		// IMultiLanguage オブジェクトを作成	暫定　毎度クリエイトするのは無駄 
	
	CoInitialize( NULL );
//	HRESULT hr = CoCreateInstance( CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &pIML2 ));
	HRESULT hr = CoCreateInstance( CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, IID_IMultiLanguage2, ( void ** )( &pIML2 ));
	if ( !SUCCEEDED( hr ))
	{	CoUninitialize();
		return;
	}
	UINT	pcSrcSize = ( lstrlen(( LPCTSTR )pszUni ) + 1 ) * sizeof( TCHAR );		// バイト数で格納するバッファへのポインタ
	UINT	pcDestSize = lenUtf8;													// 文字列のバイト数
	DWORD	dwMode = 0;

	HRESULT Hr = pIML2->ConvertStringFromUnicode( &dwMode, CP_UTF8, ( WCHAR * )pszUni, &pcSrcSize, pszUtf8, &pcDestSize );
	pIML2->Release();
	CoUninitialize();
//  [in]      WCHAR     *pSrcStr, 
//  [in, out] UINT      *pcSrcSize,
//  [in]      __wchar_t *pDstStr,
//  [in, out] UINT      *pcDstSize
//);
#else
	//	wchar_t bufUnicode[MAX_LINE];
	int iLenUnicode;

//	{	CStringW tmp;
//		tmp = pszUni;
//		iLenUnicode = tmp.GetLength();	// * sizeof( TCHAR );
//	}
	iLenUnicode = ( int )wcslen(( const wchar_t* )pszUni );	// * sizeof( TCHAR );
//	iLenUnicode = wcslen(( const wchar_t* )pszUni ) / sizeof( wchar_t );	// * sizeof( TCHAR );
//		次に、UniocdeからUTF8に変換する	// サイズを計算する
	int iLenUtf8 = WideCharToMultiByte( CP_UTF8, 0, ( wchar_t * )pszUni, iLenUnicode, NULL, 0, NULL, NULL );
	if ( iLenUtf8 <= lenUtf8 )
	{	WideCharToMultiByte( CP_UTF8, 0, ( wchar_t * )pszUni, iLenUnicode, pszUtf8, lenUtf8, NULL, NULL );
		*( pszUtf8 + iLenUtf8 ) = '\0';
	}
#endif
}
#endif


void initializeCodePage()
{	nCodePage = 0;
}

#define currentCodePage()	analizCodePage( NULL, 0 )

int analizCodePage( char *pBuf, int cbBuf )
{
	if (( nCodePage == 0 ) &&		// ( seekPos == 0 )	// 初回の読み込み？ 
				( pBuf != NULL ))
	{	CoInitialize( NULL );	// 文字コード判定 
		IMultiLanguage2 *pIMulLang;
		if ( CoCreateInstance( CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER,
			IID_IMultiLanguage2, ( void** )&pIMulLang ) == S_OK )
		{	int					detectEncCount = 1, cbBufcpy = cbBuf;
			DetectEncodingInfo	detectEnc;

			// DetectInputCodepage関数の場合
			//   byte *data    - 調べたいデータ
			//   int   dataLen - 調べたいデータのバイト数を渡し、
			//                   処理されたバイト数を受け取る
			if ( pIMulLang->DetectInputCodepage(
				MLDETECTCP_NONE, 0, pBuf, &cbBufcpy,
					&detectEnc, &detectEncCount ) == S_OK )
			{	nCodePage = detectEnc.nCodePage;
				// コードページが UNICODE 以外の場合 ANSI に変更		
				if ( nCodePage == 20127 )	// コードページが 20127(US-ASCII) の場合 UTF-8 に変更
				{	nCodePage = 65001;
				}
				else if (( nCodePage != 1200 ) &&	// UTF-16LE
						( nCodePage != 1201 ) &&	// UTF-16BE
						( nCodePage != 65001 ))		// UTF-8
				{	nCodePage = 932;				// 日本語シフトJIS
			}	}
			pIMulLang->Release();
		}
		CoUninitialize();
	}
	return ( nCodePage );
}


wchar_t *exchgExt( wchar_t *szFile, wchar_t *szNewExt )
{	wchar_t drive[_MAX_DRIVE];
	wchar_t dir[_MAX_DIR * 2];
	wchar_t fname[_MAX_FNAME];
	wchar_t ext[_MAX_EXT];
	wchar_t path[_MAX_PATH * 2];

	_wsplitpath( szFile, drive, dir, fname, ext );
	_wmakepath( path, drive, dir, fname, szNewExt );
//	if ( wcslen( szFile ) == wcslen( path ));
//	{	wcscpy( szFile, path );
//	}
	return ( _wcsdup( path ));
}


void Pdf2Txt( wchar_t *szFile )
{	DWORD page, pageLen;
	wchar_t *szOutFile;
	FILE *fp;
	unsigned char ReadBuf[MAXPAGELENGTH];
	wchar_t Utf8Buf[MAXPAGELENGTH];

	initializeCodePage();	//	nCodePage = 0;
	szOutFile = exchgExt( szFile, L".txt" );
	if (( fp = _wfopen( szOutFile, L"rt" )) != NULL )
	{	swprintf( Utf8Buf, L"%s ファイルが存在します。", szOutFile );
		UniToUTF8(( char * )Utf8Buf, ( char * )ReadBuf,  sizeof( ReadBuf ));
		fprintf( stderr, ( char * )ReadBuf );
#if 0						// file read debug
		{	fclose( fp );
			return;
		}
#endif
		fclose( fp );
	}
	if (( fp = _wfopen( szOutFile, L"wt" )) == NULL )
	{	return;
	}
	PdfRead.PdfOpen( szFile );
#if 0						// file read debug
	PdfRead.repgetForwardToken(( char * )( &ReadBuf[0]));
	PdfRead.repgetPrevToken(( char * )( &ReadBuf[0]));
#else
	for ( page = 0; page < PdfRead.PageCnt; page++ )
	{	pageLen = PdfRead.PdfReadPage( page, ( char * )ReadBuf, sizeof( ReadBuf ));
		if ( page == 0 )
		{	analizCodePage(( char * )ReadBuf, pageLen );
		}
		if ( ReadBuf[0] != '\0' )
		{	UniToUTF8(( char * )ReadBuf, ( char * )Utf8Buf,  sizeof( Utf8Buf ));
			fprintf( fp,  "%s\n", Utf8Buf );
	}	}
#endif
	PdfRead.PdfClose();
	fclose( fp );
	free( szOutFile );
}



int _tmain(int argc, _TCHAR* argv[])
{
	SetConsoleCP( 65001 );
	SetConsoleOutputCP( 65001 );
//	SetConsoleCP( UTF-16LE );
//	SetConsoleOutputCP( CP_WINUNICODE );
//	SetConsoleOutputCP( 1200 );
//	_setmode( _fileno(stdout), _O_U16TEXT );
//	Pdf2Txt( L"D:\\Mytool\\転生したらスライムだった件\\N2791GD 聖女の魔力は万能です　～ 番外編 ～.pdf" );
	Pdf2Txt( L"D:\\Mytool\\転生したらスライムだった件\\N7078HE 僕を聖女と呼ばないで！.pdf" );
	return 0;
}

