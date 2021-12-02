// CorpasDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MecaDoku.h"
#include "CorpasDlg.h"
#include "SarchCsv.h"
#include "afxdialogex.h"
#include "mecab.h"
#include "mecabdll.h"

extern void UniToUTF8( char *pszUni, char *pszUtf8, int lenUtf8 );
extern void UTF8ToUni( char *pszUtf8, char *pszUni, int lenUni );
extern CSarchCsv	*pSarchCsv;
//extern mecab_model_t *mecab_model_new_build_option();
extern void xchgJIS2004( wchar_t *pmbstring );
extern BOOL extractHatuonfeature( char *dst, char *src );
extern BOOL	fgBgBlack;


// CCorpasDlg �_�C�A���O

IMPLEMENT_DYNAMIC(CCorpasDlg, CDialogEx)

CCorpasDlg::CCorpasDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCorpasDlg::IDD, pParent)
{

}

CCorpasDlg::~CCorpasDlg()
{
}

void CCorpasDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CORPAS21, m_Corpas);
	//  DDX_Control(pDX, IDC_CORPASAPEND, m_CorpasAppend);
	//  DDX_Control(pDX, IDC_CORPASREF, m_CorpasRef);
	//  DDX_Control(pDX, IDC_TESTAPEND, m_TestAppend);
	//  DDX_Control(pDX, IDC_TESTREF, m_TestRef);
	//  DDX_Control(pDX, IDC_CORPAS, m_CorpasFile);
	//  DDX_Control(pDX, IDC_TEST, m_TestFile);
}


BEGIN_MESSAGE_MAP(CCorpasDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CORPASAPEND, &CCorpasDlg::OnClickedCorpasapend)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_COPY, &CCorpasDlg::OnCopy)
	ON_COMMAND(ID_PASTE, &CCorpasDlg::OnPaste)
	ON_COMMAND(ID_DEL, &CCorpasDlg::OnDel)
	ON_WM_SIZE()
	ON_COMMAND(ID__CSVSERCH, &CCorpasDlg::OnCsvserch)
	ON_COMMAND(ID__ALLCLEAR2, &CCorpasDlg::OnAllclear2)
END_MESSAGE_MAP()


void CCorpasDlg::InitialUpdate()
{	CMecaDokuApp *lpApp = ( CMecaDokuApp * )AfxGetApp();
//	wchar_t szVal[16];

//	m_CorpasRef.setBase_BottomLeft();
//	m_CorpasAppend.setBase_BottomLeft();
//	m_TestRef.setBase_BottomLeft();
//	m_TestAppend.setBase_BottomLeft();
	m_Corpas.setBase( m_Corpas.normal );
//	m_CorpasFile.setBase( m_CorpasFile.bottom );
//	m_TestFile.setBase( m_TestFile.bottom );
	m_Corpas.LimitText( 32768l );
#if 1
	int	hight;

	if ( fgBgBlack )							// ���o�b�N�ɔ����� 
	{	COLORREF bkclr = RGB( 0, 0, 0 );
		::SendMessage( m_Corpas.m_hWnd, EM_SETBKGNDCOLOR, ( WPARAM )0, ( LPARAM )bkclr );
	}
	if (( hight = lpApp->GetProfCharHight()) >= 30 )
	{	CHARFORMAT2 cf;

		memset(( void * )&cf, 0, sizeof( cf ));
		cf.dwMask = CFM_SIZE | CFM_FACE;
		cf.yHeight = hight;
		wcscpy( cf.szFaceName, L"Meiryo UI" );
		if ( fgBgBlack )							// ���o�b�N�ɔ����� 
		{	COLORREF chrclr = RGB( 255, 255, 255 );
			cf.dwMask |= CFM_COLOR;
			cf.dwEffects &= ~CFE_AUTOCOLOR;
			cf.crTextColor = chrclr;
		}
		m_Corpas.SetDefaultCharFormat( cf );
	}
#endif
}


