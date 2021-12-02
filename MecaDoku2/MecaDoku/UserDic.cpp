// UserDic.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MecaDoku.h"
#include "MecaDokuDlg.h"
#include "UserDic.h"
#include "afxdialogex.h"
#include "mecab.h"
#include "mecabdll.h"

#include <stdlib.h>
#include <wchar.h>

/*
static const int idColumnHeader[] =
{	IDS_COL01, IDS_COL02, IDS_COL03, IDS_COL04, IDS_COL05,
	IDS_COL06, IDS_COL07, IDS_COL08, IDS_COL09, IDS_COL10,
	IDS_COL11, IDS_COL12, IDS_COL13, IDS_COL14, IDS_COL15,
	IDS_COL16, IDS_COL17, IDS_COL18, IDS_COL19, IDS_COL20
};
*/
//	static char *ppArgv[] = { "", "-d", "D:\\Mytool\\mecab-0.996\\dic\\NEologd", "-u", "D:\\Mytool\\mecab-0.996\\dic\\Fantasy\\fantasy.dic", 0, 0 };
//	static char *ppArgv[] = { "", "-u", "D:\\Mytool\\mecab-0.996\\dic\\Fantasy\\fantasy.dic", "-p", "-F", "%m\t%H\t%pw,%pC,%pc\n", "-E", "EOS\t%pw,%pC,%pc\n", 0 };
	static char *ppArgv[] = { "", "-u", "D:\\Mytool\\mecab-0.996\\dic\\Fantasy\\fantasy.dic", "-F", "%m\t%H\t%pw,%pC,%pc\n", "-E", "EOS\t%pw,%pC,%pc\n", 0 };
	

void UniToUTF8( char *pszUni, char *pszUtf8, int lenUtf8 );
void UTF8ToUni( char *pszUtf8, char *pszUni, int lenUni );
extern wchar_t	szUserDicCsv[_MAX_PATH];
extern wchar_t	szUserDictionary[_MAX_PATH];

static mecab_model_t *model;
static mecab_t *mecab;
static mecab_lattice_t *lattice;


static wchar_t *cvtUnicr2crlf( wchar_t *pszsrc, int lensrc )
{	size_t srclen = wcslen( pszsrc );
	wchar_t *pszsrccpy = ( wchar_t * )calloc( srclen + 8, sizeof( wchar_t ));
	wchar_t *p, *q;

	wcscpy( pszsrccpy, pszsrc );
	*pszsrc = '\0';
	for ( p = pszsrccpy; *p != '\0'; p = q + 1 )
	{	if (( q = wcschr( p, L'\n' )) != NULL )
		{	*q = '\0';
			wcscat( pszsrc, p );
			wcscat( pszsrc, L"\r\n" );
		}
	}
	free( pszsrccpy );
	return( pszsrc );
}


// CUserDic ダイアログ

IMPLEMENT_DYNAMIC(CUserDic, CDialogEx)

CUserDic::CUserDic(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUserDic::IDD, pParent)
{

}

CUserDic::~CUserDic()
{
}

void CUserDic::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//	DDX_Control(pDX, IDC_USERDICLST, m_newWord);
	DDX_Control(pDX, IDC_OUTPUTEDIT, m_OutPut);
//	DDX_Control(pDX, IDC_HINSHICMB, m_Hinshi);
	//	DDX_Control(pDX, IDC_HATUONEDIT, m_Hatuon);
	DDX_Control(pDX, IDC_SEIKICOSTEDIT, m_SeikiCost);
	//	DDX_Control(pDX, IDC_WORDEDIT, m_Word);
	//	DDX_Control(pDX, IDC_YOMIEDIT, m_Yomi);
	DDX_Control(pDX, IDC_CSVEDIT, m_CsvEdit);
	DDX_Control(pDX, IDC_HINSHI, m_HinshiCmb);
	DDX_Control(pDX, IDC_LEFTID, m_LeftId);
	DDX_Control(pDX, IDC_RIGHTID, m_RightId);
}


BEGIN_MESSAGE_MAP(CUserDic, CDialogEx)
//	ON_BN_CLICKED(IDC_WORDBTN, &CUserDic::OnClickedWordbtn)
	ON_BN_CLICKED(IDC_UPDATE, &CUserDic::OnClickedUpdate)
	ON_BN_CLICKED(IDC_BUILDBTN, &CUserDic::OnClickedBuildbtn)
//	ON_EN_CHANGE(IDC_WORDEDIT, &CUserDic::OnChangeWordedit)
//ON_WM_LBUTTONDOWN()
ON_WM_DESTROY()
ON_MESSAGE(UM_USERACTION, &CUserDic::OnUmUseraction)
ON_CBN_SELCHANGE(IDC_LEFTID, &CUserDic::OnSelchangeLeftid)
ON_CBN_EDITUPDATE(IDC_LEFTID, &CUserDic::OnEditupdateLeftid)
ON_CBN_SELCHANGE(IDC_RIGHTID, &CUserDic::OnSelchangeRightid)
ON_CBN_SELCHANGE(IDC_HINSHI, &CUserDic::OnSelchangeHinshi)
END_MESSAGE_MAP()


