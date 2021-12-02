
// MecaDokuDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MecaDoku.h"
#include "MecaDokuDlg.h"
#include "CorpasDlg.h"
#include "afxdialogex.h"
#include "atlstr.h"
#include "mecabdll.h"
#include <mlang.h>
#include <Mmsystem.h>
#include <WinNT.h>

//#ifdef JTALK
//#include "jtalk.h"
//#else
#include "AquesTalk.h"
//#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable : 4996)

extern int initial_dlls();									// mecab & aquest & wave
//extern int aqtk10waveInitialize();
extern int lineCvtAndPlay( wchar_t *pmbstring, struct PLAYINGSENTENCE *ps );
extern int lineCvtAndPlaySapi( wchar_t *pmbstring, struct PLAYINGSENTENCE *ps );
extern int waveFainalize();
extern int fainalize_dlls();
extern void UTF8ToUni( char *pszUtf8, char *pszUni, int lenUni );
extern int CodePageToUni( UINT codepage,  char *pszCPstr, char *pszUni, int lenUni );
extern void appVolSet( USHORT vol );
extern USHORT appVolGet();
extern DWORD GetFileSize( wchar_t *Path );
extern int aqtk10waveInitialize2();
extern BOOL	fgBgBlack;

extern TTSNAME	SelectTts;
extern wchar_t	*lpszRecentFile;
extern char				bufHiraganaJis[MAX_LINE];
extern AQTK_VOICE		voice;

//struct LINEBUFS lb[2] = { 0 };
struct PLAYINGSENTENCE	ps = { 0 };
CSarchCsv				*pSarchCsv = NULL;
CCorpasDlg				*pCorpasDlg = NULL;

int						iNxtLineNo = 0;
int						iTgtQue = 0;
int						nCodePage = 0;
BOOL					fgSkipspeak = FALSE;
enum PLAYSTATE			iPlayState;

extern	wchar_t			szMeCabExe[_MAX_PATH];
extern	wchar_t			szUserDictionary[_MAX_PATH];
extern	LONG			editcharHeight;
extern	volatile int	ctPlayRec;		// waveOutWrite touroku count
extern	BOOL			fgPausedspoff;
extern	BOOL			fgKigouPass;

#define _MAX_QUE		4
#define _MAX_LINE		10240

struct LineData
{	wchar_t	*pszLine;
	int		iallocSize;
	int		iLineSize;
	int		iIndexStart;
	int		iIndexEnd;
};

struct LineData	LineQue[_MAX_QUE] = { 0 };
int		iQueIn = 0;
int		iQueOut = 0;
int		iQueCnt = 0;
static CRITICAL_SECTION	csQueCnt;
//InitializeCriticalSection( &cstPlayRec );
//    EnterCriticalSection( &cstPlayRec );
//    LeaveCriticalSection( &cstPlayRec );
//	CFont editFont;
wchar_t	szLineBuild[_MAX_LINE] = { 0 };


//
//  �@�\    :�f�[�^�t�@�C���̑��݃`�F�N
//
//  �Ԃ�l  :�݂�F��O�@�����F�O
//
//  �@�\����:�f�[�^�t�@�C���̑��݃`�F�N 
//
DWORD GetFileSize( wchar_t *Path )
{	struct _wfinddata64_t c_file;

	if ( _wfindfirst64( Path, &c_file ) == -1L )
	{	return ( -1 );
	}
	return (( DWORD )c_file.size );
}


int splitSentence( wchar_t *pszLine )
{	wchar_t *p;

	if (( p = wcschr( pszLine, L'�B' )) == NULL )
	{	if (( p = wcschr( pszLine, L'�A' )) == NULL )
		{	if (( p = wcschr( pszLine, L'�v' )) == NULL )
			{	if (( p = wcschr( pszLine, L'�j' )) == NULL )
				{	if (( p = wcschr( pszLine, L'�x' )) == NULL )
					{	if (( p = wcschr( pszLine, L'�t' )) == NULL )
						{	if (( p = wcschr( pszLine, L'�@' )) == NULL )
							{	if (( p = wcschr( pszLine, L'�H' )) == NULL )
								{	return ( -1 );
								}
								else
								{	*p = L'�A';
							}	}
							else
							{	*p = L'�A';
	}	}	}	}	}	}	}
	return (( int )( p + 1 - pszLine ));
}


UINT preTalkThread( LPVOID pParam )
{	CMecaDokuDlg* pObj = ( CMecaDokuDlg * )pParam;
	int	iQueCntCpy;
	wchar_t *pnonSpace;

	if ( initial_dlls() != 0 )								// mecab & aquest & wave
	{	AfxMessageBox( L"DLL �I�[�v���G���[�I�ݒ�����m�F��������" );
		return ( 0 );
	}
	pObj->PostMessageW( UM_REQUEST_NEXTLINE, iQueOut = 0, 0 );
	pObj->PostMessageW( UM_REQUEST_NEXTLINE, 1, 0 );
//	pObj->PostMessageW( UM_ENABLE_VOLUMESLIDER, 0, 0 );
	for ( iTgtQue = 0; iPlayState != STOP; )
	{	if ( iPlayState == PAUSE )
		{	_sleep( 100 );
			continue;
		}
		EnterCriticalSection( &csQueCnt );
		iQueCntCpy = iQueCnt;
		LeaveCriticalSection( &csQueCnt );
		if ( iQueCntCpy <= 0 )
		{	if ( iPlayState == PRESTOP )
			{	if ( ctPlayRec <= 0 )
				{	break;
				}
				ps.iLineNo = 0;
				if ( SelectTts == SAPI54 )
				{	lineCvtAndPlaySapi( L"", &ps );
				}
				else
				{	lineCvtAndPlay( L"", &ps );
				}
				continue;
			}
			_sleep( 100 );
			continue;
		}
		ps.iLineNo = LineQue[iQueOut].iIndexStart;
		pObj->PostMessageW( UM_REQUEST_SELLINE, iQueOut, 0 );
		for ( pnonSpace = LineQue[iQueOut].pszLine;
			( *pnonSpace == L' ' ) || ( *pnonSpace == L'�@' ) || ( *pnonSpace == L'\t' ); pnonSpace++ )
		{	;			// �s���̋󔒂��X�L�b�v���� MeCab���s���󔒂Ō�邽�ߒǉ� 
		}
		if ( *pnonSpace == L'\0' )
		{	continue;	// �s���܂ŃX�L�b�v������ 
		}
		if ( SelectTts == SAPI54 )
		{	lineCvtAndPlaySapi( pnonSpace/*LineQue[iQueOut].pszLine*/, &ps );
		}
		else
		{	lineCvtAndPlay( pnonSpace/*LineQue[iQueOut].pszLine*/, &ps );
		}
//		LineQue[iQueOut].iLineNo = -1;
		pObj->PostMessageW( UM_REQUEST_NEXTLINE, iTgtQue, 0 );
		iQueOut = ( iQueOut + 1 ) % lengthof( LineQue );
		EnterCriticalSection( &csQueCnt );
		iQueCnt--;
		LeaveCriticalSection( &csQueCnt );
	}
	fainalize_dlls();
	{	CButton *pStopBtn = ( CButton * )pObj->GetDlgItem( IDC_STOP );

		pStopBtn->PostMessageW( BM_CLICK, 0, 0 );		// �Ō�̃Z���e���X�����Ȃ����� 
	}
    return 0;   // thread completed successfully
}


