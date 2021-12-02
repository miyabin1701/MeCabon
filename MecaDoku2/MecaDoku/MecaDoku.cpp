
// MecaDoku.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
//

#include "stdafx.h"
#include "MecaDoku.h"
#include "MecaDokuDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

wchar_t	szMeCabExe[_MAX_PATH];
wchar_t	szUserDicCsv[_MAX_PATH];
wchar_t	szUserDictionary[_MAX_PATH];
wchar_t	szSysDictionary[_MAX_PATH];
wchar_t	szSysDicWork[_MAX_PATH];
wchar_t	szTextEditor[_MAX_PATH];
wchar_t	szEditOpt[64];
wchar_t	szCharSetUserCsv[32]		= L"UTF-8";
wchar_t	szCharSetUserDic[32]		= L"UTF-8";
wchar_t	*szLeftIdDefFname			= L"left-id.def";
wchar_t	*szRightIdDefFname			= L"right-id.def";
wchar_t	*szPosIdDefFname			= L"pos-id.def";
BOOL	fgPausedspoff				= TRUE;
BOOL	fgKigouPass					= TRUE;
BOOL	fgBgBlack					= FALSE;


const wchar_t	lpszEnvSection[]		= L"MeCabDoku";
const wchar_t	lpszMeCabPath[]			= L"mecab_exe";
const wchar_t	lpszUserCsv[]			= L"user_Csv";
const wchar_t	lpszUserDic[]			= L"user_dic";
const wchar_t	lpszSysDic[]			= L"sys_dic";
const wchar_t	lpszSysDicWork[]		= L"SysDicWork";
const wchar_t	lpszTextEditor[]		= L"TextEditor";
const wchar_t	lpszEditOpt[]			= L"EditOpt";
const wchar_t	lpszeditcharHeight[]	= L"EditCharHeight";
const wchar_t	lpszSelectTts[]			= L"TtsSel";
const wchar_t	lpszsapi54iRate[]		= L"Sapi54Rate";
const wchar_t	lpszsapi54iVol[]		= L"Sapi54Vol";
const wchar_t	lpszsapi54iModeSel[]	= L"SapiModeSel";
const wchar_t	lpszsapi54iVoiceSel[]	= L"SapiVoiceSel";
const wchar_t	lpszsapi54iMuonMs[]		= L"SapiMuonMs";
const wchar_t	lpszfgPausedspoff[]		= L"fgPausedspoff";
const wchar_t	lpszfgKigouPass[]		= L"fgKigouPass";
const wchar_t	lpszfgBgBlack[]			= L"lpszfgBgBlack";

const wchar_t	lpszRecentFileName[]	= L"RecentFileName";
const wchar_t	lpszRecentFileLine[]	= L"RecentFileLine";

LONG	editcharHeight;

TTSNAME		SelectTts = SAPI54;		
struct SAPI54SETTING	sapi54 = { 0, 10, 0, 0, 50, NULL };
wchar_t	*lpszRecentFile	= NULL;
extern int	iNxtLineNo;
extern wchar_t *exchgFileName( wchar_t *pszDest, wchar_t *pszPath, wchar_t *pszfileName );


// CMecaDokuApp