//
//  機能    :データファイルの存在チェク
//
//  返り値  :在り：非０　無し：０
//
//  機能説明:データファイルの存在チェク 
//
DWORD GetFileSize( wchar_t *Path )
{	struct _wfinddata64_t c_file;

	if ( _wfindfirst64( Path, &c_file ) == -1L )
	{	return ( -1 );
	}
	return ( c_file.size );
}


void cvtEucjp2Uni( char *pszEucjp, char *pszUni, int lenUni )
{    
//	まずUniocdeに変換する		// サイズを計算する
	int iLenUnicode = MultiByteToWideChar( CP_ACP, 0, pszEucjp, strlen( pszEucjp ) + 1, NULL, NULL );
	if ( iLenUnicode <= lenUni )
	{	MultiByteToWideChar( CP_ACP, 0, pszEucjp, strlen( pszEucjp ) + 1, ( LPWSTR )pszUni, lenUni );
	}
}
//20932	EUC-JP	Japanese (JIS 0208-1990 and 0212-1990)
//51932	euc-jp	EUC Japanese

wchar_t	szLeftIdDef[_MAX_PATH];
wchar_t	szRightIdDef[_MAX_PATH];
wchar_t	szPosIdDef[_MAX_PATH];
extern wchar_t	szSysDictionary[_MAX_PATH];
extern wchar_t	*szLeftIdDefFname;		// = L"left-id.def";
extern wchar_t	*szRightIdDefFname;		// = L"right-id.def";
extern wchar_t	*szPosIdDefFname;

wchar_t *exchgFileName( wchar_t *pszDest, wchar_t *pszPath, wchar_t *pszfileName )
{	wchar_t	szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFname[_MAX_FNAME], szExt[_MAX_EXT];
	errno_t errno;

	errno = _wsplitpath_s( pszPath, szDrive, lengthof( szDrive ), 
		szDir, lengthof( szDir ), szFname, lengthof( szFname ), szExt, lengthof( szExt ));
	_wmakepath( pszDest, szDrive, szDir, pszfileName, L"" );
	return ( pszDest );
}


//
//		返り値は使用後freeすること！エラー時はNULL
//
BOOL LoadLRIdDef( CComboBox *pcmbbox, wchar_t *pszIdDef )
{	wchar_t szLine[MAX_LINE], szUniLine[MAX_LINE];
	wchar_t *plf;
	FILE *csvFp;
	int size;

//	exchgFileName( szLeftIdDef, szSysDictionary, szLeftIdDefFname );
	if (( csvFp = _wfopen( pszIdDef, L"rt" )) != NULL )
	{	while ( !feof( csvFp ))
		{	if ( fgets(( char * )szLine, lengthof( szLine ), csvFp ) == NULL )
			{	continue;
			}
			cvtEucjp2Uni(( char * )szLine, ( char * )&szUniLine[0], lengthof( szUniLine ));
			if (( plf = wcschr( szUniLine, L'\n' )) != NULL )
			{	*plf = L'\0';
			}
			pcmbbox->AddString( szUniLine );
		}
		fclose( csvFp );
		return ( TRUE );
	}
	return ( FALSE );
}


// CUserDic メッセージ ハンドラー