afx_msg LRESULT CMecaDokuDlg::OnUmRequestNextline(WPARAM wParam, LPARAM lParam)
{	int lineSize, iLineLen;
	wchar_t *p;

	if ( iQueCnt >= ( _MAX_QUE - 1 ))
	{	return 0;
	}
	for ( ; ( iLineLen = splitSentence( szLineBuild )) <= 0; iNxtLineNo++ )
	{	if ( iNxtLineNo >= ( iLineLen = PlayTextEdit.GetLineCount()))
		{	iPlayState = PRESTOP;
			wcscat_s( szLineBuild, lengthof( szLineBuild ) - 1, L"�B" );
			iLineLen = ( int )wcslen( szLineBuild );	// for tchar
			break;
		}
		iLineLen = ( int )wcslen( szLineBuild );		// for tchar
		lineSize = PlayTextEdit.LineLength( iNxtLineNo );
		if (( iLineLen + lineSize + 1 ) > lengthof( szLineBuild ))
		{	AfxMessageBox( L"LineBuildBuffer overflow! skiped!" );
			iLineLen = 0;
		}
		lineSize = PlayTextEdit.GetLine( iNxtLineNo, &( szLineBuild[iLineLen]), lengthof( szLineBuild ) - iLineLen );
		szLineBuild[iLineLen + lineSize] = L'\0';
		if (( p = wcschr( szLineBuild, L'\r' )) != NULL )
		{	*p = L'\0';
		}
		_sleep( 0 );
	}
	if ( LineQue[iQueIn].iallocSize < iLineLen )
	{	if ( LineQue[iQueIn].pszLine != NULL )
		{	free( LineQue[iQueIn].pszLine );
		}
		LineQue[iQueIn].iallocSize = iLineLen * 15 / 10;
		if ( LineQue[iQueIn].iallocSize < MAXLINEDEF )
		{	LineQue[iQueIn].iallocSize = MAXLINEDEF;
		}
		if (( LineQue[iQueIn].pszLine = ( wchar_t * )malloc( LineQue[iQueIn].iallocSize )) == NULL )
		{	LineQue[iQueIn].iallocSize = 0;		// memory allocate error
			return -1;
	}	}
	wcsncpy( LineQue[iQueIn].pszLine, szLineBuild, iLineLen );		// + 1
	LineQue[iQueIn].pszLine[( LineQue[iQueIn].iLineSize = iLineLen)/* + 1*/] = '\0';
	LineQue[iQueIn].iIndexStart = iNxtLineNo - 1;
	LineQue[iQueIn].iIndexEnd = iNxtLineNo;
	wcscpy( &( szLineBuild[0]), &( szLineBuild[iLineLen/* + 1*/]));
//	szLineBuild[iLineLen/* + 1*/] = '\0';
	iQueIn = ( iQueIn + 1 ) % lengthof( LineQue );
	EnterCriticalSection( &csQueCnt );
	if ( iPlayState != STOP )
	{	iQueCnt++;
	}
	LeaveCriticalSection( &csQueCnt );
	return 0;
}


// �A�v���P�[�V�����̃o�[�W�������Ɏg���� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

// ����
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnCancel();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_MOUSEHWHEEL()
ON_WM_POWERBROADCAST()
END_MESSAGE_MAP()


// CMecaDokuDlg �_�C�A���O




CMecaDokuDlg::CMecaDokuDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMecaDokuDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMecaDokuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLAYTEXTEDIT, PlayTextEdit);
	DDX_Control(pDX, IDC_PLAY, btnPlay);
	DDX_Control(pDX, IDC_STOP, btnStop);
	//	DDX_Control(pDX, IDC_PAUSE, btnPause);
	DDX_Control(pDX, IDC_VOLUMESLIDER, sldVolume);
	//	DDX_Control(pDX, IDC_VOLUMFSIZE, sldFontSize);
	DDX_Control(pDX, IDC_VOLCOMMENT, m_VolComment);
	DDX_Control(pDX, IDC_SARCH, m_SarchBtn);
	DDX_Control(pDX, IDC_SARCHSTR, m_SarchStr);
}

BEGIN_MESSAGE_MAP(CMecaDokuDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_PLAY, &CMecaDokuDlg::OnClickedPlay)
//	ON_BN_CLICKED(IDC_PAUSE, &CMecaDokuDlg::OnClickedPause)
	ON_BN_CLICKED(IDC_STOP, &CMecaDokuDlg::OnClickedStop)
	ON_MESSAGE(UM_REQUEST_NEXTLINE, &CMecaDokuDlg::OnUmRequestNextline)
	ON_MESSAGE(UM_REQUEST_SELLINE, &CMecaDokuDlg::OnUmRequestSelline)
//	ON_MESSAGE(UM_ENABLE_VOLUMESLIDER, &CMecaDokuDlg::OnUmEnableSlider)
//	ON_NOTIFY(NM_THEMECHANGED, IDC_VOLUMESLIDER, &CMecaDokuDlg::OnThemechangedVolumeslider)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_SETTING, &CMecaDokuDlg::OnClickedSetting)
//	ON_BN_CLICKED(IDC_DICTIONALY, &CMecaDokuDlg::OnClickedDictionaly)
	ON_WM_DESTROY()
	ON_WM_DROPFILES()
//	ON_BN_CLICKED(IDC_2CORPAS, &CMecaDokuDlg::OnClicked2corpas)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_COPY, &CMecaDokuDlg::OnCopy)
	ON_COMMAND(ID_PASTE, &CMecaDokuDlg::OnPaste)
	ON_COMMAND(ID_DEL, &CMecaDokuDlg::OnDel)
	ON_COMMAND(ID__CORPAS, &CMecaDokuDlg::OnCorpas)
	ON_COMMAND(ID__ALLCLR, &CMecaDokuDlg::OnAllclr)
//	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID__CLEARBEF, &CMecaDokuDlg::OnClearbef)
	ON_COMMAND(ID__PLAYTHIS, &CMecaDokuDlg::OnPlaythis)
	ON_BN_CLICKED(IDC_SARCH, &CMecaDokuDlg::OnClickedSarch)
	ON_COMMAND(ID_UNDO, &CMecaDokuDlg::OnUndo)
	ON_COMMAND(ID__PAUSEDSPOFF, &CMecaDokuDlg::OnPausedspoff)
	ON_COMMAND(ID__SKIPSPEAK, &CMecaDokuDlg::OnSkipspeak)
	ON_COMMAND(ID__KIGOUPASS, &CMecaDokuDlg::OnKigoupass)
	ON_COMMAND(WM_LOADRECENTFILE, &CMecaDokuDlg::OnLoadrecentfile)
	ON_WM_POWERBROADCAST()
END_MESSAGE_MAP()


// CMecaDokuDlg ���b�Z�[�W �n���h���[