void CCorpasDlg::toCorpas( wchar_t *psrc )
{	static char *szDaitai = "????,????,*,*,*,*,*";
	char *pszOrg;
	char bufUTF8[MAX_LINE], szOutput[MAX_LINE];
	static mecab_model_t *model;
	static mecab_t *mecab;
	static mecab_lattice_t *lattice;
//	const mecab_node_t *node;

	if ( OpenMecabDll() == NULL )
	{	return;
	}
	if (( model = mecab_model_new_build_option()) == NULL )
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
	if (( lattice = fpmecab_model_new_lattice( model)) == NULL )
	{	fpmecab_destroy( mecab );
		return;
	}
	xchgJIS2004( psrc );					// JIS2004�ǉ��������ȑO�̊����ɒu������ 
	memset( bufUTF8, 0, sizeof( bufUTF8 ));
	wcscpy(( wchar_t * )szOutput, psrc );
	wcscat(( wchar_t * )szOutput, L"\n" );
	UniToUTF8(( char * )psrc, &( bufUTF8[0]), sizeof( bufUTF8 ));
	if (( pszOrg = _strdup( bufUTF8 )) == NULL )
	{	return;
	}
	fpmecab_lattice_set_sentence( lattice, pszOrg );
	fpmecab_parse_lattice( mecab, lattice );
#if 0
	node = fpmecab_lattice_get_bos_node( lattice );
	node = node->next;
//	int end = m_Corpas.GetWindowTextLength();
//	m_Corpas.SetSel( end, end );
//	m_Corpas.ReplaceSel(( PCTSTR )szOutput );
	for ( ; node != NULL; node = node->next )
	{	if ( node->posid != 0 )
		{	strncpy_s( szOutput, sizeof( szOutput ), ( char * )node->surface, node->length );
			szOutput[node->length] = '\t';
			szOutput[node->length + 1] = EOS;
			if ( extractHatuonfeature( bufUTF8, ( char * )node->feature ) != 0 )
			{	strcpy(( char * )&( szOutput[node->length + 1]), ( char * )node->feature );
			}
			else
			{	strcpy(( char * )&( szOutput[node->length + 1]), szDaitai );
			}
			strcat( szOutput, "\n" );
			UTF8ToUni(( char * )szOutput, bufUTF8, sizeof( bufUTF8 ));
			end = m_Corpas.GetWindowTextLength();
			m_Corpas.SetSel( end, end );
			m_Corpas.ReplaceSel(( PCTSTR )bufUTF8 );
	}	}
	end = m_Corpas.GetWindowTextLength();
	m_Corpas.SetSel( end, end );
	m_Corpas.ReplaceSel(( PCTSTR )L"EOS\n" );
	DWORD orgLen = m_Corpas.GetLimitText();
	m_Corpas.LimitText( orgLen + 1024 );
#else
	sprintf_s( szOutput, sizeof( szOutput ), "%s\n%s", bufUTF8, fpmecab_lattice_tostr( lattice ));
	UTF8ToUni( szOutput, bufUTF8, sizeof( bufUTF8 ));
	int end = m_Corpas.GetWindowTextLength();
	m_Corpas.SetSel( end, end );
	m_Corpas.ReplaceSel(( PCTSTR )bufUTF8 );
#endif
	if ( pszOrg != NULL )	free( pszOrg );
	fpmecab_destroy( mecab );
	fpmecab_lattice_destroy( lattice );
	fpmecab_model_destroy( model );
//	CloseMecabDll();
	m_Corpas.PostMessage( WM_VSCROLL, MAKELONG( SB_BOTTOM, NULL ));
}


// CCorpasDlg ���b�Z�[�W �n���h���[


void CCorpasDlg::OnClickedCorpasapend()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
}


void CCorpasDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point )
{	CRect rc;
	CMenu cMenu, *menuPopup;

	if (( point.x == -1 ) && ( point.y == -1 ))
	{	LONG start, end;

		m_Corpas.GetSel( start, end );
		point = m_Corpas.PosFromChar( end );
		m_Corpas.ClientToScreen( &point );
	}
	// ���W�����b�`�G�f�B�b�g�R���g���[�����̏ꍇ�̂݃|�b�v�A�b�v���j���[��\��
	m_Corpas.GetClientRect( &rc );
	m_Corpas.ClientToScreen( &rc );
	if ( rc.PtInRect( point ))
	{	cMenu.LoadMenu( IDR_POPUP_EDIT );
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


void CCorpasDlg::OnCopy()
{	m_Corpas.Copy();
}


void CCorpasDlg::OnPaste()
{	m_Corpas.Paste();
}


void CCorpasDlg::OnDel()
{	m_Corpas.Cut();
}


void CCorpasDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	m_Corpas.Resize();
//	m_CorpasRef.Resize();
//	m_CorpasAppend.Resize();
//	m_CorpasFile.Resize();
//	m_TestRef.Resize();
//	m_TestAppend.Resize();
//	m_TestFile.Resize();
}


void CCorpasDlg::OnCsvserch()
{	int LineNo;
	long	nStartChar, nEndChar;
	wchar_t sarchstr[MAX_LINE], *p;
	RECT rctThis, rctSetting;

	m_Corpas.GetSel( nStartChar, nEndChar );
	LineNo = m_Corpas.LineFromChar( nStartChar );
	//wcscpy( sarchstr, L"\\n" );
	//m_Corpas.GetLine( LineNo, &( sarchstr[2]), lengthof( sarchstr ) - 3 );
	memset( &( sarchstr[0]), 0, sizeof( sarchstr ));
	m_Corpas.GetLine( LineNo, &( sarchstr[0]), lengthof( sarchstr ) - 1 );
//	sarchstr[MAX_LINE - 1] = L'\0';
	if (( p = wcschr( sarchstr, L'\t' )) == NULL )
	{	return;
	}
//	int lennn = ( int )( p - &( sarchstr[0]));
//	int len = lengthof( sarchstr ) - lennn;
//	wcscpy_s( p, len, L"," );
	wcscpy_s( p, lengthof( sarchstr ) - (( int )( p - &( sarchstr[0]))) - 1, L"," );
	if ( pSarchCsv->ShowWindow( SW_SHOWNA ) == 0 )
	{	GetWindowRect( &rctThis );					// ��ʍ��W
		pSarchCsv->GetWindowRect( &rctSetting );		// ��ʍ��W
		pSarchCsv->MoveWindow( rctThis.left, rctThis.bottom,
			rctSetting.right - rctSetting.left, rctSetting.bottom - rctSetting.top, FALSE );
		pSarchCsv->ShowWindow( SW_SHOW );
	}
	pSarchCsv->SetSarchStr( sarchstr );
}


void CCorpasDlg::OnAllclear2()
{	m_Corpas.SetSel( 0, -1 );
	m_Corpas.Cut();
}



BOOL CCorpasDlg::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN )
	{	if (( pMsg->wParam == VK_RETURN ) || ( pMsg->wParam == VK_ESCAPE ))
		{	return ( FALSE );
	}	}
	else if ( pMsg->message == WM_MOUSEWHEEL )
	{	int id = FromHandle( pMsg->hwnd )->GetDlgCtrlID();	// �t�H�[�J�X�̃R���g���[��ID
		if ( id == IDC_CORPAS21 )							// �t�H�[�J�X���G�f�B�b�g�R���g���[�� 
		{	unsigned short fwKeys = GET_KEYSTATE_WPARAM( pMsg->wParam );
			if (( fwKeys & MK_CONTROL ) != 0 )
			{	CHARFORMAT2 cf;
				DWORD dwMask;
				short zDelta = GET_WHEEL_DELTA_WPARAM( pMsg->wParam ); // ���̒l�͕��ɂȂ邱�Ƃ�����

				memset(( void * )&cf, 0, sizeof( cf ));
				dwMask = m_Corpas.GetDefaultCharFormat( cf );
				cf.yHeight += zDelta / 30;	//120;		// 120�P�ʂŕω�����炵�� 
				if ( cf.yHeight < 30 )
				{	cf.yHeight = 30;
				}
				cf.dwMask = CFM_SIZE;
				m_Corpas.SetDefaultCharFormat( cf );
				return ( TRUE );
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CCorpasDlg::OnCancel()
{	CDialogEx::OnCancel();
}




void CCorpasDlg::OnDsplayOutputDlg()
{//	int LineNo;
//	long	nStartChar, nEndChar;
//	wchar_t sarchstr[MAX_LINE], *p;
	RECT rctThis, rctSetting;

	if ( pSarchCsv->ShowWindow( SW_SHOWNA ) == 0 )
	{	GetWindowRect( &rctThis );					// ��ʍ��W
		pSarchCsv->GetWindowRect( &rctSetting );	// ��ʍ��W
		pSarchCsv->MoveWindow( rctThis.left, rctThis.bottom,
			rctSetting.right - rctSetting.left, rctSetting.bottom - rctSetting.top, FALSE );
		pSarchCsv->ShowWindow( SW_SHOW );
	}
}