void CUserDic::InitialUpdate()
{
	CEdit *pCsvEdit = ( CEdit * )GetDlgItem( IDC_CSVEDIT );
	CComboBox *pLefId = ( CComboBox * )GetDlgItem( IDC_LEFTID );
	CComboBox *pRightId = ( CComboBox * )GetDlgItem( IDC_RIGHTID );
	CComboBox *pPosId = ( CComboBox * )GetDlgItem( IDC_HINSHI );
	wchar_t *pszBuff, *plf;
	wchar_t szLine[MAX_LINE], szUniLine[MAX_LINE];
	FILE *csvFp;
	int i, l;
	DWORD size;
	CString ColumnTitle;
	LVITEM lvItem;

#if 0
	if ( OpenMecabDll() == NULL )
	{	return;
	}
	//	if (( model = mecab_model_new( 8, ppArgv )) == NULL )
	if (( model = fpmecab_model_new( 7, ppArgv )) == NULL )
	{	//CString errorMessage = ( LPCTSTR )MeCab::getLastError();
#if 0
		CString errorMessage = ( LPCTSTR )fpgetLastError();

		AfxMessageBox( errorMessage );
#endif
		return;
	}
	if (( mecab = fpmecab_model_new_tagger( model )) == NULL )
	{	return;
	}
	if (( lattice = fpmecab_model_new_lattice( model )) == NULL )
	{	fpmecab_destroy( mecab );
		return;
	}
#endif

#if 1
	if (( size = GetFileSize( szUserDicCsv )) > 0 )		//	ファイルサイズ取得	から　
	{	size = size / 10 * 13;							// ３０％（文字セット変換増分）？増しでバッファを確保
		if (( pszBuff = ( wchar_t * )calloc( size, sizeof( wchar_t ))) != NULL )
		{	memset( pszBuff, 0, size );
			if (( csvFp = _wfopen( szUserDicCsv, L"rt+" )) != NULL )
			{	for ( ; !feof( csvFp ); )
				{	if ( fgets(( char * )szLine, lengthof( szLine ), csvFp ) == NULL )
					{	continue;
					}
					cvtEucjp2Uni(( char * )szLine, ( char * )&szUniLine[0], lengthof( szUniLine ));
					if (( plf = wcschr( szUniLine, L'\n' )) != NULL )
					{	wcscpy( plf, L"\r\n" );
					}
					wcscat( pszBuff, szUniLine );
				}
				fclose( csvFp );
			}
			pCsvEdit->SetWindowTextW( pszBuff );
			free( pszBuff );
	}	}
	int inoLine = pCsvEdit->GetLineCount();
	pCsvEdit->LineScroll( inoLine - 6 );
	pCsvEdit->SetSel( pCsvEdit->LineIndex( inoLine - 2 ), pCsvEdit->LineIndex( inoLine - 1 ), FALSE );
	pCsvEdit->SetFocus();
	pCsvEdit->ShowCaret();
	pCsvEdit->SetModify( FALSE );
#endif
//
//
//
	LoadLRIdDef( pLefId, exchgFileName( szLeftIdDef, szSysDictionary, szLeftIdDefFname ));
	LoadLRIdDef( pRightId, exchgFileName( szRightIdDef, szSysDictionary, szRightIdDefFname ));
	LoadLRIdDef( pPosId, exchgFileName( szPosIdDef, szSysDictionary, szPosIdDefFname ));

#if 0
	m_newWord.SetExtendedStyle( m_newWord.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	for ( i = 0; i < lengthof( idColumnHeader ); i++ )
	{	ColumnTitle.LoadStringW( idColumnHeader[i]);
		m_newWord.InsertColumn( i, ColumnTitle,  LVCFMT_LEFT, 100, -1 );
//		int nCol,
//		LPCTSTR lpszColumnHeading,
//		int nFormat = 
//		int nWidth = -1,
//		int nSubItem = -1);
	}
	for ( l = 0; l < 50; l++ )
	{	lvItem.mask = LVIF_TEXT;
		lvItem.iItem = l;
		lvItem.iSubItem = 0;
		lvItem.pszText = ( LPTSTR )"";
		m_newWord.InsertItem( &lvItem );
		for ( i = 1; i < lengthof( idColumnHeader ); i++ )
		{
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = l;
			lvItem.iSubItem = i;
			lvItem.pszText = ( LPTSTR )"";
			m_newWord.SetItem( &lvItem );
		}
	}
	m_newWord.Invalidate();
#endif
}



//void CUserDic::OnClickedWordbtn()
//{
//	// TODO: ここにコントロール通知ハンドラー コードを追加します。
//}


void CUserDic::OnClickedUpdate()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
}


void CUserDic::OnClickedBuildbtn()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
}