BOOL CMecaDokuDlg::OnInitDialog()
{	CMecaDokuApp *lpApp = ( CMecaDokuApp * )AfxGetApp();

	CDialogEx::OnInitDialog();

	// "�o�[�W�������..." ���j���[���V�X�e�� ���j���[�ɒǉ����܂��B

	// IDM_ABOUTBOX �́A�V�X�e�� �R�}���h�͈͓̔��ɂȂ���΂Ȃ�܂���B
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	pThread = NULL;
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if ( pSysMenu != NULL )
	{	BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if ( !strAboutMenu.IsEmpty())
		{	pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	}	}

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
	m_hAccel = LoadAccelerators(AfxGetResourceHandle(),
	MAKEINTRESOURCE(IDR_ACCELERATOR1));

	PlayTextEdit.LimitText( 32768l * 4 );
	btnPlay.setBase_BottomRight();
	btnStop.setBase_BottomRight();
//	btnSetting.setBase_BottomRight();
//	btnDictionary.setBase_BottomRight();
	sldVolume.setBase_BottomLeft();
	m_VolComment.setBase_BottomLeft();
//	m_toCorpas.setBase_BottomRight();
//	btnPause.setBase_BottomRight();
//	sldFontSize.setBase_BottomLeft();
	m_SarchBtn.setBase_BottomRight();
	m_SarchStr.setBase_BottomRight();

	PlayTextEdit.HideSelection( FALSE, TRUE );

	iPlayState = STOP;
//	for ( i = 0; i< lengthof( lb ); i++ )
//	{	lb[i].iLineNo = -1;
//	}
//	if ( aqtk10waveInitialize() != 0 )			// multimedia open 
//	{	
//	}
	//sldVolume.SetRange( 0, 300, TRUE );		// int vol;	// ���� 0-300 default:100
	//sldVolume.SetTic( 5 );
	//sldVolume.SetTicFreq( 50 );
	//sldVolume.SetPos( 10 );		
	sldVolume.SetRange( 0, 65535, TRUE );		// multimedia system ���� 0-65535 
	sldVolume.SetTic( 5 );
	sldVolume.SetTicFreq( 4096 );
//	sldVolume.SetPos( appVolGet());				//  waveOutOpen��łȂ��Ǝ擾�ł��Ȃ��̂ŉ��Ɉړ� 
//	sldVolume.EnableWindow( FALSE );
#if 1
	int	hight;

	lpApp->LoadProfile();
//	if ( fgBgBlack )							// ���o�b�N�ɔ����� 
//	{	COLORREF bkclr = RGB( 0, 0, 0 );
//		::SendMessage( PlayTextEdit.m_hWnd, EM_SETBKGNDCOLOR, ( WPARAM )0, ( LPARAM )bkclr );
//	}

//	if (( hight = lpApp->GetProfCharHight()) >= 30 )
	{	CHARFORMAT2 cf;

		memset(( void * )&cf, 0, sizeof( cf ));
		cf.dwMask = CFM_FACE | CFM_PROTECTED;
		wcscpy( cf.szFaceName, L"Meiryo UI" );
		cf.dwEffects = CFE_PROTECTED;
		if (( hight = lpApp->GetProfCharHight()) >= 30 )
		{	cf.dwMask |= CFM_SIZE;
			cf.yHeight = hight;
		}
		if ( fgBgBlack )							// ���o�b�N�ɔ����� 
		{	COLORREF chrclr = RGB( 255, 255, 255 );
			COLORREF bkclr = RGB( 0, 0, 0 );

			cf.dwMask |= CFM_COLOR;
			cf.dwEffects &= ~CFE_AUTOCOLOR;
			cf.crTextColor = chrclr;
			::SendMessage( PlayTextEdit.m_hWnd, EM_SETBKGNDCOLOR, ( WPARAM )0, ( LPARAM )bkclr );
		}
		PlayTextEdit.SetDefaultCharFormat( cf );
//		long mask = PlayTextEdit.GetEventMask();			// ����t�����̃t�H���g�ω��Ɍ��ʂȂ�
//		PlayTextEdit.SetEventMask( mask | EN_PROTECTED );
// �f���A���t�H���g���[�h�𖳌��ɂ��锼�p�����̎��p				����t�����̃t�H���g�ω��Ɍ��ʂȂ�
//		LPARAM dwLangOptions = ::SendMessage( PlayTextEdit.m_hWnd, EM_GETLANGOPTIONS, 0, 0 ); 
//		dwLangOptions &= ~IMF_DUALFONT;
//		::SendMessage( PlayTextEdit.m_hWnd, EM_SETLANGOPTIONS, 0, ( LPARAM )dwLangOptions );
	}
#else
	LOGFONT LogFont;
	CFont*pFont = PlayTextEdit.GetFont();
	if ( pFont != NULL )
	{	if ( pFont->GetLogFont( &LogFont ) != 0 )
		{	LogFont.lfHeight = editcharHeight;
			editFont.CreateFontIndirect( &LogFont );
			PlayTextEdit.SetFont( &editFont );
	}	}
	else
	{	memset(( char * )&( editFont ), 0, sizeof( editFont ));
	}
#endif
//	pSetting = NULL;

	pSettingTab = new CSettingTab( this );
	pSettingTab->Create( CSettingTab::IDD, this );	// IDD_SETTING
//	pSettingTab->ShowWindow( SW_SHOW );
	pSettingTab->InitialUpdate();

//	pSetting = new CSettingDlg( this );
//	pSetting->Create( IDD_SETTING, this );	// IDD_SETTING
//	pSetting->ShowWindow( SW_SHOW );
//	pSetting->InitialUpdate();

	pCorpasDlg = new CCorpasDlg( this );
	pCorpasDlg->Create( IDD_CORPAS, this );	// IDD_CORPAS
//	pCorpasDlg->ShowWindow( SW_SHOW );
	pCorpasDlg->InitialUpdate();

#if 0
	pUserDict = new CUserDic( this );
	pUserDict->Create( IDD_USERDICUPDATE, this );	// IDD_USERDICUPDATE
	pUserDict->InitialUpdate();
//	pUserDict->ShowWindow( SW_SHOW );
#endif

	pSarchCsv = new CSarchCsv( this );
	pSarchCsv->Create( IDD_SARCHCSV, this );	// IDD_SARCHCSV
	pSarchCsv->InitialUpdate();
//	pSarchCsv->ShowWindow( SW_SHOW );

	InitializeCriticalSection( &csQueCnt );

//	if ( btnPlay.GetIcon() == NULL )
//	{	btnPlay.SetIcon( lpApp->LoadIcon( IDI_PLAYU ));
//	}
//	IDS_LOADRECENTFILE

	GUID display = GUID_CONSOLE_DISPLAY_STATE;
	RegisterPowerSettingNotification( m_hWnd, &display, DEVICE_NOTIFY_WINDOW_HANDLE );
	aqtk10waveInitialize2();				// waveOutOpen ��Uopen TTS�ɂ���Đݒ肪�Ⴄ�̂� 
	sldVolume.SetPos( appVolGet());	
	waveFainalize();						// waveOutClose �����N���[�Y 
	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}


void CMecaDokuDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CMecaDokuDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
/*
		LOGFONT LogFont;
		CFont font;
		CFont*pFont = PlayTextEdit.GetFont();

		if ( pFont != NULL )
		{	if ( pFont->GetLogFont( &LogFont ) != 0 )
			{	LogFont.lfHeight = -sldFontSize.GetPos();
				font.CreateFontIndirect( &LogFont );	//�t�H���g�̍쐬
				PlayTextEdit.SetFont( &font );
		}	}
*/
		CDialogEx::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR CMecaDokuDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMecaDokuDlg::OnSize(UINT nType, int cx, int cy)
{

	CDialogEx::OnSize(nType, cx, cy);
	PlayTextEdit.Resize();
	btnPlay.Resize();
	btnStop.Resize();
//	btnPause.Resize();
//	btnSetting.Resize();
//	btnDictionary.Resize();
	sldVolume.Resize();
	m_VolComment.Resize();
//	sldFontSize.Resize();
//	m_toCorpas.Resize();
	m_SarchBtn.Resize();
	m_SarchStr.Resize();
}


void CMecaDokuDlg::OnClickedPlay()
{//	char szBuf[16];
//	int i;

	if ( PlayTextEdit.GetLineCount() > 0 )
	{	CButton *pPlayBtn = ( CButton * )GetDlgItem( IDC_PLAY );

		btnPlay.EnableWindow( 0 );
		if ( iPlayState == STOP )
		{	iPlayState = PLAY;
			btnPlay.SetState( 1 );
//			btnPause.SetState( 0 );
			btnStop.SetState( 0 );
//			for ( i = 0; i< lengthof( lb ); i++ )
//			{	::lb[i].iLineNo = -1;
//			}
			EnterCriticalSection( &csQueCnt );
			iQueIn = 0;
			iQueOut = 0;
			iQueCnt = 0;
			LeaveCriticalSection( &csQueCnt );
			PostMessageW( UM_REQUEST_NEXTLINE, iNxtLineNo = 0, 0 );
			pThread = AfxBeginThread( preTalkThread, this, THREAD_PRIORITY_HIGHEST );
//			THREAD_PRIORITY_TIME_CRITICAL		THREAD_PRIORITY_HIGHEST	// add Priority
			PostMessageW( UM_REQUEST_NEXTLINE, iNxtLineNo = 1, 0 );
			iNxtLineNo = 0;
			pPlayBtn->SetWindowTextW( L"Pouse" );
//			PlayTextEdit.SetSel( 0, 0 );
			GotoDlgCtrl( GetDlgItem( IDC_PLAYTEXTEDIT ));
			int nFirstVisible = PlayTextEdit.GetFirstVisibleLine();
			if ( nFirstVisible > 0 )
			{	PlayTextEdit.LineScroll( -nFirstVisible, 0 );
		}	}
		else if ( iPlayState == PLAY )
		{	iPlayState = PAUSE;
			btnPlay.SetState( 0 );
//			btnPause.SetState( 1 );
			btnStop.SetState( 0 );
			pPlayBtn->SetWindowTextW( L"Play" );
		}
		else if ( iPlayState == PAUSE )
		{	// if ( OpenMecabDll() != NULL )
			{	iPlayState = PLAY;
				btnPlay.SetState( 1 );
//				btnPause.SetState( 0 );
				btnStop.SetState( 0 );
				pPlayBtn->SetWindowTextW( L"Pouse" );
				GotoDlgCtrl( GetDlgItem( IDC_PLAYTEXTEDIT ));
			}
		}
		btnPlay.EnableWindow( 1 );
	}
}