BEGIN_MESSAGE_MAP(CMecaDokuApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMecaDokuApp �R���X�g���N�V����

CMecaDokuApp::CMecaDokuApp()
{
	// �ċN���}�l�[�W���[���T�|�[�g���܂�
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: ���̈ʒu�ɍ\�z�p�R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
}


// �B��� CMecaDokuApp �I�u�W�F�N�g�ł��B

CMecaDokuApp theApp;


// CMecaDokuApp ������

BOOL CMecaDokuApp::InitInstance()
{
	// �A�v���P�[�V���� �}�j�t�F�X�g�� visual �X�^�C����L���ɂ��邽�߂ɁA
	// ComCtl32.dll Version 6 �ȍ~�̎g�p���w�肷��ꍇ�́A
	// Windows XP �� InitCommonControlsEx() ���K�v�ł��B�����Ȃ���΁A�E�B���h�E�쐬�͂��ׂĎ��s���܂��B
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// �A�v���P�[�V�����Ŏg�p���邷�ׂẴR���� �R���g���[�� �N���X���܂߂�ɂ́A
	// �����ݒ肵�܂��B
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();
	AfxInitRichEdit2();

	// �_�C�A���O�ɃV�F�� �c���[ �r���[�܂��̓V�F�� ���X�g �r���[ �R���g���[����
	// �܂܂�Ă���ꍇ�ɃV�F�� �}�l�[�W���[���쐬���܂��B
	CShellManager *pShellManager = new CShellManager;

	// �W��������
	// �����̋@�\���g�킸�ɍŏI�I�Ȏ��s�\�t�@�C����
	// �T�C�Y���k���������ꍇ�́A�ȉ�����s�v�ȏ�����
	// ���[�`�����폜���Ă��������B
	// �ݒ肪�i�[����Ă��郌�W�X�g�� �L�[��ύX���܂��B
	// TODO: ��Ж��܂��͑g�D���Ȃǂ̓K�؂ȕ������
	// ���̕������ύX���Ă��������B
	SetRegistryKey( _T( "Mecabon" ));

	CMecaDokuDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �_�C�A���O�� <OK> �ŏ����ꂽ���̃R�[�h��
		//  �L�q���Ă��������B
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �_�C�A���O�� <�L�����Z��> �ŏ����ꂽ���̃R�[�h��
		//  �L�q���Ă��������B
	}

	// ��ō쐬���ꂽ�V�F�� �}�l�[�W���[���폜���܂��B
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// �_�C�A���O�͕����܂����B�A�v���P�[�V�����̃��b�Z�[�W �|���v���J�n���Ȃ���
	//  �A�v���P�[�V�������I�����邽�߂� FALSE ��Ԃ��Ă��������B
	return FALSE;
}


void CMecaDokuApp::LoadProfile()
{	wchar_t	RecentFile[_MAX_PATH];
	wchar_t	selfpath[_MAX_PATH], *p;

    ::GetModuleFileName( NULL, selfpath, sizeof( selfpath ));
	exchgFileName( selfpath, selfpath, L"mecab-0.996\\work\\final" );		// 21/12/02
	editcharHeight = ( long )GetProfileInt( lpszEnvSection, lpszeditcharHeight, -10 );
	SelectTts = ( TTSNAME )GetProfileInt( lpszEnvSection, lpszSelectTts, 0 );
	sapi54.iRate = ( long )GetProfileInt( lpszEnvSection, lpszsapi54iRate, 0 );
	sapi54.iVol = ( long )GetProfileInt( lpszEnvSection, lpszsapi54iVol, 10 );
	sapi54.iModeSel = ( long )GetProfileInt( lpszEnvSection, lpszsapi54iModeSel, 0 );
	sapi54.iVoiceSel = ( long )GetProfileInt( lpszEnvSection, lpszsapi54iVoiceSel, 0 );
	sapi54.iMuonMs = ( long )GetProfileInt( lpszEnvSection, lpszsapi54iMuonMs, 50 );
	wcscpy_s( szMeCabExe, lengthof( szMeCabExe ), ( LPCTSTR )GetProfileString( lpszEnvSection, lpszMeCabPath, L"C:\\Program Files (x86)\\MeCab\\bin\\mecab.exe" ));	//21/12/01
//	wcscpy_s( szMeCabExe, lengthof( szMeCabExe ), ( LPCTSTR )GetProfileString( lpszEnvSection, lpszMeCabPath, L".\\mecab-0.996\\bin\\mecab.exe" ));
//	wcscpy_s( szSysDictionary, lengthof( szSysDictionary ), ( LPCTSTR )GetProfileString( lpszEnvSection, lpszSysDic, L".\\mecab-0.996\\work\\final" ));
	wcscpy_s( szSysDictionary, lengthof( szSysDictionary ), ( LPCTSTR )GetProfileString( lpszEnvSection, lpszSysDic, selfpath ));
//	wcscpy_s( szSysDicWork, lengthof( szSysDicWork ), ( LPCTSTR )GetProfileString( lpszEnvSection, lpszSysDicWork, L".\\mecab-0.996\\work" ));
	exchgFileName( selfpath, selfpath, L"" );			// 21/12/02
	if (( p = wcsrchr( selfpath, L'\\' )) != NULL )
	{	if ( *( p + 1 ) == L'\0' )
		{	*p = L'\0';
	}	}
	wcscpy_s( szSysDicWork, lengthof( szSysDicWork ), ( LPCTSTR )GetProfileString( lpszEnvSection, lpszSysDicWork, selfpath ));
	wcscpy_s( szUserDicCsv, lengthof( szUserDicCsv ), ( LPCTSTR )GetProfileString( lpszEnvSection, lpszUserCsv, L"" ));
	wcscpy_s( szUserDictionary, lengthof( szUserDictionary ), ( LPCTSTR )GetProfileString( lpszEnvSection, lpszUserDic, L"" ));
	wcscpy_s( szTextEditor, lengthof( szTextEditor ), ( LPCTSTR )GetProfileString( lpszEnvSection, lpszTextEditor, L"notepad++.exe" ));
	wcscpy_s( szEditOpt, lengthof( szEditOpt ), ( LPCTSTR )GetProfileString( lpszEnvSection, lpszEditOpt, L"-n%l %f" ));
	fgPausedspoff = ( BOOL )GetProfileInt( lpszEnvSection, lpszfgPausedspoff, TRUE );
	fgKigouPass = ( BOOL )GetProfileInt( lpszEnvSection, lpszfgKigouPass, TRUE );
	fgBgBlack = ( BOOL )GetProfileInt( lpszEnvSection, lpszfgBgBlack, FALSE );
	wcscpy_s( RecentFile, lengthof( RecentFile ), ( LPCTSTR )GetProfileString( lpszEnvSection, lpszRecentFileName, L"" ));
	lpszRecentFile = wcsdup( RecentFile );
	iNxtLineNo = ( long )GetProfileInt( lpszEnvSection, lpszRecentFileLine, 0 );
}


int CMecaDokuApp::GetProfCharHight()
{	return ( GetProfileInt( lpszEnvSection, lpszeditcharHeight, -10 ));
}


void CMecaDokuApp::SaveProfile()
{
	WriteProfileInt( lpszEnvSection, lpszeditcharHeight, editcharHeight );
	WriteProfileInt( lpszEnvSection, lpszSelectTts, SelectTts );
	WriteProfileInt( lpszEnvSection, lpszsapi54iRate, sapi54.iRate );
	WriteProfileInt( lpszEnvSection, lpszsapi54iVol, sapi54.iVol );
	WriteProfileInt( lpszEnvSection, lpszsapi54iModeSel, sapi54.iModeSel );
	WriteProfileInt( lpszEnvSection, lpszsapi54iVoiceSel, sapi54.iVoiceSel );
	WriteProfileInt( lpszEnvSection, lpszsapi54iMuonMs, sapi54.iMuonMs );
	WriteProfileString( lpszEnvSection, lpszMeCabPath, szMeCabExe );
	WriteProfileString( lpszEnvSection, lpszUserCsv, szUserDicCsv );
	WriteProfileString( lpszEnvSection, lpszUserDic, szUserDictionary );
	WriteProfileString( lpszEnvSection, lpszSysDic, szSysDictionary );
	WriteProfileString( lpszEnvSection, lpszTextEditor, szTextEditor );
	WriteProfileString( lpszEnvSection, lpszSysDicWork, szSysDicWork );
	WriteProfileString( lpszEnvSection, lpszEditOpt, szEditOpt );
	WriteProfileInt( lpszEnvSection, lpszfgPausedspoff, fgPausedspoff );
	WriteProfileInt( lpszEnvSection, lpszfgKigouPass, fgKigouPass );
	WriteProfileInt( lpszEnvSection, lpszfgBgBlack, fgBgBlack );
	WriteProfileString( lpszEnvSection, lpszRecentFileName, lpszRecentFile );
	WriteProfileInt( lpszEnvSection, lpszRecentFileLine, iNxtLineNo );
}