void CUserDic::OnChangeWordedit()
{
	// TODO:  これが RICHEDIT コントロールの場合、このコントロールが
	// この通知を送信するには、CDialogEx::OnInitDialog() 関数をオーバーライドし、
	// CRichEditCtrl().SetEventMask() を
	// OR 状態の ENM_CHANGE フラグをマスクに入れて呼び出す必要があります。

//	CEdit *pTergetWord = ( CEdit * )GetDlgItem( IDC_WORDEDIT );
//	CEdit *pYomi = ( CEdit * )GetDlgItem( IDC_YOMIEDIT );
//	CEdit *pHatsuon = ( CEdit * )GetDlgItem( IDC_HATUONEDIT );
	CEdit *pSeikiCost = ( CEdit * )GetDlgItem( IDC_SEIKICOSTEDIT );
	CEdit *pOutput = ( CEdit * )GetDlgItem( IDC_OUTPUTEDIT );
	CEdit *pNewCost = ( CEdit * )GetDlgItem( IDC_SEIKICOSTEDIT );
//	CComboBox *pHinshi = ( CComboBox * )GetDlgItem( IDC_HINSHICMB );
	CString TergetWord;
	char *p;
	char szUtf8[512];
	wchar_t szUni[512], szUni2[512];
	wchar_t *pEOS1, *pEOS2, *pTAB3;
	int unilen, splitCost, wordCost, seikiCost;

//	pTergetWord->GetWindowText( TergetWord );
//	if ( onUserDic( TergetWord, &csvInfo ))
//	{	pYomi->SetWindowTextW( csvInfo.yomi );
//		pHatsuon->SetWindowTextW( csvInfo.hatsuon );
//	}
//	else
//	{	pYomi->SetWindowTextW(( LPCTSTR )"" );
//		pHatsuon->SetWindowTextW(( LPCTSTR )"" );
//	}
	memset( &( szUtf8[0]), 0, sizeof( szUtf8 ));
	wcscpy( &szUni[0], TergetWord.GetBuffer());
//	wcscat( &szUni[0], L"\r\nEOS\r\n" );
	UniToUTF8(( char * )&szUni[0], &( szUtf8[0]), sizeof( szUtf8 ));
#if 0
//	if (( model = mecab_model_new( 8, ppArgv )) == NULL )
	if (( model = fpmecab_model_new( 7, ppArgv )) == NULL )
	{	//CString errorMessage = ( LPCTSTR )MeCab::getLastError();
#if 0
		CString errorMessage = ( LPCTSTR )fpgetLastError();

		AfxMessageBox( errorMessage );
#endif
		return;
	}
	if (( mecab = fpmecab_model_new_tagger( model )) == NULL )
	{	return;
	}
	if (( lattice = fpmecab_model_new_lattice( model )) == NULL )
	{	fpmecab_destroy( mecab );
		return;
	}
#endif
	fpmecab_lattice_set_sentence( lattice, &( szUtf8[0]) );
	fpmecab_parse_lattice( mecab, lattice );
	p = ( char * )fpmecab_lattice_tostr( lattice );
//	node = fpmecab_lattice_get_bos_node( lattice );
//	node = node->next;
//	fgCont = (( node->posid != 0 ) || ( bufHiraganaJis[0] == EOS ));
//	for ( /* bufHiraganaJis[0] = EOS*/; node != NULL; node = node->next )
	UTF8ToUni( p, ( char * )( &szUni[0]), sizeof( szUni ));
	unilen = wcslen( szUni );

	memset( &( szUtf8[0]), 0, sizeof( szUtf8 ));
	wcscpy( &szUni2[0], TergetWord.GetBuffer());
	UniToUTF8(( char * )&szUni2[0], &( szUtf8[0]), sizeof( szUtf8 ));
	int utf8len = strlen( szUtf8 );
	int i;
	fpmecab_lattice_set_sentence( lattice, &( szUtf8[0]));
	fpmecab_lattice_set_boundary_constraint( lattice, 0, MECAB_TOKEN_BOUNDARY );
	for ( i = 1; i < utf8len; i++ )
	{	fpmecab_lattice_set_boundary_constraint( lattice, i, MECAB_INSIDE_TOKEN );
	}
	fpmecab_lattice_set_boundary_constraint( lattice, i, MECAB_TOKEN_BOUNDARY );
/*
	lattice->set_boundary_constraint( 0, MECAB_TOKEN_BOUNDARY );
	lattice->set_boundary_constraint( 1,  );
	lattice->set_boundary_constraint( 2, MECAB_INSIDE_TOKEN );
	lattice->set_boundary_constraint( 3, MECAB_INSIDE_TOKEN );
	lattice->set_boundary_constraint( 4, MECAB_INSIDE_TOKEN );
	lattice->set_boundary_constraint( 5, MECAB_TOKEN_BOUNDARY );
*/
	fpmecab_parse_lattice( mecab, lattice );
	p = ( char * )fpmecab_lattice_tostr( lattice );
	UTF8ToUni( p, ( char * )( &szUni[unilen]), sizeof( szUni ) - unilen );

	cvtUnicr2crlf(( &szUni[0]), sizeof( szUni ));
	pOutput->SetWindowTextW(( LPCTSTR )szUni );
	splitCost = wordCost = seikiCost = 0;
	if (( pEOS1 = wcsstr( szUni, L"EOS" )) != NULL )
	{	swscanf( pEOS1, L"EOS\t%*d,%*d,%d", &splitCost );
		if (( pEOS2 = wcsstr( pEOS1 + 1, L"EOS" )) != NULL )
		{	swscanf( pEOS2, L"EOS\t%*d,%*d,%d", &wordCost );
			if (( pTAB3 = wcsstr( pEOS1 + 1, L"\t" )) != NULL )
			{	if (( pTAB3 = wcsstr( pTAB3 + 1, L"\t" )) != NULL )
				{	if (( pTAB3 = wcsstr( pTAB3 + 1, L"\t" )) != NULL )
					{	swscanf( pTAB3, L"%d,%*d,%*d", &seikiCost );
		}	}	}	}
		seikiCost -= wordCost - splitCost + 1;
/*
二      名詞,数,*,*,*,*,二,ニ,ニ        2914,570,3484
人      名詞,接尾,助数詞,*,*,*,人,ニン,ニン     8672,-10731,1425
EOS     0,28,1453
二人    名詞,一般,*,*,*,*,二人,フタリ,フタリ    8962,-283,8679
EOS     0,-573,8106
*/
	}
	sprintf( szUtf8, "%d", seikiCost );
	UTF8ToUni( szUtf8, ( char * )( &szUni[0]), sizeof( szUni ));
	pNewCost->SetWindowTextW(( LPCTSTR )szUni );
//	pOutput->SetWindowTextW( L"二	名詞,数,*,*,*,*,二,ニ,ニ	2914,570,3484\r\n人	名詞,接尾,助数詞,*,*,*,人,ニン,ニン	8672,-10731,1425\r\nEOS	0,28,1453\r\n" );
}
//const wchar_t	lpszMeCabPath[]		= L"mecab_exe";