void CMecaDokuDlg::OnClickedPause()
{
#if 0
	btnPause.EnableWindow( 0 );
	switch ( iPlayState )
	{
	  case PLAY:
		iPlayState = PAUSE;
		btnPlay.SetState( 0 );
		btnPause.SetState( 1 );
		btnStop.SetState( 0 );
		break;
	  case PAUSE:
		iPlayState = PLAY;
		btnPlay.SetState( 1 );
		btnPause.SetState( 0 );
		btnStop.SetState( 0 );
		break;
	};
	btnPause.EnableWindow( 1 );
#endif
}


void CMecaDokuDlg::OnClickedStop()
{	CButton *pPlayBtn = ( CButton * )GetDlgItem( IDC_PLAY );

	iPlayState = STOP;
//	iPlayState = PRESTOP;
	pPlayBtn->SetWindowTextW( L"Play" );
	btnPlay.SetState( 0 );
//	btnPause.SetState( 0 );
	btnStop.SetState( 1 );
}


afx_msg LRESULT CMecaDokuDlg::OnUmRequestSelline(WPARAM wParam, LPARAM lParam)
{	int ss, se;

	ss = PlayTextEdit.LineIndex( ps.iLineNo );
	se = PlayTextEdit.LineIndex( ps.iLineNo + 1 ) - 1;
//	PlayTextEdit.UpdateData();
	PlayTextEdit.SetSel( ss, se );
//	PlayTextEdit.Invalidate();
//	PlayTextEdit.LineScroll( ps.iLineNo, 0 );
	return 0;
}



void CMecaDokuDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h���[ �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B
	CSliderCtrl *pSlider;

	pSlider = ( CSliderCtrl * )pScrollBar;
	if ( pSlider == &sldVolume )
	{	appVolSet( sldVolume.GetPos());
//		voice.vol = sldVolume.GetPos();
		return;
	}
#if 0
	else if ( pSlider == &sldFontSize )
	{
		LOGFONT LogFont;
		CFont*pFont = PlayTextEdit.GetFont();

		if ( pFont != NULL )
		{	if ( pFont->GetLogFont( &LogFont ) != 0 )
			{	LogFont.lfHeight = -sldFontSize.GetPos();
				font.CreateFontIndirect( &LogFont );	//�t�H���g�̍쐬
				PlayTextEdit.SetFont( &font );
		}	}

		PlayTextEdit.Invalidate();
		return;
	}
#endif
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CMecaDokuDlg::OnClickedSetting()
{	RECT rctThis, rctSetting;
//	pSettingTab->ShowWindow( SW_SHOW );

#if 0
	if ( pSetting->ShowWindow( SW_SHOWNA ) == 0 )
	{	GetWindowRect( &rctThis );					// ��ʍ��W
		pSetting->GetWindowRect( &rctSetting );		// ��ʍ��W
		pSetting->MoveWindow( rctThis.right, rctThis.top,
			rctSetting.right - rctSetting.left, rctSetting.bottom - rctSetting.top, FALSE );
		pSetting->ShowWindow( SW_SHOW );
	}
	else
	{	pSetting->ShowWindow( SW_HIDE );
	}
#else
	if ( pSettingTab->ShowWindow( SW_SHOWNA ) == 0 )
	{	GetWindowRect( &rctThis );					// ��ʍ��W
		pSettingTab->GetWindowRect( &rctSetting );		// ��ʍ��W
		pSettingTab->MoveWindow( rctThis.right, rctThis.top,
			rctSetting.right - rctSetting.left, rctSetting.bottom - rctSetting.top, FALSE );
		pSettingTab->ShowWindow( SW_SHOW );
	}
	else
	{	pSettingTab->ShowWindow( SW_HIDE );
	}
#endif
}


#if 0
void CMecaDokuDlg::OnClickedDictionaly()
{	RECT rctThis, rctSetting;

	if ( pUserDict->ShowWindow( SW_SHOWNA ) == 0 )
	{	GetWindowRect( &rctThis );					// ��ʍ��W
		pUserDict->GetWindowRect( &rctSetting );		// ��ʍ��W
		pUserDict->MoveWindow( rctThis.left, rctThis.bottom,
			rctSetting.right - rctSetting.left, rctSetting.bottom - rctSetting.top, FALSE );
		pUserDict->ShowWindow( SW_SHOW );
	}
	else
	{	pUserDict->ShowWindow( SW_HIDE );
	}
}
#endif


void CMecaDokuDlg::OnDestroy()
{	CMecaDokuApp *lpApp = ( CMecaDokuApp * )AfxGetApp();
	int i;
#if 1
	CHARFORMAT2 cf;
	DWORD dwMask;

	memset(( void * )&cf, 0, sizeof( cf ));
	dwMask = PlayTextEdit.GetDefaultCharFormat( cf );
	editcharHeight = cf.yHeight;
#else
	LOGFONT LogFont;

	if ( editFont.GetLogFont( &LogFont ) != 0 )
	{	editcharHeight = LogFont.lfHeight;
	}
#endif
	BeginWaitCursor();
	if ( pThread != NULL )
	{	iPlayState = STOP;
		WaitForSingleObject( pThread->m_hThread, INFINITE );
	}
//	for ( i = 0; i < lengthof( lb ); i++ )
//	{	if ( lb[i].pszLineBuf != NULL )
//		{	free( lb[i].pszLineBuf );
//	}	}
	for ( i = 0; i < lengthof( LineQue ); i++ )
	{	if ( LineQue[i].pszLine != NULL )
		{	free( LineQue[i].pszLine );
	}	}
	 
	pSarchCsv->DestroyWindow();
	delete pSarchCsv;
	pCorpasDlg->DestroyWindow();
	delete pCorpasDlg;

//	pSetting->DestroyWindow();
//	delete pSetting;
//	pUserDict->DestroyWindow();
//	delete pUserDict;

	pSettingTab->DestroyWindow();
	delete pSettingTab;

	editFont.DeleteObject();
	DeleteCriticalSection( &csQueCnt );
	CDialogEx::OnDestroy();
	lpApp->SaveProfile();
	if ( lpszRecentFile	!= NULL )
	{	free( lpszRecentFile );
	}
	EndWaitCursor();
}



#if 0
void CMecaDokuDlg::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// ���̋@�\�ɂ� Windows Vista �ȍ~�̃o�[�W�������K�v�ł��B
	// �V���{�� _WIN32_WINNT �� >= 0x0600 �ɂ���K�v������܂��B
	// TODO: �����Ƀ��b�Z�[�W �n���h���[ �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B
	if (( nFlags & MK_CONTROL ) != 0 )
	{	LOGFONT LogFont;

		if ( editFont.GetLogFont( &LogFont ) != 0 )
		{	//i = abs( LogFont.lfHeight );
			editFont.DeleteObject();
			LogFont.lfHeight += 10;
			editFont.CreateFontIndirect( &LogFont );
			PlayTextEdit.SetFont( &editFont );
		}
		return;
	}
#if 0

wParam
��ʃ��[�h�̓z�C�[������]���������i��]�ʁj�������BWHEEL_DELTA(120)�̔{���̌`�ŕ\�����B
���̒l�͑O(��)�A���̒l�͌��(��O)�։񂳂ꂽ�����Ӗ�����B
���ʃ��[�h�͂ǂ̃L�[��������Ă邩�������B

MK_CONTROL	Ctrl�L�[
MK_LBUTTON	���̃}�E�X�{�^����������Ă�
MK_MBUTTON	�^�񒆂̃}�E�X�{�^����������Ă�
MK_RBUTTON	�E
MK_SHIFT	Shift�L�[
MK_XBUTTON1	Windows 2000/XP: �ŏ���X�{�^����������Ă� (?)
MK_XBUTTON2	Windows 2000/XP: ����X�{�^����������Ă��� (?)
fwKeys = GET_KEYSTATE_WPARAM(wParam);
zDelta = GET_WHEEL_DELTA_WPARAM(wParam); // ���̒l�͕��ɂȂ邱�Ƃ�����

#endif
	CDialogEx::OnMouseHWheel(nFlags, zDelta, pt);
}

#endif


BOOL CMecaDokuDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊�{�N���X���Ăяo���Ă��������B
	if ( ::TranslateAccelerator( GetSafeHwnd(), m_hAccel, pMsg ))
	{	return TRUE;	// ESC �ŗ�����̂ŁA�������� 
	}
/*
typedef struct tagMSG {
    HWND        hwnd;
    UINT        message;
    WPARAM      wParam;
    LPARAM      lParam;
    DWORD       time;
    POINT       pt;
#ifdef _MAC
    DWORD       lPrivate;
#endif
} MSG, *PMSG, NEAR *NPMSG, FAR *LPMSG;
*/
	if ( pMsg->message == WM_MOUSEWHEEL )
	{	int id = FromHandle( pMsg->hwnd )->GetDlgCtrlID();	// �t�H�[�J�X�̃R���g���[��ID
		if ( id == IDC_PLAYTEXTEDIT )						// �t�H�[�J�X���G�f�B�b�g�R���g���[�� 
		{	unsigned short fwKeys = GET_KEYSTATE_WPARAM( pMsg->wParam );
			if (( fwKeys & MK_CONTROL ) != 0 )
			{
#if 1
				CHARFORMAT2 cf;
				DWORD dwMask;
				short zDelta = GET_WHEEL_DELTA_WPARAM( pMsg->wParam ); // ���̒l�͕��ɂȂ邱�Ƃ�����

				memset(( void * )&cf, 0, sizeof( cf ));
				dwMask = PlayTextEdit.GetDefaultCharFormat( cf );
				cf.yHeight += zDelta / 30;	//120;		// 120�P�ʂŕω�����炵�� 
				if ( cf.yHeight < 30 )
				{	cf.yHeight = 30;
				}
				cf.dwMask = CFM_SIZE;
				PlayTextEdit.SetDefaultCharFormat( cf );
#else
				LOGFONT LogFont;
				short zDelta = GET_WHEEL_DELTA_WPARAM( pMsg->wParam ); // ���̒l�͕��ɂȂ邱�Ƃ�����

				if ( editFont.GetLogFont( &LogFont ) != 0 )
				{	//i = abs( LogFont.lfHeight );
					editFont.DeleteObject();
					LogFont.lfHeight += zDelta / 120;		// 120�P�ʂŕω�����炵�� 
					if ( LogFont.lfHeight >= 0 )
					{	LogFont.lfHeight = -1;
					}
					editFont.CreateFontIndirect( &LogFont );
					PlayTextEdit.SetFont( &editFont );
					PlayTextEdit.Invalidate( FALSE );
				}
#endif
				return ( TRUE );
			}
		}
	}
	else if ( pMsg->message == WM_LBUTTONDBLCLK )
	{	int id = FromHandle( pMsg->hwnd )->GetDlgCtrlID();	// �t�H�[�J�X�̃R���g���[��ID
		if ( id == IDC_PLAYTEXTEDIT )						// �t�H�[�J�X���G�f�B�b�g�R���g���[�� 
		{	unsigned short fwKeys = GET_KEYSTATE_WPARAM( pMsg->wParam );
			if (( fwKeys & ( MK_CONTROL | MK_SHIFT )) == ( MK_CONTROL | MK_SHIFT ))
			{	CPoint pt;

				pt.x = GET_X_LPARAM( pMsg->lParam );
				pt.y = GET_Y_LPARAM( pMsg->lParam );
				int chrPos = PlayTextEdit.CharFromPos( pt );
				iNxtLineNo = PlayTextEdit.LineFromChar( chrPos );
			}
		}
//lParam
//���ʃ��[�h�́A�J�[�\���� x ���W���w�肵�܂��B ���W�́A�N���C�A���g�̈�̍��������Ƃ��Ă��܂��B
//��ʃ��[�h�́A�J�[�\���� y ���W���w�肵�܂��B ���W�́A�N���C�A���g�̈�̍��������Ƃ��Ă��܂��B
	}
	else if ( pMsg->message == WM_DROPFILES )
	{	int id = FromHandle( pMsg->hwnd )->GetDlgCtrlID();	// �t�H�[�J�X�̃R���g���[��ID
		if ( id == IDC_PLAYTEXTEDIT )						// �t�H�[�J�X���G�f�B�b�g�R���g���[�� 
		{	HDROP hDropInfo = ( HDROP )pMsg->wParam;
#if 0
			TCHAR	filename[MAX_PATH + 8];
			int stat;

			if (( stat = ::DragQueryFile( hDropInfo, 0, filename, lengthof( filename ))) < 0 )
			{	return ( TRUE );
			}
			AfxMessageBox( filename );
//			CEdit
#endif
			OnDropFiles( hDropInfo );
			return ( TRUE );
		}
	}
	else if ( pMsg->message == WM_VSCROLL )
	{	int id = FromHandle( pMsg->hwnd )->GetDlgCtrlID();	// �t�H�[�J�X�̃R���g���[��ID
		if ( id == IDC_PLAYTEXTEDIT )						// �t�H�[�J�X���G�f�B�b�g�R���g���[�� 
		{	HDROP hDropInfo = ( HDROP )pMsg->wParam;
			TCHAR	filename[MAX_PATH + 8];
//			int stat;

			AfxMessageBox( filename );
//			CEdit
		}
	}
	else if ( pMsg->message == WM_KEYDOWN )
	{	if (( pMsg->wParam == VK_RETURN ) || ( pMsg->wParam == VK_ESCAPE ))
		{	return ( FALSE );
	}	}
//	else if ( pMsg->message == ENM_PROTECTED )
//	{	return ( FALSE );
//	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


char *hgets( char *pbuf, int buffsize, HANDLE fh )
{	static char preBuff[MAX_LINE * 2 + 1] = { '\0' };
	static int	inxIn = 0, inxOut = 0, cbBuf = 0;
	char buildBuff[( MAX_LINE + 1 ) * 2];
	BOOL	stat;
	char	*pCr;
	int		lineLen, precatlen, uniLen;

	*pbuf = buildBuff[0] = '\0';
	for ( ; ; )
	{	if ( cbBuf > 0 )
		{	if (( pCr = strchr( &preBuff[inxOut], '\r' )) != NULL )
			{	break;
			}
			if ( cbBuf >= buffsize )
			{	pCr = &preBuff[inxOut + buffsize];
				break;
			}
			strcat_s( buildBuff, sizeof( buildBuff ), &preBuff[inxOut]);
		}
//		seekPos = SetFilePointer( fh, 0l, NULL, FILE_CURRENT );
		stat = ReadFile( fh, preBuff, ( DWORD )( sizeof( preBuff ) - 2 ), ( LPDWORD )&cbBuf, NULL );
		if ( stat == FALSE )
		{	int err;

			if (( err = GetLastError()) == ERROR_HANDLE_EOF )
			{	if ( *buildBuff != '\0' )
				{	
					return ( pbuf );
			}	}
			return ( NULL );
		}
		preBuff[cbBuf] = '\0';
		inxOut = 0;
		if ( cbBuf <= 0 )
		{	pCr = &preBuff[0];
			break;
		}
		if ( nCodePage == 0 )		// ( seekPos == 0 )	// ����̓ǂݍ��݁H 
		{	CoInitialize( NULL );	// �����R�[�h���� 
			IMultiLanguage2 *pIMulLang;
			if ( CoCreateInstance( CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER,
				IID_IMultiLanguage2, ( void** )&pIMulLang ) == S_OK )
			{	int					detectEncCount = 1, cbBufcpy = cbBuf;
				DetectEncodingInfo	detectEnc;

				// DetectInputCodepage�֐��̏ꍇ
				//   byte *data    - ���ׂ����f�[�^
				//   int   dataLen - ���ׂ����f�[�^�̃o�C�g����n���A
				//                   �������ꂽ�o�C�g�����󂯎��
				if ( pIMulLang->DetectInputCodepage(
					MLDETECTCP_NONE, 0, preBuff, &cbBufcpy,
						&detectEnc, &detectEncCount ) == S_OK )
				{	nCodePage = detectEnc.nCodePage;
					// �R�[�h�y�[�W�� UNICODE �ȊO�̏ꍇ ANSI �ɕύX		
					if ( nCodePage == 20127 )	// �R�[�h�y�[�W�� 20127(US-ASCII) �̏ꍇ UTF-8 �ɕύX
					{	nCodePage = 65001;		// ���p�R�[�h�����j�R�[�h�ɕύX����ƃo�C�g�T�C�Y�͔{�ɁI
					}
					else if (( nCodePage != 1200 ) &&
						( nCodePage != 1201 ) && ( nCodePage != 65001 ))
					{	nCodePage = 932;		// ���p�R�[�h�����j�R�[�h�ɕύX����ƃo�C�g�T�C�Y�͔{�ɁI
				}	}
				pIMulLang->Release();
			}
			CoUninitialize();
	}	}
	precatlen = ( int )strlen( buildBuff );
	lineLen = ( int )( pCr - &preBuff[inxOut] + 1 );
	if (( precatlen + lineLen ) >= sizeof( buildBuff ))
	{	lineLen = sizeof( buildBuff ) - precatlen - 1;
	}
	strncpy_s( &( buildBuff[precatlen]), sizeof( buildBuff ) - precatlen, &preBuff[inxOut], lineLen );
	buildBuff[precatlen + lineLen] = '\0';
	if ( precatlen + lineLen >= sizeof( buildBuff ))
	{	buildBuff[precatlen + lineLen] = '\0';
	}
	inxOut += lineLen;
	cbBuf -= lineLen;
	if ( preBuff[inxOut] == '\n' )
	{	inxOut++;
		cbBuf--;
	}
	if ( buildBuff[0] != '\0' )
	{	uniLen = CodePageToUni( nCodePage, buildBuff, pbuf, buffsize );
		if ( uniLen > buffsize )
		{	return ( pbuf );
		}
		return ( pbuf );
	}
	return ( NULL );
}


#define isUniHankaku(x)		(((0<x)&&(x<0x7f))||((0xff61<=x)&&(x<=0xff9f)))
static DWORD CALLBACK StreamInCallback( DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb )
{	char preBuf[MAX_LINE * sizeof( wchar_t ) * 2 + 1];
	wchar_t *p;
	ATLASSERT( dwCookie != 0 );
	ATLASSERT( pcb != NULL );

	do
	{	if ( hgets( preBuf, sizeof( preBuf ), ( HANDLE )dwCookie ) == NULL )
		{	return ( TRUE );
		}
//		UTF8ToUni( preBuf, ( char * )pbBuff, cb );
		for ( p = ( wchar_t * )preBuf; *p != L'\0'; p++ )
		{	if (( p = wcschr( p, L' ' )) == NULL )	// ���p�󔒃T�[�` 
			{	break;		// ���̂���b�̓r���ɔ��p�󔒂�����AMeCab�̎ז���
			}				// ���Ă����̂őS�p�����ɋ��܂ꂽ���p�󔒂��폜����B
			if ( p != ( wchar_t * )preBuf )			// �o�b�t�@�擪�Ȃ�p�X 
			{	if ( !isUniHankaku( *( p - 1 )) && !isUniHankaku( *( p + 1 )))
				{	wcscpy( p, p + 1 );
		}	}	}
		wcscpy(( wchar_t * )pbBuff, ( wchar_t * )preBuf );
		*pcb = ( long )( wcslen(( wchar_t * )pbBuff ) * sizeof( wchar_t ));
		Sleep( 0 );
	} while ( *pcb == 0 );
	return( NULL );
}


//typedef struct _OVERLAPPED {
//  ULONG_PTR Internal;
//  ULONG_PTR InternalHigh;
//  union {
//    struct {
//      DWORD Offset;
//      DWORD OffsetHigh;
//    } ;
//    PVOID  Pointer;
//  } ;
//  HANDLE    hEvent;
//} OVERLAPPED, *LPOVERLAPPED;


void CMecaDokuDlg::LoadPdf( wchar_t *szFile )
{
	nCodePage = 0;
	HANDLE hFile = ::CreateFile( szFile, GENERIC_READ, 0, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
	{	return;
	}
	PlayTextEdit.SetSel( 0, -1 );
	PlayTextEdit.Cut();
	EDITSTREAM es;					/// �X�g���[�����g���ēǂݍ���
//typedef struct _editstream {
//  DWORD_PTR          dwCookie;
//  DWORD              dwError;
//  EDITSTREAMCALLBACK pfnCallback;
//} EDITSTREAM;
	es.dwCookie = ( DWORD )hFile;
	es.dwError = 0;
	es.pfnCallback = ( EDITSTREAMCALLBACK )&StreamInCallback;
	PlayTextEdit.StreamIn( SF_TEXT | SF_UNICODE, es );
	::CloseHandle( hFile );


}


void CMecaDokuDlg::LoadText( wchar_t *szFile )	// txt loader
{
#if 1
	nCodePage = 0;
	HANDLE hFile = ::CreateFile( szFile, GENERIC_READ, 0, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
	{	return;
	}
	if ( lpszRecentFile != szFile )
	{	if ( lpszRecentFile != NULL )
		{	free( lpszRecentFile );
		}
		lpszRecentFile = wcsdup( szFile );
	}
	{	wchar_t szTitle[MAX_PATH + 20], *p;

		GetWindowText( szTitle, lengthof( szTitle ));
		if (( p = wcsstr( szTitle, L" - " )) != NULL )
		{	wcscpy( p + 3, szFile );
		}
		else
		{	wcscat( szTitle, L" - " );
			wcscat( szTitle, szFile );
		}
		SetWindowText( szTitle );
	}
// �ǂݍ��݌�ɒǋL�ł��Ȃ��̂�EditCtrl�̋��e�T�C�Y���t�@�C���T�C�Y�{���ɐݒ肵�Ă���  
	DWORD needLen = GetFileSize( szFile ) + 4095;	// �ҏW�A�ǉ��padd
	DWORD orgLen = PlayTextEdit.GetLimitText();
	if ( needLen > orgLen )
	{	PlayTextEdit.LimitText( needLen );
	}
	PlayTextEdit.SetSel( 0, -1 );
	PlayTextEdit.Cut();

	EDITSTREAM es;					/// �X�g���[�����g���ēǂݍ���
//typedef struct _editstream {
//  DWORD_PTR          dwCookie;
//  DWORD              dwError;
//  EDITSTREAMCALLBACK pfnCallback;
//} EDITSTREAM;
	es.dwCookie = ( DWORD )hFile;
	es.dwError = 0;
	es.pfnCallback = ( EDITSTREAMCALLBACK )&StreamInCallback;
	PlayTextEdit.StreamIn( SF_TEXT | SF_UNICODE, es );
	::CloseHandle( hFile );
#else
	DWORD size;
	FILE *csvFp;
	char szLine[2048];
	wchar_t szUniLine[4096], *pszBuff, *plf;

	if (( size = GetFileSize( szFile )) > 0 )		//	�t�@�C���T�C�Y�擾	����@
	{	size = size / 10 * 13;							// �R�O���i�����Z�b�g�ϊ������j�H�����Ńo�b�t�@���m��
		if (( pszBuff = ( wchar_t * )calloc( size, sizeof( wchar_t ))) != NULL )
		{	memset( pszBuff, 0, size );
			if (( csvFp = _wfopen( szFile, L"rt+" )) != NULL )
			{	for ( ; !feof( csvFp ); )
				{	if ( fgets(( char * )szLine, lengthof( szLine ), csvFp ) == NULL )
					{	continue;
					}
					CodePageToUni( 65001, szLine, ( char * )&szUniLine[0], lengthof( szUniLine ));
//					cvtutf82Uni(( char * )szLine, ( char * )&szUniLine[0], lengthof( szUniLine ));
					if (( plf = wcschr( szUniLine, L'\n' )) != NULL )
					{	wcscpy( plf, L"\r\n" );
					}
					wcscat( pszBuff, szUniLine );
				}
				fclose( csvFp );
			}
			PlayTextEdit.SetWindowTextW( pszBuff );
			free( pszBuff );
	}	}
#endif
}


void CMecaDokuDlg::OnDropFiles(HDROP hDropInfo)
{	wchar_t szFile[MAX_PATH + 1];
	wchar_t ext[_MAX_EXT];

	if ( ::DragQueryFile( hDropInfo, -1, NULL, 0 ) <= 0 )
	{	return;
	}
	if ( ::DragQueryFile( hDropInfo, 0, szFile, lengthof( szFile )) < 0 )
	{	return;
	}
	::DragFinish( hDropInfo );
	{	wchar_t drive[_MAX_DRIVE];
		wchar_t dir[_MAX_DIR];
		wchar_t fname[_MAX_FNAME];

		_wsplitpath( szFile, drive, dir, fname, ext );
	}
/*
	if ( wcscmp( ext, L".pdf" ) == 0 )
	{	LoadPdf( szFile );				// pdf loader
		return;
	}
	else
*/
	if ( wcscmp( ext, L".txt" ) != 0 )
	{	return;
	}
	LoadText( szFile );				// txt loader
//	CDialogEx::OnDropFiles(hDropInfo);
}


#if 0
void CMecaDokuDlg::OnClicked2corpas()
{	RECT rctThis, rctSetting;
	wchar_t srcstr[MAX_LINE], *p, *pnonSpace;

	if ( pCorpasDlg->ShowWindow( SW_SHOWNA ) == 0 )
	{	GetWindowRect( &rctThis );						// ��ʍ��W
		pCorpasDlg->GetWindowRect( &rctSetting );		// ��ʍ��W
		pCorpasDlg->MoveWindow( rctThis.right, rctThis.top,
			rctSetting.right - rctSetting.left, rctSetting.bottom - rctSetting.top, FALSE );
		pCorpasDlg->ShowWindow( SW_SHOW );
	}
#if 1
	long len = PlayTextEdit.GetSelText(( LPSTR )srcstr );
	while (( p = wcschr( srcstr, L'\r' )) != NULL )	// srcstr����@CRLF���폜����I
	{	wcscpy( p, p + 1 );
	}
#else
	if ( OpenClipboard())
	{	EmptyClipboard();
		CloseClipboard();
	}
	PlayTextEdit.Copy();
	if ( OpenClipboard())
	{	HGLOBAL	hMem;			// �擾�p�̃������ϐ�
		LPTSTR	lpBuff;			// �Q�Ɨp�̃|�C���^

		if (( hMem = GetClipboardData( CF_UNICODETEXT )) == NULL )
		{	CloseClipboard();
			return;
		}
		if (( lpBuff = ( LPTSTR )GlobalLock( hMem )) == NULL )
		{	CloseClipboard();
			return;
		}
		wcsncpy( srcstr, lpBuff, lengthof( srcstr ) - 1 );
		srcstr[MAX_LINE - 1] = L'\0';
		GlobalUnlock( hMem );
		CloseClipboard();
	}
	while (( p = wcschr( srcstr, L'\r' )) != NULL )	// srcstr����@CRLF���폜����I
	{	wcscpy( p, p + 2 );
	}
#endif
	for ( pnonSpace = srcstr;
		( *pnonSpace == L' ' ) || ( *pnonSpace == L'�@' ) || ( *pnonSpace == L'\t' ); pnonSpace++ )
	{	;						// �s���̋󔒂��X�L�b�v���� MeCab���s���󔒂Ō�邽�ߒǉ� 
	}
	if ( *pnonSpace != L'\0' )	// ��s�łȂ���� 
	{	pCorpasDlg->toCorpas( srcstr );
	}
}
#endif


BOOL CMecaDokuDlg::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.dwExStyle |= WS_EX_ACCEPTFILES;	//�t�@�C���̃h���b�v���󂯕t����

	return CDialogEx::PreCreateWindow(cs);
}


int CMecaDokuDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �����ɓ���ȍ쐬�R�[�h��ǉ����Ă��������B

	return 0;
}


void CMecaDokuDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point )
{	CRect rc;
	CMenu cMenu, *menuPopup;
	CButton *pPlayBtn = ( CButton * )GetDlgItem( IDC_PLAY );

	if (( point.x == -1 ) && ( point.y == -1 ))
	{	LONG start, end;

		PlayTextEdit.GetSel( start, end );
		point = PlayTextEdit.PosFromChar( end );
		PlayTextEdit.ClientToScreen( &point );
	}
	// ���W�����b�`�G�f�B�b�g�R���g���[�����̏ꍇ�̂݃|�b�v�A�b�v���j���[��\��
	if ( iPlayState == PLAY )
	{	iPlayState = PAUSE;
		btnPlay.SetState( 0 );
		btnStop.SetState( 0 );
		pPlayBtn->SetWindowTextW( L"PLAY" );
	}
	PlayTextEdit.GetClientRect( &rc );
	PlayTextEdit.ClientToScreen( &rc );
	if ( rc.PtInRect( point ))
	{	cMenu.LoadMenu( IDR_POPUP_MAIN );
		menuPopup = cMenu.GetSubMenu( 0 );
		if ( iPlayState == PLAY )
		{	menuPopup->EnableMenuItem( ID__PLAYTHIS, MF_GRAYED | MF_BYCOMMAND );
		}
		menuPopup->CheckMenuItem( ID__KIGOUPASS, (( fgKigouPass )? MF_CHECKED: MF_UNCHECKED ) | MF_BYCOMMAND );
		menuPopup->CheckMenuItem( ID__PAUSEDSPOFF, (( fgPausedspoff )? MF_CHECKED: MF_UNCHECKED ) | MF_BYCOMMAND );
		menuPopup->CheckMenuItem( ID__SKIPSPEAK, (( fgSkipspeak )? MF_CHECKED: MF_UNCHECKED ) | MF_BYCOMMAND );
		if ( lpszRecentFile != NULL )	// �O��h���b�v�����t�@�C�������邩�H 
		{	menuPopup->AppendMenu( MF_SEPARATOR, 0, ( wchar_t * )NULL );
			menuPopup->AppendMenu( MF_STRING, WM_LOADRECENTFILE, lpszRecentFile );
		}
		menuPopup->TrackPopupMenu(
			TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON,
			point.x, point.y, this );
		cMenu.DestroyMenu();	// DestroyMenu( menuPopup );
	}
	if ( iPlayState == PAUSE )
	{	iPlayState = PLAY;
		btnPlay.SetState( 1 );
		btnStop.SetState( 0 );
		pPlayBtn->SetWindowTextW( L"POUSE" );
//		GotoDlgCtrl( GetDlgItem( IDC_PLAYTEXTEDIT ));
	}
}