//表層形,左文脈ID,右文脈ID,コスト,品詞,品詞細分類1,品詞細分類2,品詞細分類3,活用形,活用型,原形,読み,発音


BOOL CUserDic::onUserDic( CString TergetWord, struct CsvDicInfo *pcsvInfo )
{	CMecaDokuApp *lpApp = ( CMecaDokuApp * )AfxGetApp();
	FILE *fpUserCsv;
	wchar_t szCsvUni[512];
	char szCsvLine[512];
	int iTergetLen, iLine;
//	CString sUserDic;

//	sUserDic = lpApp->szUserDictionary;
//	if (( fpUserDic = _wfopen(( &szUserDictionary[0]), L"rt" )) == NULL )
//	if (( fpUserDic = _wfopen( L"D:\\Mytool\\mecab-0.996\\mecab-ipadic-2.7.0-20070801\\mecab-ipadic-2.7.0-20070801.model\\Gekokujyou.csv", L"rt" )) == NULL )
	if (( fpUserCsv = fopen( "D:\\Mytool\\mecab-0.996\\mecab-ipadic-2.7.0-20070801\\mecab-ipadic-2.7.0-20070801.model\\Gekokujyou.csv", "rt" )) == NULL )
	{	return ( FALSE );
	}
	iTergetLen = wcslen( TergetWord );
//	iTergetLen = strlen(( char * )TergetWord.GetBuffer());
	for ( iLine = 0; !feof( fpUserCsv ); iLine++ )
//	{	if ( fgetws( szCsvLine, lengthof( szCsvLine ), fpUserDic ) == NULL )
	{	if ( fgets( szCsvLine, sizeof( szCsvLine ), fpUserCsv ) == NULL )
		{	continue;
		}
		UTF8ToUni( szCsvLine, ( char * )&szCsvUni[0], lengthof( szCsvUni ));

		if ( wcsncmp( szCsvUni, TergetWord, iTergetLen ) == 0 )
		{	wchar_t *szcolm[16] = { 0 };
			wchar_t *p;
			int i;
			CString canma( "," );


			pcsvInfo->LineText = szCsvLine;
			pcsvInfo->iLineNo = iLine;
			fclose( fpUserCsv );
			for ( szcolm[0] = p = szCsvUni, i = 1; *p != '\0' && i < lengthof( szcolm ); )
			{	if (( p = wcschr( p, ',' )) == NULL )
				{	break;
				}
				*p = '\0';
				szcolm[i++] = ++p;
			}
//表層形,左文脈ID,右文脈ID,コスト,品詞,品詞細分類1,品詞細分類2,品詞細分類3,活用形,活用型,原形,読み,発音
			pcsvInfo->Hyosou = szcolm[0];
			pcsvInfo->lId = _wtoi( szcolm[1]);
			pcsvInfo->rId = _wtoi( szcolm[2]);
			pcsvInfo->cost = _wtoi( szcolm[3]);
			pcsvInfo->Hinshi = szcolm[4] + canma + szcolm[5] + canma + szcolm[6] + canma + szcolm[7];
			pcsvInfo->Katsuyou = szcolm[8] + canma + szcolm[9];
			pcsvInfo->Genkei = szcolm[10];
			pcsvInfo->yomi = szcolm[11];
			pcsvInfo->hatsuon = szcolm[12];
			return ( TRUE );
		}
	}
	fclose( fpUserCsv );
	return ( FALSE );
//wchar_t	szCharSetUserCsv[32] = "UTF-8";
//wchar_t	szCharSetUserDic[32] = "UTF-8";
//		wcscpy( szUserDictionary, ( LPCTSTR )GetProfileString( lpszEnvSection, lpszUserDic, L"user.dic" ));
}