void CMecaDokuDlg::OnCopy()
{	PlayTextEdit.Copy();
}


long GetClipbordTextLength()
{	HANDLE hText;
	char *pText;
	long len = 0;

	OpenClipboard( NULL );
	hText = GetClipboardData( CF_TEXT );
	if ( hText != NULL )
	{	pText = ( char * )GlobalLock( hText );
		len = ( long )(( wcslen(( const wchar_t * )pText ) + 1 ) * sizeof( wchar_t ));
		GlobalUnlock( hText );
	}
	CloseClipboard();
	return ( len );
}


void CMecaDokuDlg::OnPaste()
{	long orgLen, needLen;

	if (( needLen = GetClipbordTextLength()) > 0 )
	{	orgLen = PlayTextEdit.GetLimitText();
		needLen += 4095;						// �ҏW�A�ǉ��padd
		if ( needLen > orgLen )
		{	PlayTextEdit.LimitText( needLen );
		}
//		PlayTextEdit.Paste();
		PlayTextEdit.PasteSpecial( CF_TEXT );
		// PlayTextEdit.LimitText( orgLen );
		if ( lpszRecentFile != NULL )
		{	free( lpszRecentFile );
			lpszRecentFile = NULL;
			{	wchar_t szTitle[MAX_PATH + 20], *p;

				GetWindowText( szTitle, lengthof( szTitle ));
				if (( p = wcsstr( szTitle, L" - " )) != NULL )	// �^�C�g���Ƀt�@�C�������\������Ă����� 
				{	*p = EOS;									// ���� 
					SetWindowText( szTitle );
	}	}	}	}
}


void CMecaDokuDlg::OnDel()
{	PlayTextEdit.Cut();
}


void CMecaDokuDlg::OnUndo()
{	PlayTextEdit.Undo();
}


void CMecaDokuDlg::OnCorpas()
{	long	nStartChar, nEndChar;
	wchar_t targetstr[MAX_LINE], *p, *pnonSpace;
	RECT rctThis, rctSetting;

	if ( pCorpasDlg->ShowWindow( SW_SHOWNA ) == 0 )
	{	GetWindowRect( &rctThis );						// ��ʍ��W
		pCorpasDlg->GetWindowRect( &rctSetting );		// ��ʍ��W
		pCorpasDlg->MoveWindow( rctThis.right, rctThis.top,
			rctSetting.right - rctSetting.left, rctSetting.bottom - rctSetting.top, FALSE );
		pCorpasDlg->ShowWindow( SW_SHOW );
	}
	PlayTextEdit.GetSel( nStartChar, nEndChar );
	if (( nEndChar - nStartChar + 2 ) >= lengthof( targetstr ))
	{	return;
	}
	PlayTextEdit.GetSelText(( LPSTR )targetstr );
	while (( p = wcschr( targetstr, L'\r' )) != NULL )	// targetstr����@CRLF���폜����I
	{	wcscpy( p, p + 1 );
	}
	for ( pnonSpace = targetstr;
		( *pnonSpace == L' ' ) || ( *pnonSpace == L'�@' ) || ( *pnonSpace == L'\t' ); pnonSpace++ )
	{	;						// �s���̋󔒂��X�L�b�v���� MeCab���s���󔒂Ō�邽�ߒǉ� 
	}
	if ( *pnonSpace != L'\0' )	// ��s�łȂ���� 
	{	pCorpasDlg->toCorpas( pnonSpace );
	}
}


void CMecaDokuDlg::OnAllclr()
{	PlayTextEdit.SetSel( 0, -1 );
	PlayTextEdit.Cut();
}


void CAboutDlg::OnCancel()
{//	CDialogEx::OnCancel();
}


BOOL CMecaDokuDlg::DestroyWindow()
{	return CDialogEx::DestroyWindow();
}


void CMecaDokuDlg::OnClearbef()
{	long nStartChar, nEndChar, baseLine;

	PlayTextEdit.GetSel( nStartChar, nEndChar );
	PlayTextEdit.SetSel( 0, PlayTextEdit.LineIndex( baseLine = PlayTextEdit.LineFromChar( nStartChar )) - 1 );
	PlayTextEdit.Cut();
	iNxtLineNo -= baseLine;		// �J�����g�s���� 
}


void CMecaDokuDlg::OnPlaythis()
{	long nStartChar, nEndChar;
	CButton *pPlayBtn = ( CButton * )GetDlgItem( IDC_PLAY );

	PlayTextEdit.GetSel( nStartChar, nEndChar );
	iNxtLineNo = PlayTextEdit.LineFromChar( nStartChar ) - 1;
	btnPlay.SetState( 1 );
	btnStop.SetState( 0 );
//	btnPause.SetState( 0 );
//	for ( i = 0; i< lengthof( lb ); i++ )
//	{	::lb[i].iLineNo = -1;
//	}
	EnterCriticalSection( &csQueCnt );
	iQueIn = 0;
	iQueOut = 0;
	iQueCnt = 0;
	LeaveCriticalSection( &csQueCnt );
	PostMessageW( UM_REQUEST_NEXTLINE, iNxtLineNo, 0 );
	if ( iPlayState == STOP )
	{	pThread = AfxBeginThread( preTalkThread, this );
	}
	PostMessageW( UM_REQUEST_NEXTLINE, iNxtLineNo++, 0 );
	iPlayState = PLAY;
	pPlayBtn->SetWindowTextW( L"POUSE" );
//	PlayTextEdit.SetSel( 0, 0 );
	GotoDlgCtrl( GetDlgItem( IDC_PLAYTEXTEDIT ));
	int nFirstVisible = PlayTextEdit.GetFirstVisibleLine();
	if ( nFirstVisible > 0 )
	{	PlayTextEdit.LineScroll( iNxtLineNo - 1 - nFirstVisible, 0 );
	}
}



UINT CMecaDokuDlg::OnPowerBroadcast(UINT nPowerEvent, UINT nEventData)
{
	if (( nPowerEvent == PBT_POWERSETTINGCHANGE ) && ( fgPausedspoff != FALSE ))
	{	POWERBROADCAST_SETTING *ppbcs = ( POWERBROADCAST_SETTING * )nEventData;
		if ( ppbcs->PowerSetting == GUID_CONSOLE_DISPLAY_STATE )
		{	if ( ppbcs->Data[0] == 0 )				// �f�B�X�v���C��OFF
			{	if ( iPlayState == PLAY )
				{	OnClickedPlay();				// PLAY���Ȃ�PAUSE
#if 0												// �����Đ��X�^�[�g�͎~�܂��Ă����ǂ����s���ɂȂ�̂� 
			}	}
			else									// �f�B�X�v���CON
			{	if ( iPlayState == PAUSE )
				{	OnClickedPlay();				// PAUSE���Ȃ�PLAY
#endif
	}	}	}	}
	return CDialogEx::OnPowerBroadcast(nPowerEvent, nEventData);
}


void CMecaDokuDlg::OnClickedSarch()
{	wchar_t szSarchStr[256];
	int ctSarchStr;
	long nStartChar, nEndChar;
	DWORD flags = FR_DOWN;
	FINDTEXTEXW stFindTxt;

	memset(( char * )szSarchStr, 0, sizeof( szSarchStr ));
	ctSarchStr = m_SarchStr.GetLine( 0, szSarchStr, sizeof( szSarchStr ) - 1 );
	if ( ctSarchStr <= 0 )
	{	PlaySound(( LPCTSTR )SND_ALIAS_SYSTEMQUESTION, NULL, SND_ASYNC | SND_ALIAS_ID );
		return;
	}
//	flags = (( GetKeyState( VK_SHIFT ) & 0x8000 ))? 0: FR_DOWN;
	PlayTextEdit.GetSel( nStartChar, nEndChar );
	stFindTxt.chrg.cpMin = nEndChar;
	stFindTxt.chrg.cpMax = -1;
	stFindTxt.lpstrText = szSarchStr;
	stFindTxt.chrgText.cpMax = stFindTxt.chrgText.cpMin = 0;
	if ( PlayTextEdit.FindText( flags, &stFindTxt ) < 0 )
	{	stFindTxt.chrg.cpMin = 0;
		stFindTxt.chrg.cpMax = nStartChar;
		stFindTxt.lpstrText = szSarchStr;
		stFindTxt.chrgText.cpMax = stFindTxt.chrgText.cpMin = 0;
		if ( PlayTextEdit.FindText( flags, &stFindTxt ) < 0 )
		{	return;
	}	}
	if (( ctSarchStr = PlayTextEdit.LineFromChar( stFindTxt.chrgText.cpMin )) > 2 )
	{	PlayTextEdit.LineScroll( ctSarchStr - 2 );
	}
	PlayTextEdit.SetSel( stFindTxt.chrgText.cpMin, stFindTxt.chrgText.cpMax );
}


void CMecaDokuDlg::OnPausedspoff()
{	fgPausedspoff = !fgPausedspoff;
}


void CMecaDokuDlg::OnSkipspeak()
{	fgSkipspeak = !fgSkipspeak;
}


void CMecaDokuDlg::OnKigoupass()
{	fgKigouPass = !fgKigouPass;
}


void CMecaDokuDlg::OnLoadrecentfile()
{	if ( lpszRecentFile	!= NULL )
	{	BeginWaitCursor();
		LoadText( lpszRecentFile );
		while ( PlayTextEdit.GetLineCount() < iNxtLineNo )
		{	_sleep( 500 );		// �X�g���[�����͂����̍s�܂œǂݍ��ނ܂ő҂� 
		}						// �҂��Ȃ��Ɛ������X�N���[���o���Ȃ� 
		if ( iNxtLineNo > 2 )
		{	PlayTextEdit.LineScroll( iNxtLineNo - 2 );
		}
		EndWaitCursor();
	}
}