BOOL CUserDic::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_DRAWITEM )
	{	int id = FromHandle( pMsg->hwnd )->GetDlgCtrlID();	// フォーカスのコントロールID
		if ( id == IDC_CSVEDIT )							// フォーカスがエディットコントロール 
		{	CEdit *pCsvEdit = ( CEdit * )GetDlgItem( IDC_CSVEDIT );

			CDialogEx::PreTranslateMessage(pMsg);
			int inoLine = pCsvEdit->GetLineCount();
			pCsvEdit->LineScroll( inoLine - 6 );
			pCsvEdit->SetSel( pCsvEdit->LineIndex( inoLine - 1 ), pCsvEdit->LineIndex( inoLine ), FALSE );
			return ( TRUE );
	}	}
	else if ( pMsg->message == WM_LBUTTONDOWN )
	{	int id = FromHandle( pMsg->hwnd )->GetDlgCtrlID();	// フォーカスのコントロールID
		if ( id == IDC_CSVEDIT )							// フォーカスがエディットコントロール 
		{	CEdit *pCsvEdit = ( CEdit * )GetDlgItem( IDC_CSVEDIT );
			CPoint point = ( CPoint )pMsg->lParam;
//    WPARAM      wParam;
//    LPARAM      lParam;
			int n = pCsvEdit->CharFromPos( point );
			int nLineIndex = HIWORD( n );
			int nCol = LOWORD( n );
			int inoLine = pCsvEdit->GetLineCount();

//			pCsvEdit->SetSel( pCsvEdit->LineIndex( nLineIndex ), pCsvEdit->LineIndex( nLineIndex ), TRUE );
			pCsvEdit->SetSel( nCol, nCol, TRUE );
			pCsvEdit->SetFocus();
			pCsvEdit->ShowCaret();
			return ( TRUE );
	}	}
	else if ( pMsg->message == EM_SETSEL )
	{	int id = FromHandle( pMsg->hwnd )->GetDlgCtrlID();	// フォーカスのコントロールID
		if ( id == IDC_CSVEDIT )							// フォーカスがエディットコントロール 
		{	CEdit *pCsvEdit = ( CEdit * )GetDlgItem( IDC_CSVEDIT );
			BOOL stat = CDialogEx::PreTranslateMessage( pMsg );
			int LineNo = pCsvEdit->LineFromChar( pMsg->wParam );
			wchar_t szLine[256];
//wParam:	選択範囲の開始文字の位置。
//lParam:	選択範囲の終了文字位置
			pCsvEdit->GetLine( LineNo, szLine, lengthof( szLine ));
			return ( stat );
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


//void CUserDic::OnLButtonDown(UINT nFlags, CPoint point)
//{	CEdit *pCsvEdit = ( CEdit * )GetDlgItem( IDC_CSVEDIT );
//
//	pCsvEdit->SetSel( pCsvEdit->LineIndex( inoLine - 1 ), pCsvEdit->LineIndex( inoLine ), FALSE );
//	CDialogEx::OnLButtonDown(nFlags, point);
//}


void CUserDic::OnDestroy()
{
	CDialogEx::OnDestroy();

#if 0
	fpmecab_destroy( mecab );
	fpmecab_lattice_destroy( lattice );
	fpmecab_model_destroy( model );
#endif
}


afx_msg LRESULT CUserDic::OnUmUseraction(WPARAM wParam, LPARAM lParam)
{	CEdit *pCsvEdit = ( CEdit * )GetDlgItem( IDC_CSVEDIT );
	static int LineNoo = -1;
	CPoint point;
	wchar_t szLine[256], *p;

	point = pCsvEdit->GetCaretPos();
	int n = pCsvEdit->CharFromPos( point );
	int LineNo = HIWORD( n );
	if ( LineNoo != LineNo )
	{	LineNoo = LineNo;
		pCsvEdit->GetLine( LineNo, szLine, lengthof( szLine ));
//		if (( p = wcschr( szLine, L',' )) != NULL )
//		{	*p = L'\0';
			CostCalc( szLine );
//		}
	}
	return 0;
}


void CUserDic::CostCalc( wchar_t *szLine )
{	CEdit *pSeikiCost = ( CEdit * )GetDlgItem( IDC_SEIKICOSTEDIT );
	CEdit *pOutput = ( CEdit * )GetDlgItem( IDC_OUTPUTEDIT );
	CComboBox *pLefId = ( CComboBox * )GetDlgItem( IDC_LEFTID );
	CComboBox *pRightId = ( CComboBox * )GetDlgItem( IDC_RIGHTID );
	CComboBox *pPosId = ( CComboBox * )GetDlgItem( IDC_HINSHI );
	char *p;
	char szUtf8[512];
	wchar_t szUni[512], szUni2[512];
	wchar_t *pEOS1, *pEOS2, *pTAB3;
	int unilen, splitCost, wordCost, seikiCost;
	int lId, rId, iNo, i;

	lId = rId = 0;
	if (( pEOS1 = wcschr( szLine, L',' )) == NULL )
	{	return;
	}
	*pEOS1 = L'\0';
	swscanf( pEOS1 + 1, L"%d,%d", &lId, &rId );
	swprintf( szUni, L"%d", lId );
	if (( iNo = pLefId->FindString( 0, szUni )) != CB_ERR )
	{	pLefId->SetCurSel( iNo );
	}
	swprintf( szUni, L"%d", rId );
	if (( iNo = pRightId->FindString( 0, szUni )) != CB_ERR )
	{	pRightId->SetCurSel( iNo );
	}
	for ( i = 0; i < 3; i++ )
	{	if (( pEOS1 = wcschr( pEOS1 + 1, L',' )) == NULL )
		{	break;;
	}	}
	if ( pEOS1 != NULL )
	{	pEOS2 = pEOS1 + 1;
		for ( i = 0; i < 4; i++ )
		{	if (( pEOS1 = wcschr( pEOS1 + 1, L',' )) == NULL )
			{	break;;
		}	}
		if ( pEOS1 != NULL )
		{	*pEOS1 = L'\0';
			if (( iNo = pPosId->FindString( 0, pEOS2 )) != CB_ERR )
			{	pPosId->SetCurSel( iNo );
	}	}	}

//	swprintf( szUni, L"%d", lId );
//	pPosId->SetWindowTextW( szUni );
	memset( &( szUtf8[0]), 0, sizeof( szUtf8 ));
	wcscpy( &szUni[0], szLine );
	UniToUTF8(( char * )szLine, &( szUtf8[0]), sizeof( szUtf8 ));
	fpmecab_lattice_set_sentence( lattice, &( szUtf8[0]));
	fpmecab_parse_lattice( mecab, lattice );
	p = ( char * )fpmecab_lattice_tostr( lattice );
	UTF8ToUni( p, ( char * )( &szUni[0]), sizeof( szUni ));
	unilen = wcslen( szUni );

	memset( &( szUtf8[0]), 0, sizeof( szUtf8 ));
	wcscpy( &szUni2[0], szLine );
	UniToUTF8(( char * )&szUni2[0], &( szUtf8[0]), sizeof( szUtf8 ));
	int utf8len = strlen( szUtf8 );
//	int i;
	fpmecab_lattice_set_sentence( lattice, &( szUtf8[0]));
	fpmecab_lattice_set_boundary_constraint( lattice, 0, MECAB_TOKEN_BOUNDARY );
	for ( i = 1; i < utf8len; i++ )
	{	fpmecab_lattice_set_boundary_constraint( lattice, i, MECAB_INSIDE_TOKEN );
	}
	fpmecab_lattice_set_boundary_constraint( lattice, i, MECAB_TOKEN_BOUNDARY );
	fpmecab_parse_lattice( mecab, lattice );
	p = ( char * )fpmecab_lattice_tostr( lattice );
	UTF8ToUni( p, ( char * )( &szUni[unilen]), sizeof( szUni ) - unilen );

	cvtUnicr2crlf(( &szUni[0]), sizeof( szUni ));
	pOutput->SetWindowTextW(( LPCTSTR )szUni );
	splitCost = wordCost = seikiCost = 0;
	if (( pEOS1 = wcsstr( szUni, L"EOS" )) != NULL )
	{	swscanf( pEOS1, L"EOS\t%*d,%*d,%d", &splitCost );
		if (( pEOS2 = wcsstr( pEOS1 + 1, L"EOS" )) != NULL )
		{	swscanf( pEOS2, L"EOS\t%*d,%*d,%d", &wordCost );
			if (( pTAB3 = wcsstr( pEOS1 + 1, L"\t" )) != NULL )
			{	if (( pTAB3 = wcsstr( pTAB3 + 1, L"\t" )) != NULL )
				{	if (( pTAB3 = wcsstr( pTAB3 + 1, L"\t" )) != NULL )
					{	swscanf( pTAB3, L"%d,%*d,%*d", &seikiCost );
		}	}	}	}
		seikiCost -= wordCost - splitCost + 1;
/*
二      名詞,数,*,*,*,*,二,ニ,ニ        2914,570,3484
人      名詞,接尾,助数詞,*,*,*,人,ニン,ニン     8672,-10731,1425
EOS     0,28,1453
二人    名詞,一般,*,*,*,*,二人,フタリ,フタリ    8962,-283,8679
EOS     0,-573,8106
*/
	}
	sprintf( szUtf8, "%d", seikiCost );
	UTF8ToUni( szUtf8, ( char * )( &szUni[0]), sizeof( szUni ));
	pSeikiCost->SetWindowTextW(( LPCTSTR )szUni );
//	pOutput->SetWindowTextW( L"二	名詞,数,*,*,*,*,二,ニ,ニ	2914,570,3484\r\n人	名詞,接尾,助数詞,*,*,*,人,ニン,ニン	8672,-10731,1425\r\nEOS	0,28,1453\r\n" );


}



void CUserDic::OnSelchangeLeftid()
{	CEdit *pCsvEdit = ( CEdit * )GetDlgItem( IDC_CSVEDIT );
	CComboBox *pLefId = ( CComboBox * )GetDlgItem( IDC_LEFTID );
	wchar_t szLine[256], *p, *pCanma1, *pCanma2;
	int lId, iLineIdx;

	pLefId->GetLBText( pLefId->GetCurSel(), szLine );
	swscanf( szLine, L"%d", &lId );
	pCsvEdit->GetLine(( iLineIdx = pCsvEdit->LineFromChar()), szLine, lengthof( szLine ));
	if (( pCanma1 = wcschr( szLine, L',' )) != NULL )
	{	pCanma1++;
		if (( pCanma2 = wcschr( pCanma1, L',' )) != NULL )
		{	//pCanma2--;
			iLineIdx = pCsvEdit->LineIndex( iLineIdx );
			pCsvEdit->SetSel( iLineIdx + ( pCanma1 - szLine ), iLineIdx + ( pCanma2 - szLine ), FALSE );
			swprintf( szLine, L"%d", lId );
			pCsvEdit->ReplaceSel( szLine );
			pCsvEdit->SetModify( TRUE );
	}	}
}


void CUserDic::OnEditupdateLeftid()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
}


void CUserDic::OnSelchangeRightid()
{	CEdit *pCsvEdit = ( CEdit * )GetDlgItem( IDC_CSVEDIT );
	CComboBox *pRightId = ( CComboBox * )GetDlgItem( IDC_RIGHTID );
	wchar_t szLine[256], *p, *pCanma1, *pCanma2;
	int rId, iLineIdx;

	pRightId->GetLBText( pRightId->GetCurSel(), szLine );
	swscanf( szLine, L"%d", &rId );
	pCsvEdit->GetLine(( iLineIdx = pCsvEdit->LineFromChar()), szLine, lengthof( szLine ));
	if (( pCanma1 = wcschr( szLine, L',' )) != NULL )
	{	if (( pCanma1 = wcschr( pCanma1 + 1, L',' )) != NULL )
		{	pCanma1++;
			if (( pCanma2 = wcschr( pCanma1, L',' )) != NULL )
			{	//pCanma2--;
				iLineIdx = pCsvEdit->LineIndex( iLineIdx );
				pCsvEdit->SetSel( iLineIdx + ( pCanma1 - szLine ), iLineIdx + ( pCanma2 - szLine ), FALSE );
				swprintf( szLine, L"%d", rId );
				pCsvEdit->ReplaceSel( szLine );
				pCsvEdit->SetModify( TRUE );
	}	}	}
}


void CUserDic::OnSelchangeHinshi()
{	CEdit *pCsvEdit = ( CEdit * )GetDlgItem( IDC_CSVEDIT );
	CComboBox *pPosId = ( CComboBox * )GetDlgItem( IDC_HINSHI );
	wchar_t szLine[256], szEdCsv[256], *p, *pCanma1, *pCanma2;
	int rId, iLineIdx;

	pPosId->GetLBText( pPosId->GetCurSel(), szLine );
	if (( pCanma1 = wcschr( szLine, L' ' )) != NULL )
	{	*pCanma1 = L'\0';
	}
	pCsvEdit->GetLine(( iLineIdx = pCsvEdit->LineFromChar()), szEdCsv, lengthof( szEdCsv ));
	if (( pCanma1 = wcschr( szEdCsv, L',' )) != NULL )
	{	if (( pCanma1 = wcschr( pCanma1 + 1, L',' )) != NULL )
		{	if (( pCanma1 = wcschr( pCanma1 + 1, L',' )) != NULL )
			{	if (( pCanma1 = wcschr( pCanma1 + 1, L',' )) != NULL )
				{	pCanma1++;
					if (( pCanma2 = wcschr( pCanma1, L',' )) != NULL )
					{	if (( pCanma2 = wcschr( pCanma2 + 1, L',' )) != NULL )
						{	if (( pCanma2 = wcschr( pCanma2 + 1, L',' )) != NULL )
							{	if (( pCanma2 = wcschr( pCanma2 + 1, L',' )) != NULL )
								{	//pCanma2--;
									iLineIdx = pCsvEdit->LineIndex( iLineIdx );
									pCsvEdit->SetSel( iLineIdx + ( pCanma1 - szEdCsv ), iLineIdx + ( pCanma2 - szEdCsv ), FALSE );
									pCsvEdit->ReplaceSel( szLine );
									pCsvEdit->SetModify( TRUE );
	}	}	}	}	}	}	}	}
}
