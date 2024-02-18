
// MecaDokuDlg.cpp : 実装ファイル
//
// VoiceVox SAPIForVOICEVOX 対応 
//

#include "stdafx.h"
#include "MecaDoku.h"
#include "MecaDokuDlg.h"
#include "afxdialogex.h"
#include <string.h>
#include <windows.h>
//#include <synchapi.h>
#include <stdio.h>
#include <ctype.h>
#include <iostream>
#include <mbstring.h>
//#include <mmeapi.h>
#include <mmsystem.h>
#include "mecab.h"
#include "mecabdll.h"
#include <sapi.h>
#include <sphelper.h>
#include <io.h>
#include "JpnCode.h"

//#define JTALK	TRUE
//#define SAPI5P4	TRUE

//#ifdef JTALK
//#include "jtalk.h"
//#else
//#include "c:\\Tools\\aqtk10\\aqtk10regst.h"	// AquesTalk１０のライセンスコード設定だけのヘッダーです。 
#include "d:\\aqtk10\\aqtk10regst.h"	// AquesTalk１０のライセンスコード設定だけのヘッダーです。 
//#define AQTK10REGISTCODE	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"	// 公開できないのでファイルの場所をプロジェクト外にしています。
																// 必要な方はアクエストさんに発行して頂いてください。
																// ヘッダファイルには上記デファインだけでOKです。
																// ただ、EXEファイルをダンプした際に、コードが見えないよう少し
																// 加工して定義しています。
//#ifdef AQTK10REGISTCODE											// ライセンスコードが未定義ならセットしない 
#include "AquesTalk.h"
//#endif
//#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#pragma comment(lib, "libmecab.lib")
#pragma comment (lib, "winmm.lib")

extern BOOL				fgSkipspeak;
extern BOOL				fgKigouPass;
extern CCorpasDlg		*pCorpasDlg;
extern CSarchCsv		*pSarchCsv;
extern wchar_t			szMeCabExe[_MAX_PATH];
extern wchar_t			szUserDictionary[_MAX_PATH];
extern wchar_t			szSysDictionary[_MAX_PATH];
extern TTSNAME			SelectTts;
extern enum PLAYSTATE	iPlayState;
void __stdcall			StaticWaveOutProc( HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance,
                                           DWORD dwParam1, DWORD dwParam2 );
extern DWORD StartProcess( _TCHAR *processPath, _TCHAR *processOption, _TCHAR *execFolder, PROCESS_INFORMATION *pProcInfo );
extern void EndProcess( PROCESS_INFORMATION *pProcInfo );
extern wchar_t *m_szWFileName;	// grobal for free
extern struct SAPI54SETTING	sapi54;
extern wchar_t *exchgFileName( wchar_t *pszDest, wchar_t *pszPath, wchar_t *pszfileName );
extern wchar_t *pszRegistryEntry[];
extern wchar_t	szVoiceVoxPath[_MAX_PATH];
extern BOOL fgVoiceVox;
extern PROCESS_INFORMATION	VoiceVoxProcessInfomation;


//#define	MAX_LINE	4096
//						
LPmecab_model_new				fpmecab_model_new;
LPgetLastError					fpgetLastError;
LPmecab_model_new_tagger		fpmecab_model_new_tagger;
LPmecab_dictionary_info			fpmecab_dictionary_info;
LPmecab_model_new_lattice		fpmecab_model_new_lattice;
LPmecab_destroy					fpmecab_destroy;
LPmecab_lattice_set_sentence	fpmecab_lattice_set_sentence;
LPmecab_parse_lattice			fpmecab_parse_lattice;
LPmecab_lattice_get_bos_node	fpmecab_lattice_get_bos_node;
LPmecab_lattice_destroy			fpmecab_lattice_destroy;
LPmecab_model_destroy			fpmecab_model_destroy;
LPmecab_lattice_tostr			fpmecab_lattice_tostr;
LPmecab_lattice_set_boundary_constraint fpmecab_lattice_set_boundary_constraint;

HINSTANCE						hMecabDLL = NULL;			// Handle to DLL
long							mecabDllLastError = 0;

//
//	最初シフトJISで、シフトJISでは変換不能な文字が在るので、後にUTF-8に改造した。
//	UTF-8文字、及び文字列がVS2010ではu8が使えないのでVS2019を試してみたが、
//	文字列は可能だが、u8'！'等でエラーとなるため、VS2010で'\xxx'、"\xxx"表記とした。
//	16進表記のUTF-8ではソースを観た際分らないため、元の文字列をコメントとして残した。
//
//static char *ZENERRKIGOU	= "！ ＃ ＊  ， ． ／ ： ； ＜ ＝ ＞ ＾ ＿ ～ ー";
//static char *ZENERRKIGOU	= "！ ＃ ＊  ， ． ／ ： ； ＜ ＝ ＞ ＾ ＿ ～";	// SHIFT JIS  ↓ UTF-8
//static char *ZENERRKIGOU	= u8"！ ＃ ＊  ， ． ／ ： ； ＜ ＝ ＞ ＾ ＿ ～―";
static char *ZENERRKIGOU	= "\xef\xbc\x81 \xef\xbc\x83 \xef\xbc\x8a \xef\xbc\x8c \xef\xbc\x8e \xef\xbc\x8f \xef\xbc\x9a \xef\xbc\x9b \xef\xbc\x9c \xef\xbc\x9d \xef\xbc\x9e \xef\xbc\xbe \xef\xbc\xbf \xef\xbd\x9e \xE2\x80\x95 ";
static char *HALFERRKIGOU	= "!#*,./:;<=>^_~";
//static char *YOMIERRKIGOU[]	= {		"びっく\'り",	"しゃ\'ーぷ",		"あ\'すた",
//									"か\'んま",		"どっと",			"すら\'っしゅ",
//									"こ\'ろん",		"せみこ\'ろん",		"しょ\'ーなり",
//									"いこ\'ーる",	"だ\'いなり",		"は\'っと",
//									"あ\'んだー",	"から" };
static char *YOMIERRKIGOU[]	=
{	"\xE3\x81\xB3\xE3\x81\xA3\xE3\x81\x8F\'\xE3\x82\x8A",	// "びっく\'り"
	"\xE3\x81\x97\xE3\x82\x83\'\xE3\x83\xBC\xE3\x81\xB7",	// "しゃ\'ーぷ",
	"\xE3\x81\x82\'\xE3\x81\x99\xE3\x81\x9F",				// "あ\'すた",
	"\xE3\x81\x8B\'\xE3\x82\x93\xE3\x81\xBE",				// "か\'んま",
	"\xE3\x81\xA9\xE3\x81\xA3\xE3\x81\xA8",					// "どっと",
	"\xE3\x81\x99\xE3\x82\x89\'\xE3\x81\xA3\xE3\x81\x97\xE3\x82\x85",	// "すら\'っしゅ",
	"\xE3\x81\x93\'\xE3\x82\x8D\xE3\x82\x93",				// "こ\'ろん",
	"\xE3\x81\x9B\xE3\x81\xBF\xE3\x81\x93\'\xE3\x82\x8D\xE3\x82\x93",	// "せみこ\'ろん",
	"\xE3\x81\x97\xE3\x82\x87\'\xE3\x83\xBC\xE3\x81\xAA\xE3\x82\x8A",	// "しょ\'ーなり",
	"\xE3\x81\x84\xE3\x81\x93\'\xE3\x83\xBC\xE3\x82\x8B",	// "いこ\'ーる",
	"\xE3\x81\xA0\'\xE3\x81\x84\xE3\x81\xAA\xE3\x82\x8A",	// "だ\'いなり",
	"\xE3\x81\xAF\'\xE3\x81\xA3\xE3\x81\xA8",				// "は\'っと",
	"\xE3\x81\x82\'\xE3\x82\x93\xE3\x81\xA0\xE3\x83\xBC",	// "あ\'んだー",
	"\xE3\x81\x8B\xE3\x82\x89",								// "から"
	"\xE3\x80\x82",											// "。"
};
//static char *ZENKIGOU		= "！＃＄％＆＊＋，－．／：；＜＝＞？＠￥＾＿、。ー";
//static char *ZENKIGOU		= u8"！＃＄％＆＊＋，－．／：；＜＝＞？＠￥＾＿、。ー";
static char *ZENKIGOU		= "\xef\xbc\x81 \xef\xbc\x83 \xef\xbc\x84 \xEF\xBC\x85 \xEF\xBC\x86 \xEF\xBC\x8A \xEF\xBC\x8B \xEF\xBC\x8C \xEF\xBC\x8D \xEF\xBC\x8E \xEF\xBC\x8F \xEF\xBC\x9A \xEF\xBC\x9B \xEF\xBC\x9C \xEF\xBC\x9D \xEF\xBC\x9E \xEF\xBC\x9F \xEF\xBC\xA0 \xEF\xBF\xA5 \xEF\xBC\xBE \xEF\xBC\xBF \xE3\x80\x81 \xE3\x80\x82 \xE3\x83\xBC";
static char *HALFKIGOU		= " !#$%&*+,-./:;<=>?@\\^_";
//static char *COAIUEO		= "ぁぃぅぇぉ";	// SHIFT JIS  ↓ UTF-8
//static char *COAIUEO		= u8"ぁぃぅぇぉ";
static char *COAIUEO		= "\xe3\x81\x81\xe3\x81\x83\xe3\x81\x85\xe3\x81\x87\xe3\x81\x89";

//
//	センテンスの最初の文字としてはAquesTalkがエラーにしてしまう文字表 
//	小文字等、Mecabが適切に区切れていない等で発生する。
//
static const char tblErrorStart[] =
{			 0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  0,  0,  0,  0,  0,
//e38180		ぁ	あ	ぃ	い	ぅ	う	ぇ	え	ぉ	お	か	が	き	ぎ	く
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//e38190	ぐ	け	げ	こ	ご	さ	ざ	し	じ	す	ず	せ	ぜ	そ	ぞ	た
//			 0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,		// 21/10/25 "っ"は解除 
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//e381a0	だ	ち	ぢ	っ	つ	づ	て	で	と	ど	な	に	ぬ	ね	の	は
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//e381b0	ば	ぱ	ひ	び	ぴ	ふ	ぶ	ぷ	へ	べ	ぺ	ほ	ぼ	ぽ	ま	み
			 0,  0,  0,  1,  0,  1,  0,  1,  0,  0,  0,  0,  0,  0,  1,  0,
//e38280	む	め	も	ゃ	や	ゅ	ゆ	ょ	よ	ら	り	る	れ	ろ	ゎ	わ
			 0,  0,  0,  0,  0,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//e38290	ゐ	ゑ	を	ん	ゔ	ゕ	ゖ			゙	゚	゛	゜	ゝ	ゞ	ゟ
			 0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  0,  0,  0,  0,  0,
//e382a0	゠	ァ	ア	ィ	イ	ゥ	ウ	ェ	エ	ォ	オ	カ	ガ	キ	ギ	ク
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//e382b0	グ	ケ	ゲ	コ	ゴ	サ	ザ	シ	ジ	ス	ズ	セ	ゼ	ソ	ゾ	タ
//			 0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,		// 21/10/25 "っ"は解除 
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//e38380	ダ	チ	ヂ	ッ	ツ	ヅ	テ	デ	ト	ド	ナ	ニ	ヌ	ネ	ノ	ハ
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//e38390	バ	パ	ヒ	ビ	ピ	フ	ブ	プ	ヘ	ベ	ペ	ホ	ボ	ポ	マ	ミ
			 0,  0,  0,  1,  0,  1,  0,  1,  0,  0,  0,  0,  0,  0,  1,  0,
//e383a0	ム	メ	モ	ャ	ヤ	ュ	ユ	ョ	ヨ	ラ	リ	ル	レ	ロ	ヮ	ワ
			 0,  0,  0,  0,  0,  1,  1,  0,  0,  0,  0,  0,  1,  0,  0,  0
//e383b0	ヰ	ヱ	ヲ	ン	ヴ	ヵ	ヶ	ヷ	ヸ	ヹ	ヺ	・	ー	ヽ	ヾ	ヿ
};

//
#define	YAYUEYO		1
#define	YU			2
#define	AIO			4
#define	U			8
#define	I			16
#define	E			32

struct COMOJI
{	char	szComoji[4];	// 
	int		procs;			// YAYUEYO, YU, AIO, U, I
};
struct COMOJI lstComoji[] = {
	{	"\xE3\x82\x83"/*"ゃ"*/, YAYUEYO },
	{	"\xE3\x82\x85"/*"ゅ"*/, YAYUEYO | YU },
	{	"\xE3\x81\x87"/*"ぇ"*/, E },
	{	"\xE3\x82\x87"/*"ょ"*/, YAYUEYO },
	{	"\xE3\x81\x81"/*"ぁ"*/, AIO },
	{	"\xE3\x81\x83"/*"ぃ"*/, AIO | I },
//	{	"\xE3\x81\x87"/*"ぇ"*/, AIO },
	{	"\xE3\x81\x89"/*"ぉ"*/, AIO },
	{	"\xE3\x81\x85"/*"ぅ"*/, U }
};

//static char *COYAYUYOOK	= "き し ち に ひ み り ぎ じ び ぴ ";
static char *COYAYUYOOK		= "\xE3\x81\x8D \xE3\x81\x97 \xE3\x81\xA1 \xE3\x81\xAB \xE3\x81\xB2 \xE3\x81\xBF \xE3\x82\x8A \xE3\x81\x8E \xE3\x81\x98 \xE3\x81\xB3 \xE3\x81\xB4 ";
//static char *COYUOK		= "で て ";
static char *COYUOK			= "\xE3\x81\xA7 \xE3\x81\xA6 ";
//static char *COAIEOOK		= "うつふ";
static char *COAIEOOK		= "\xE3\x81\x86\xE3\x81\xA4\xE3\x81\xB5 ";
//static char *COUOK		= "と ど ";
static char *COUOK			= "\xE3\x81\xA8 \xE3\x81\xA9 ";
//static char *COIOK		= "す て ず で ";
static char *COIOK			= "\xE3\x81\x99 \xE3\x81\xA6 \xE3\x81\x9A \xE3\x81\xA7 ";
//static char *COYAYUYO		= "ゃゅぇょ";
//static char *COYU			= "ゅ";
//static char *COAIEO		= "ぁぃぇぉ";
//static char *COU			= "ぅ";
//static char *COI			= "ぃ";
//
//static char *COUNG			= "ぶぁ ぶぃ ぶぅ ぶぇ ぶぉ";
//static char *COUNGx			= "ば   び   ぶ   べ   ぼ";
//
//static char *THUHU			= "つふ";	// SHIFT JIS  ↓ UTF-8
//static char *THUHU			= "\xe3\x81\xa4\xe3\x81\xb5";
//static char *THUHU			= u8"つふ";
static char *THUHU			= "\xE3\x81\xA4 \xE3\x81\xB5";
//static char *CYOUON			= "ー";	// SHIFT JIS  ↓ UTF-8
//static char *CYOUON			= "\xe3\x83\xbc";
//static char *CYOUON			= u8"ー";	// SHIFT JIS  ↓ UTF-8
static char *CYOUON			= "\xE3\x83\xBC";	// SHIFT JIS  ↓ UTF-8

char bufUTF8[MAX_LINE];
char bufShiftJis[MAX_LINE];
char bufHiraganaJis[MAX_LINE] = { 0 };

											// AquesTalkの
#define SRATE			16000				// 標本化周波数(1秒間のサンプル数)
#define F				400					// 周波数(1秒間の波形数)
#define BITWIDTH		16					// 量子化ビット数

enum WAVEOUTOPENTATE	StateWaveOutOpen = WFNONOPEN;	// WaveOutOpen wfe設定の種別 
											// WFNONOPEN:	no open
											// WFAQTK10:	Aqtk10
											// WFSAPI:		Sapi
											// WaveOutOpenの設定（Bitrate等）がAqtk10用かSapi用か 
WAVEFORMATEX			wfe;
HWAVEOUT				hWaveOut = 0;
#if 0										// マルチバッファしてみたがつなぎ目でノイズが発声した。 
											// センテンス毎に発音する場合、音が連続しているわけでは
											// 無いので、逆にセンテンス間に無音期間が無いとノイズに
											// なると思われる。
static WAVEHDR			whdr[2];			// wave header
static LPBYTE			lpWave[2];			// wave buf
static int				bcWave[2];			// alloc size
#else
WAVEHDR					whdr[1];			// wave header
LPBYTE					lpWave[1];			// wave buf
long long int			bcWave[1];			// alloc size
#endif
static int				ctRcvDone = 0;
int						ctCnfDone = 0;
int						ixPlayRec = 0;		// waveOutWrite touroku index
//static int			ixPlayEnd = 0;		// Done recive index
volatile int			ctPlayRec = 0;		// waveOutWrite touroku count
CRITICAL_SECTION		cstPlayRec;
//	InitializeCriticalSection( &cstPlayRec );
//	EnterCriticalSection( &cstPlayRec );
//	LeaveCriticalSection( &cstPlayRec );
static char				szLstChar[4] = { 0 };
mecab_model_t			*model, *another_model;
mecab_t					*mecab;
mecab_lattice_t			*lattice;
BOOL					OkMecab = FALSE;
AQTK_VOICE				voice = gVoice_F1;
ISpVoice				*pVoice = NULL;
CComPtr <ISpStream>		cpWavStream;

int waveFainalize();

int aqtk10waveInitialize2()
{	MMRESULT retstate;
	int i;

	if ( StateWaveOutOpen != WFAQTK10 )	// WaveOutOpen
	{	wfe.wFormatTag		= WAVE_FORMAT_PCM;
		wfe.nChannels		= 1;									//モノラル
		wfe.wBitsPerSample	= BITWIDTH;								//量子化ビット数
		wfe.nBlockAlign		= wfe.nChannels * wfe.wBitsPerSample/8;
		wfe.nSamplesPerSec	= SRATE;								//標本化周波数
		wfe.nAvgBytesPerSec	= wfe.nSamplesPerSec * wfe.nBlockAlign;
		for ( i = 0; i < lengthof( bcWave ); i++ )
		{	bcWave[i] = 0;
			lpWave[i] = NULL;
		}
		if (( retstate = waveOutOpen( &hWaveOut, WAVE_MAPPER, &wfe, ( DWORD_PTR )StaticWaveOutProc, ( DWORD_PTR )NULL, CALLBACK_FUNCTION )) != MMSYSERR_NOERROR )
		{	return ( retstate );
#if 0
			switch ( retstate )
			{ case MMSYSERR_ALLOCATED:
				std::cout << "Specified resource is already allocated." << std::endl;
				break;
				case MMSYSERR_BADDEVICEID:
				std::cout << "Specified device identifier is out of range." << std::endl;
				break;
				case MMSYSERR_NODRIVER:
				std::cout << "No device driver is present." << std::endl;
				break;
				case MMSYSERR_NOMEM:
				std::cout << "Unable to allocate or lock memory." << std::endl;
				break;
				case WAVERR_BADFORMAT:
				std::cout << "Attempted to open with an unsupported waveform-audio format." << std::endl;
				break;
				case WAVERR_SYNC:
				std::cout << "The device is synchronous but waveOutOpen was called without using the WAVE_ALLOWSYNC flag." << std::endl;
				break;
				default:
				std::cout << "other error." << std::endl;
				break;
			}
			return ( -1 );
#endif
		}
		StateWaveOutOpen = WFAQTK10;
	}
	return ( 0 );
}


int aqtk10waveInitialize()
{//	MMRESULT retstate;
//	int i;

	if ( SelectTts != AQTK10 )
	{	waveFainalize();
	}
	aqtk10waveInitialize2();
	return ( 0 );
}


int waveFainalize()
{	if ( hWaveOut != 0 )
	{	waveOutClose( hWaveOut );
		hWaveOut = 0;
	}
	StateWaveOutOpen = WFNONOPEN;
	return ( 0 );
}


void UniToUTF8( char *pszUni, char *pszUtf8, int lenUtf8 )
{	int iLenUnicode;

	iLenUnicode = ( int )wcslen(( const wchar_t* )pszUni );	// * sizeof( TCHAR );
//		次に、UniocdeからUTF8に変換する	// サイズを計算する
	int iLenUtf8 = WideCharToMultiByte( CP_UTF8, 0, ( wchar_t * )pszUni, iLenUnicode, NULL, 0, NULL, NULL );
	if ( iLenUtf8 <= lenUtf8 )
	{	WideCharToMultiByte( CP_UTF8, 0, ( wchar_t * )pszUni, iLenUnicode, pszUtf8, lenUtf8, NULL, NULL );
		*( pszUtf8 + iLenUtf8 ) = EOS;
	}
}


int CodePageToUni( UINT codepage, char *pszCPstr, char *pszUni, int lenUni )
{    
//	まずUniocdeに変換する		// サイズを計算する
	int iLenUnicode = MultiByteToWideChar( codepage, 0, pszCPstr, ( int )strlen( pszCPstr ) + 1, NULL, NULL );
	if ( iLenUnicode <= 0 )
	{	return ( 0 );
	}
	if ( iLenUnicode <= lenUni )
	{	MultiByteToWideChar( codepage, 0, pszCPstr, ( int )strlen( pszCPstr ) + 1, ( LPWSTR )pszUni, lenUni );
		return ( iLenUnicode );
	}
	return ( -1 );
}


void UTF8ToUni( char *pszUtf8, char *pszUni, int lenUni )
{    
//	まずUniocdeに変換する		// サイズを計算する
	int iLenUnicode = MultiByteToWideChar( CP_UTF8, 0, pszUtf8, ( int )strlen( pszUtf8 ) + 1, NULL, NULL );
	if ( iLenUnicode <= lenUni )
	{	MultiByteToWideChar( CP_UTF8, 0, pszUtf8, ( int )strlen( pszUtf8 ) + 1, ( LPWSTR )pszUni, lenUni );
	}
}


#if 0
void ShiftJisToUni( char *pszSjis, char *pszUni, int lenUni )
{//	wchar_t bufUnicode[MAX_LINE];
    
//	まずUniocdeに変換する	// サイズを計算する
	size_t iLenUnicode = MultiByteToWideChar( CP_ACP, 0, pszSjis, ( int )strlen( pszSjis ) + 1, NULL, NULL );
    if ( iLenUnicode <= lenUni )
	{	MultiByteToWideChar( CP_ACP, 0, pszSjis, ( int )strlen( pszSjis ) + 1, ( LPWSTR )pszUni, lenUni );
	}
}


void ShiftJisToUTF8( char *pszSjis, char *pszUtf8, int lenUtf8 )
{	wchar_t bufUnicode[MAX_LINE];
    
//	まずUniocdeに変換する	// サイズを計算する
//	int iLenUnicode = MultiByteToWideChar( CP_ACP, 0, szShiftJis, strlen( szShiftJis ) + 1, NULL, NULL );
	int iLenUnicode = MultiByteToWideChar( CP_ACP, 0, pszSjis, ( int )strlen( pszSjis ) + 1, NULL, NULL );
    if ( iLenUnicode <= ( sizeof( bufUnicode ) / sizeof( bufUnicode[0])))
	{	MultiByteToWideChar( CP_ACP, 0, pszSjis, ( int )strlen( pszSjis ) + 1, bufUnicode, sizeof( bufUnicode ));
//		次に、UniocdeからUTF8に変換する	// サイズを計算する
		int iLenUtf8 = WideCharToMultiByte( CP_UTF8, 0, bufUnicode, iLenUnicode, NULL, 0, NULL, NULL );
		if ( iLenUtf8 <= lenUtf8 )
		{	WideCharToMultiByte( CP_UTF8, 0, bufUnicode, iLenUnicode, pszUtf8, lenUtf8, NULL, NULL );
		}
	}
}


/*
http://x0213.org/wiki/wiki.cgi?page=%C7%C8%A5%C0%A5%C3%A5%B7%A5%E5%CC%E4%C2%EA
日本語通用名称	区点	SJIS	文字名				正しいUnicode符号位置	誤り
波ダッシュ		1-33	8160	WAVE DASH				U+301C				U+FF5E
双柱			1-34	8161	DOUBLE VERTICAL LINE	U+2016				U+2225
負符号			1-61	817C	MINUS SIGN				U+2212				U+FF0D
セント記号		1-81	8191	CENT SIGN				U+00A2				U+FFE0
ポンド記号		1-82	8192	POUND SIGN				U+00A3				U+FFE1
否定記号		2-44	81CA	NOT SIGN				U+00AC				U+FFE2
ダッシュ(全角)	1-29	815C	EM DASH					U+2014				U+2015
*/


void UTF8ToShiftJis( char *pszUtf8, char *pszSjis, int lenSjis )
{	static WORD tblOfSjisNg[][2] =
	{	{	0x2212, 0xFF0D },
		{	0x301C, 0xFF5E },
		{	0x2016, 0x2225 },
		{	0x00A2, 0xFFE0 },
		{	0x00A3, 0xFFE1 },
		{	0x00AC, 0xFFE2 },
		{	0x2014, 0x2015 }
	};
	wchar_t bufUnicode[MAX_LINE], *p;
	int iSj, n;
    
//	まずUniocdeに変換する		// サイズを計算する
	int iLenUnicode = MultiByteToWideChar( CP_UTF8, 0, pszUtf8, ( int )strlen( pszUtf8 ) + 1, NULL, NULL );
	if ( iLenUnicode <= sizeof( bufUnicode ) / sizeof( bufUnicode[0]))
	{	MultiByteToWideChar( CP_UTF8, 0, pszUtf8, ( int )strlen( pszUtf8 ) + 1, bufUnicode, sizeof( bufUnicode ));
		for ( n = 0; n < lengthof( tblOfSjisNg ); n++ )	// シフトJIS誤変換文字の置き換え 
		{	while (( p = wcschr( bufUnicode, tblOfSjisNg[n][0])) != NULL )
			{	*p = tblOfSjisNg[n][1];
		}	}
//		次に、UniocdeからShiftJisに変換する		// サイズを計算する
		int iLenUtf8 = WideCharToMultiByte( CP_ACP, 0, bufUnicode, iLenUnicode, NULL, 0, NULL, NULL );
		if ( iLenUtf8 <= lenSjis )
		{	iSj = WideCharToMultiByte( CP_ACP, 0, bufUnicode, iLenUnicode, pszSjis, lenSjis, NULL, NULL );
			pszSjis[iSj] = EOS;
//			if (( *pszSjis == '?' ) && ( *( pszSjis + 1 ) == EOS ))
			if ( *pszSjis == '?' )
			{	if ( bufUnicode[0] == 0x301c )		//
//				if ( bufUnicode[0] == L'～' )
				{	strcpy_s( pszSjis + 1, lenSjis, pszSjis );
					strncpy_s( pszSjis, lenSjis, "～", 2 );
				}
			}
		}
	}
}
#endif


//WAVE出力デバイスコールバック関数（スタティック関数）
void __stdcall StaticWaveOutProc( HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance,
                                           DWORD dwParam1, DWORD dwParam2 )
{
	if ( uMsg == MM_WOM_DONE )
	{	// _sleep( 3000 );
		ctRcvDone++;
		EnterCriticalSection( &cstPlayRec );
		if ( ctPlayRec > 0 )
		{	ctPlayRec--;
		}
		LeaveCriticalSection( &cstPlayRec );
	}
}


unsigned char *conv2Hiragana( unsigned char *pHiragana, unsigned char *pSorceStr )
{	static char *pszJiHun[] =
	{	"\xE3\x81\x98",					// E3 81 98 じ 
		"\xE3\x81\xB5\xE3\x82\x93"		// E3 81 B5 E3 82 93 ふん 
	};
	unsigned char *p, *q, *pp, *pColon, zenchr[4];
	unsigned long ch;
	int ctColon;

	pColon = ( unsigned char * )strchr(( char * )pSorceStr, ':' );
	for ( ctColon = 0, p = pSorceStr, q = pHiragana; *p != EOS; )
	{	if ( isU8Hankaku( *p ))
		{	if ( isdigit( *p ) || ( *p == '-' ) || ( *p == '.' ))
			{	*q++ = '<';					//	<NUMK VAL=999>
				*q++ = 'N';
				*q++ = 'U';
				*q++ = 'M';
				if ( pColon == NULL )		// コロンが内在していない時のみ桁読み？ 
				{	*q++ = 'K';
				}
				else if (( p - pColon ) < 0 )	// コロンまでの時桁
				{	*q++ = 'K';
				}
				*q++ = ' ';
				*q++ = 'V';
				*q++ = 'A';
				*q++ = 'L';
				*q++ = '=';
				while ( isdigit( *p ) || ( *p == '-' ) || ( *p == '.' ))
				{	*q++ = *p++;
				}
				*q++ = '>';
				continue;
			}
			else if ( isalpha( *p ))
			{	*q++ = '<';				//	<NUMK VAL=999>
				*q++ = 'A';
				*q++ = 'L';
				*q++ = 'P';
				*q++ = 'H';
				*q++ = 'A';
				*q++ = ' ';
				*q++ = 'V';
				*q++ = 'A';
				*q++ = 'L';
				*q++ = '=';
				*q++ = '"';
				while (( isalnum( *p ) || ( strchr( HALFKIGOU, *p ) != NULL )) && ( *p != '\0' ))
				{	*q++ = *p++;
				}
				*q++ = '"';
				*q++ = '>';
				continue;
			}
			else if ( *p == ':' )
			{	if ( q != pHiragana )
				{	if ( isdigit( *( p + 1 )) && isdigit( *( q - 2 )))		// 数字に挟まれた':' 
					{	if ( ctColon < lengthof( pszJiHun ))
						{	for ( pp = ( unsigned char * )pszJiHun[ctColon]; *pp != EOS; )
							{	*q++ = *pp++;
							}
							ctColon++;
							p++;
						}
						else
						{	if ( !fgKigouPass )
							{	for ( pp = ( unsigned char * )YOMIERRKIGOU[6]; *pp != EOS; )
								{	*q++ = *pp++;
							}	}
							p++;
					}	}
					else
					{	if ( !fgKigouPass )
						{	for ( pp = ( unsigned char * )YOMIERRKIGOU[6]; *pp != EOS; )
							{	*q++ = *pp++;
						}	}
						p++;
				}	}
				else
				{	if ( !fgKigouPass )
					{	for ( pp = ( unsigned char * )YOMIERRKIGOU[6]; *pp != EOS; )
						{	*q++ = *pp++;
					}	}
					p++;
			}	}
			else
			{	*q++ = *p++;
		}	}
		else
		{
//			else if ( _ismbblead( *p ))
			if ( *p == 0xe3 )									// 全角ひらがな/カタカナ
			{	ch = *p << 16 | *( p + 1 ) << 8 | *( p + 2 );
				if (( 0xE382BF/*u8'タ'*/ >= ch ) && ( ch >= 0xE382A1/*u8'ァ'*/ ))		//    'ぁ' ~ 'タ'
				{	ch -= 0xE382A1/*u8'ァ'*/ - 0xE38181/*u8'ぁ'*/;
				}
				else if (( 0xE3839F/*u8'ミ'*/ >= ch ) && ( ch >= 0xE38380/*u8'ダ'*/ ))	//    'ダ' ~ 'み'
				{	if ( 0xE38382/*u8'ヂ'*/ == ch ) ch = 0xE38198/*u8'じ'*/;			//    'ヂ' <= 'じ';
					else if ( 0xE38385/*u8'ヅ'*/ == ch ) ch = 0xE3819A/*u8'ず'*/;		//    'ヅ' <= 'ず'
					else ch -= 0xE38380/*u8'ダ'*/ - 0xE381A0/*u8'だ'*/;
				}
				else if (( 0xE383B6/*u8'ヶ'*/ >= ch ) && ( ch >= 0xE383A0/*u8'ム'*/ ))	//    'ダ' ~ 'む'
				{	if ( ch == 0xE383B4/*u8'ヴ'*/ )										// ヴ	8375
					{	ch = 0xE381B6/*u8'ぶ'*/;
					}
					else
					{	ch -= 0xE383A0/*u8'ム'*/ - 0xE38280/*u8'む'*/;
				}	}
				else if (( 0xE381BF/*u8'み'*/ >= ch ) && ( ch >= 0xE38181/*u8'ぁ'*/ ))	//    'ぁ' ~ 'タ'
				{	if ( 0xE381A2/*u8'ぢ'*/ == ch ) ch = 0xE38198/*u8'じ'*/;			//    'ヂ' <= 'じ';
					else if ( 0xE381A5/*u8'づ'*/ == ch ) ch = 0xE3819A/*u8'ず'*/;		//    'ヅ' <= 'ず'
				}
				else if (( 0xE38296/*u8'ゖ'*/ >= ch ) && ( ch >= 0xE38280/*u8'む'*/ ))	//    'ぁ' ~ 'タ'
				{	if ( ch == 0xE38294/*u8'ゔ'*/ )										// ヴ	8375
					{	ch = 0xE381B6/*u8'ぶ'*/;
				}	}
//				else if ( 0xE38198/*u8'じ'*/ == ch ) ch = 0xE381A2/*u8'ぢ'*/;			//    'ヂ' <= 'じ'
//				else if ( 0xE3819A/*u8'ず'*/ == ch ) ch = 0xE381A5/*u8'づ'*/;			//    'ヅ' <= 'ず'
				else //if (( 0x8141 <= ch ) && ( ch <= 0x8197 ))	// '、' ~ '＠'
				{	memcpy( &zenchr[0], p, 3 );
					zenchr[3] = '\0';
//					if ( strcmp(( char * )&zenchr[0], "ー" ) != 0 )
					{	if ( strstr( ZENKIGOU, ( char * )&zenchr[0]) == NULL )
						{	p += 3;
							continue;
				}	}	}
				p += 3;
			}
			else if ( *p == 0xef )								// 全角数字英字区 
			{	ch = *p << 16 | *( p + 1 ) << 8 | *( p + 2 );
				if ( isU8digit( ch ) || ( ch == 0xEFBC8D/*u8'－'*/ ))		// 
				{	if ( ch == 0xEFBC8D/*u8'－'*/ )							// 
					{	unsigned long ch2 = *( p + 3 ) << 16 | *( p + 4 ) << 8 | *( p + 5 );
//						if ( !isSjisdigit( ch ))
						if ( !isU8digit( ch ))
						{	*q++ = ( unsigned char )( ch >> 16 );
							*q++ = ( unsigned char )(( ch >> 8 ) & 0xff );
							*q++ = ( unsigned char )( ch & 0xff );
							p += 3;
							continue;
					}	}
					*q++ = '<';				//	<NUMK VAL=999>
					*q++ = 'N';
					*q++ = 'U';
					*q++ = 'M';
					*q++ = 'K';
					*q++ = ' ';
					*q++ = 'V';
					*q++ = 'A';
					*q++ = 'L';
					*q++ = '=';
					while ( 1 )
					{	if ( isU8digit( ch ))
						{	*q++ = ch - ( 0xEFBC90/*u8'０'*/ - '0' );
							p += 3;
							ch = *p << 16 | *( p + 1 ) << 8 | *( p + 2 );
							continue;
						}
						else if ( ch == 0xEFBC8D/*u8'－'*/ )
						{	*q++ = '-';
							p += 3;
							ch = *p << 16 | *( p + 1 ) << 8 | *( p + 2 );
							continue;
						}
						else if ( ch == 0xEFBC8E/*u8'．'*/ )
						{	*q++ = '.';
							p += 3;
							ch = *p << 16 | *( p + 1 ) << 8 | *( p + 2 );
							continue;
						}
						break;
					}
					*q++ = '>';
					continue;
				}
				else if ( isU8Alpha( ch ))						// 
				{	*q++ = '<';				//	<NUMK VAL=999>
					*q++ = 'A';
					*q++ = 'L';
					*q++ = 'P';
					*q++ = 'H';
					*q++ = 'A';
					*q++ = ' ';
					*q++ = 'V';
					*q++ = 'A';
					*q++ = 'L';
					*q++ = '=';
					*q++ = '"';
					while ( 1 )
					{	if (( 0xEFBCA1/*u8'Ａ'*/ <= ch ) && ( ch <= 0xEFBCBA/*u8'Ｚ'*/ ))
						{	*q++ = ( unsigned char )( ch - ( 0xEFBCA1/*u8'Ａ'*/ - 'A' ));
							p += 3;
							ch = *p << 16 | *( p + 1 ) << 8 | *( p + 2 );
							continue;
						}
						else if (( 0xEFBD81/*u8'ａ'*/ <= ch ) && ( ch <= 0xEFBD9A/*u8'ｚ'*/ ))
						{	*q++ = ( unsigned char )( ch - ( 0xEFBD81/*u8'ａ'*/ - 'a' ));
							p += 3;
							ch = *p << 16 | *( p + 1 ) << 8 | *( p + 2 );
							continue;
						}
						break;
					}
					*q++ = '"';
					*q++ = '>';
					continue;
				}											// ひらがなが来ることがある様 
				else
				{	//ch = 0xE38081;/*u8'、'*/				// その他文字　エラーになりそーなので空白に 
//					ch = ( ',' << 16 ) | ( ','  << 8 ) | ',';
//					*q++ = ',';
					p += 3;
					continue;
				}
				p += 3;
			}
			else
			{	//ch = 0xE38081;/*u8'、'*/					// その他文字　エラーになりそーなので空白に 
//				ch = ( ',' << 16 ) | ( ','  << 8 ) | ',';
//				*q++ = ',';
				p += 3;
				continue;
			}
			*q++ = ( unsigned char )( ch >> 16 );
			*q++ = ( unsigned char )(( ch >> 8 ) & 0xff );
			*q++ = ( unsigned char )( ch & 0xff );
		}
	}
	*q++ = EOS;
	return ( pHiragana );
}


BOOL extractHatuonfeature( char *dst, char *src )
{	char szLine[512], *buf, *pCanma[10], *phatuon, *pFound;
	int IxCanma, val, swp;

	memset( &pCanma[0], 0, sizeof( pCanma ));
	for ( buf = &szLine[0], IxCanma = 0; *src != EOS; src++ )
	{	if (( *src == ',' ) && ( IxCanma < lengthof( pCanma )))
		{	pCanma[IxCanma++] = buf;
		}
		*buf++ = *src;
	}
	if ( pCanma[7] == NULL )
	{	return ( FALSE );
	}
	*buf = EOS;;
	swp = *( pCanma[7] + 4 );		/*4:Utf-8 3:ShiftJis*/
	*( pCanma[7] + 4 ) = '\0';
	if (( pFound = strstr( ZENERRKIGOU, pCanma[7] + 1 )) != NULL )	// エラーになる全角記号か？ 
	{	if ( !fgKigouPass )
		{	val = ( int )(( pFound - ZENERRKIGOU ) / 4/*4:Utf-8 3:ShiftJis*/ );
			for ( pFound = YOMIERRKIGOU[val]; *pFound != '\0'; )
			{	*dst++ = *pFound++;		/*4:Utf-8 3:ShiftJis*/
			}
//			*( pCanma[7] + 4 ) = swp;
	}	}
	else
	{	*( pCanma[7] + 4 ) = swp;	/*4:Utf-8 3:ShiftJis*/
		for ( phatuon = pCanma[7] + 1; ( *phatuon != '\n' ) && ( *phatuon != '\0' ); )
		{	*dst++ = *phatuon++;
	}	}
	*dst = EOS;
	szLstChar[0] = *( dst - 3 );
	szLstChar[1] = *( dst - 2 );
	szLstChar[2] = *( dst - 1 );
	return ( TRUE );
}


BOOL extractHatuonsurface( char *dst, char *src )
{	char szLine[4], *phatuon, *pFound;
	int val, ix;

	if (( val = strcmp( src, "EOS")) == 0 )					// 全角記号もくる
	{	return ( TRUE );
	}
	szLine[3] = '\0';
	for ( ix = 0; src[ix] != '\0'; )
	{	szLine[0] = src[ix];
		if ( isU8Hankaku(( unsigned char )szLine[0]))
		{	if (( pFound = strchr( HALFERRKIGOU, szLine[0])) != NULL )
			{	if ( !fgKigouPass )
				{	val = ( int )( pFound - HALFERRKIGOU );
					if (( 0 <= val ) && ( val < lengthof( YOMIERRKIGOU )))
					{	for ( pFound = YOMIERRKIGOU[val]; *pFound != '\0'; )
						{	*dst++ = *pFound++;
			}	}	}	}
			else if (( pFound = strchr( HALFKIGOU, szLine[0])) != NULL )
			{	val = ( int )(( pFound - HALFKIGOU ) * 2 );
				*dst++ = *( ZENKIGOU + val );
				*dst++ = *( ZENKIGOU + val + 1 );
				*dst++ = *( ZENKIGOU + val + 2 );
			}
			else
			{	*dst++ = szLine[0];
			}
			ix++;
			continue;
		}
		else if ( isU83Byte(( unsigned char )szLine[0]))
		{	szLine[1] = src[ix + 1];
			szLine[2] = src[ix + 2];
			if (( pFound = strstr( COAIUEO, &szLine[0])) != NULL )			// エラーになるぁぃぅぇぉか？ 
			{	if (( pFound = strstr( THUHU, &( szLstChar[0]))) == NULL )	// エラーにならないつふか？ 
				{	*dst++ = *CYOUON;
					*dst++ = *( CYOUON + 1 );
					*dst++ = *( CYOUON + 2 );
					break;
//					*dst = EOS;
//					return ( 1 );
//					for ( ; (( *src == '\n' ) || ( *src == '\r' )); src++ ) ;
//					continue;
			}	}
			if (( pFound = strstr( ZENERRKIGOU, &szLine[0])) != NULL )		// エラーになる全角記号か？ 
			{	if ( !fgKigouPass )
				{	val = ( int )(( pFound - ZENERRKIGOU ) / 4 );
					if (( 0 <= val ) && ( val < lengthof( YOMIERRKIGOU )))
					{	for ( pFound = YOMIERRKIGOU[val]; *pFound != '\0'; )
						{	*dst++ = *pFound++;
			}	}	}	}
			else if (( pFound = strstr( ZENKIGOU, &szLine[0])) != NULL )
			{	*dst++ = szLine[0];
				*dst++ = szLine[1];
				*dst++ = szLine[2];
			}
			else for ( phatuon = &szLine[0]; (( *phatuon != '\t' ) && ( *phatuon != EOS )); )
			{	*dst++ = *phatuon++;
			}
			ix += 3;
			continue;
		}
		else if ( isU82Byte(( unsigned char )szLine[0]))
		{	ix += 2;
			continue;
		}
		else if ( isU84Byte(( unsigned char )szLine[0]))
		{	ix += 4;
			continue;
		}
		else if ( isU85Byte(( unsigned char )szLine[0]))
		{	ix += 5;
			continue;
		}
		else if ( isU86Byte(( unsigned char )szLine[0]))
		{	ix += 6;
			continue;
		}
		else
		{	break;
	}	}
	*dst = EOS;
	szLstChar[0] = *( dst - 3 );
	szLstChar[1] = *( dst - 2 );
	szLstChar[2] = *( dst - 1 );
	return ( TRUE );
}


void colectErrorString( char *ptgt )
{	char szcpy[16], *pFound;
	static char LstChr[4] = { 0 };
	int i, len;

//	strcpy( szLine, ptgt );
	for ( ; ; )									// "ヴ" 対策　ひらがな変換で"ぶ"になっている 
	{	if (( pFound = strstr( ptgt, "\xE3\x81\xB6\xE3\x81\x81"/*"ぶぁ"*/ )) != NULL )
		{	strcpy( pFound, "\xE3\x81\xB0"/*"ば"*/ );
			strcpy( pFound + 3, pFound + 6 );
			continue;
		}
		else if (( pFound = strstr( ptgt, "\xE3\x81\xB6\xE3\x81\x83"/*"ぶぃ"*/ )) != NULL )
		{	strcpy( pFound, "\xE3\x81\xB3"/*"び"*/ );
			strcpy( pFound + 3, pFound + 6 );
			continue;
		}
		else if (( pFound = strstr( ptgt, "\xE3\x81\xB6\xE3\x81\x87"/*"ぶぇ"*/ )) != NULL )
		{	strcpy( pFound, "\xE3\x81\xB9"/*"べ"*/ );
			strcpy( pFound + 3, pFound + 6 );
			continue;
		}
		break;
	}
	for ( i = 0; i < lengthof( lstComoji ); i++ )
	{	for ( pFound = ptgt; pFound != NULL; )
		{	if (( pFound = strstr( pFound, lstComoji[i].szComoji )) == NULL )
			{	break;
			}
			if ( pFound != ptgt )
			{	strncpy( szcpy, pFound - 3, 3 );
			}
			else
			{	szcpy[0] = LstChr[0];
				szcpy[1] = LstChr[1];
				szcpy[2] = LstChr[2];
			}
			szcpy[3] = EOS;
			if ( lstComoji[i].procs & E )							// "ぇ"
			{	if ( strstr( COYAYUYOOK, szcpy ) == NULL )			// "き し ち に ひ み り ぎ じ び ぴ "
				{	if ( strstr( COYUOK, szcpy ) == NULL )			// "で て "
					{	if ( strstr( COAIEOOK, szcpy ) == NULL )	// "うつふ"
						{	if ( strstr( COIOK, szcpy ) == NULL )	// "す て ず で "
							{	strncpy( pFound, "\xE3\x83\xBC"/*"ー"*/, 3 );
				}	}	}	}
				pFound += 3;
			}
			else if ( lstComoji[i].procs & YAYUEYO )			// "ゃゅょ"
			{	if ( strstr( COYAYUYOOK, szcpy ) == NULL )		// "き し ち に ひ み り ぎ じ び ぴ "
				{	if ( strstr( COYUOK, szcpy ) == NULL )		// "で て "
					{	strncpy( pFound, "\xE3\x83\xBC"/*"ー"*/, 3 );
				}	}
				pFound += 3;
			}
			else if ( lstComoji[i].procs & AIO )				// "ぁぃぉ"
			{	if ( strstr( COAIEOOK, szcpy ) == NULL )		// "うつふ"
				{	if ( strstr( COIOK, szcpy ) == NULL )		// "す て ず で "
					{	strncpy( pFound, "\xE3\x83\xBC"/*"ー"*/, 3 );
				}	}
				pFound += 3;
			}
			else if ( lstComoji[i].procs & U )					// "ぅ"
			{	if ( strstr( COUOK, szcpy ) == NULL )			// "と ど "
				{	strncpy( pFound, "\xE3\x83\xBC"/*"ー"*/, 3 );
				}
				pFound += 3;
	}	}	}
	if (( len = ( int )strlen( ptgt )) > 3 )
	{	LstChr[0] = *( ptgt + len - 3 );
		LstChr[1] = *( ptgt + len - 2 );
		LstChr[2] = *( ptgt + len - 1 );
	}
	else
	{	LstChr[0] = LstChr[1] = LstChr[2] = '\0';
	}
}


mecab_model_t *mecab_model_new_build_option()
{	char *ppArgv[16] = { NULL };
//	static char *ppArgv[] = { "", "-d", "D:\\Mytool\\mecab-0.996\\work\\final\\", 0, 0 };
	char szSysDic[_MAX_PATH] = { NULL };
	char szUsrDic[_MAX_PATH] = { NULL };
	int iIdxArg = 0;
	char *p;

	ppArgv[iIdxArg++] = "";
	UniToUTF8(( char * )szSysDictionary, szSysDic, sizeof( szSysDic ) - 1 );
	strcpy( szUsrDic, szSysDic );
	if (( p = strstr( szUsrDic, "work\\" )) != NULL )
	{	struct __finddata64_t c_file;

		strcpy( p, "etc\\mecabrc" );
		ppArgv[iIdxArg++] = "-r";
		ppArgv[iIdxArg++] = szUsrDic;
		if ( _findfirst64( szUsrDic, &c_file ) == -1L )		// ファイル存在確認 
		{	wchar_t msgBuff[_MAX_PATH * 2], pathBuff[_MAX_PATH * 2];

			UTF8ToUni( szUsrDic, ( char * )pathBuff, sizeof( pathBuff ));
			swprintf( msgBuff, L"辞書のmecabrcファイル[%s}が在りません", pathBuff );
			AfxMessageBox( msgBuff );
			return ( NULL );
		}
//		ppArgv[iIdxArg++] = "D:\\Tools\\MeCabon\\mecab-0.996\\etc\\mecabrc";
	}
	if ( szSysDictionary[0] != EOS )
	{	ppArgv[iIdxArg++] = "-d";
//		UniToUTF8(( char * )szSysDictionary, szSysDic, sizeof( szSysDic ) - 1 );
		ppArgv[iIdxArg++] = szSysDic;
	}
	if ( szUserDictionary[0] != EOS )
	{	szUsrDic[0] = '\0';
		ppArgv[iIdxArg++] = "-u";
		UniToUTF8(( char * )szUserDictionary, szUsrDic, sizeof( szUsrDic ) - 1 );
		ppArgv[iIdxArg++] = szUsrDic;
	}
	return ( fpmecab_model_new( iIdxArg, ppArgv ));
}


//
//	再生停止時にMecab.Dllを切り離し辞書を更新可能にするため、この関数でMecab.dllをリンク 
//
HINSTANCE OpenMecabDll()
{	if ( hMecabDLL == NULL )
	{//	wchar_t	szlibMeCabdll[_MAX_PATH];

//		wcscpy_s( szlibMeCabdll, lengthof( szlibMeCabdll ), szMeCabExe );	// インストールされているDLLを使おうとしたが 
//		exchgFileName( szlibMeCabdll, szlibMeCabdll, L"libmecab.dll" );		// 64bitアプリから32bitDLL呼び出しとなりエラー 
//		if (( hMecabDLL = LoadLibrary( szlibMeCabdll )) == NULL )
		if (( hMecabDLL = LoadLibrary( L"libmecab.dll" )) == NULL )
		{	mecabDllLastError = -101;
//			mecabDllLastError = GetLastError();		// error code 193 32bitlib ng
			return ( NULL );
		}
		if (( fpmecab_model_new = ( LPmecab_model_new )GetProcAddress( hMecabDLL, "mecab_model_new" )) == NULL )
		{	CloseMecabDll();
			mecabDllLastError = -102;
			return ( NULL );			// ERROR_DELAY_LOAD_FAILED
		}
#if 0									// 常数なのか？取得できない 
		if (( fpgetLastError = ( LPgetLastError )GetProcAddress( hMecabDLL, "MeCab::getLastError" )) == NULL )
		{	CloseMecabDll();
			mecabDllLastError = -103;
			return ( NULL );			// ERROR_DELAY_LOAD_FAILED
		}
#endif
		if (( fpmecab_model_new_tagger = ( LPmecab_model_new_tagger )GetProcAddress( hMecabDLL, "mecab_model_new_tagger" )) == NULL )
		{	CloseMecabDll();
			mecabDllLastError = -104;
			return ( NULL );			// ERROR_DELAY_LOAD_FAILED
		}
		if (( fpmecab_dictionary_info = ( LPmecab_dictionary_info )GetProcAddress( hMecabDLL, "mecab_dictionary_info" )) == NULL )
		{	CloseMecabDll();
			mecabDllLastError = -105;
			return ( NULL );			// ERROR_DELAY_LOAD_FAILED
		}
		if (( fpmecab_model_new_lattice = ( LPmecab_model_new_lattice )GetProcAddress( hMecabDLL, "mecab_model_new_lattice" )) == NULL )
		{	CloseMecabDll();
			mecabDllLastError = -106;
			return ( NULL );			// ERROR_DELAY_LOAD_FAILED
		}
		if (( fpmecab_destroy = ( LPmecab_destroy )GetProcAddress( hMecabDLL, "mecab_destroy" )) == NULL )
		{	CloseMecabDll();
			mecabDllLastError = -107;
			return ( NULL );			// ERROR_DELAY_LOAD_FAILED
		}
		if (( fpmecab_lattice_set_sentence = ( LPmecab_lattice_set_sentence )GetProcAddress( hMecabDLL, "mecab_lattice_set_sentence" )) == NULL )
		{	CloseMecabDll();
			mecabDllLastError = -108;
			return ( NULL );			// ERROR_DELAY_LOAD_FAILED
		}
		if (( fpmecab_parse_lattice = ( LPmecab_parse_lattice )GetProcAddress( hMecabDLL, "mecab_parse_lattice" )) == NULL )
		{	CloseMecabDll();
			mecabDllLastError = -109;
			return ( NULL );			// ERROR_DELAY_LOAD_FAILED
		}
		if (( fpmecab_lattice_get_bos_node = ( LPmecab_lattice_get_bos_node )GetProcAddress( hMecabDLL, "mecab_lattice_get_bos_node" )) == NULL )
		{	CloseMecabDll();
			mecabDllLastError = -110;
			return ( NULL );			// ERROR_DELAY_LOAD_FAILED
		}
		if (( fpmecab_lattice_destroy = ( LPmecab_lattice_destroy )GetProcAddress( hMecabDLL, "mecab_lattice_destroy" )) == NULL )
		{	CloseMecabDll();
			mecabDllLastError = -111;
			return ( NULL );			// ERROR_DELAY_LOAD_FAILED
		}
		if (( fpmecab_model_destroy = ( LPmecab_model_destroy )GetProcAddress( hMecabDLL, "mecab_model_destroy" )) == NULL )
		{	CloseMecabDll();
			mecabDllLastError = -112;
			return ( NULL );			// ERROR_DELAY_LOAD_FAILED
		}
		if (( fpmecab_lattice_tostr = ( LPmecab_lattice_tostr )GetProcAddress( hMecabDLL, "mecab_lattice_tostr" )) == NULL )
		{	CloseMecabDll();
			mecabDllLastError = -113;
			return ( NULL );			// ERROR_DELAY_LOAD_FAILED
		}
		if (( fpmecab_lattice_set_boundary_constraint = ( LPmecab_lattice_set_boundary_constraint )GetProcAddress( hMecabDLL, "mecab_lattice_set_boundary_constraint" )) == NULL )
		{	CloseMecabDll();
			mecabDllLastError = -114;
			return ( NULL );			// ERROR_DELAY_LOAD_FAILED
	}	}
	pSarchCsv->PostMessageW( UM_STATE_CHANGE, iPlayState, 0 );
	return ( hMecabDLL );
}


//
//	Mecab.dllを切り離し 
//
void CloseMecabDll()
{	if ( hMecabDLL != NULL )
	{	FreeLibrary( hMecabDLL );
		hMecabDLL = NULL;
	}
	if ( pSarchCsv != 0 )
	{	if ( pSarchCsv->m_hWnd != 0 )
		{	pSarchCsv->PostMessageW( UM_STATE_CHANGE, iPlayState, 0 );
	}	}
}


BOOL isOpenMecab()
{	return ( hMecabDLL != NULL );
}


long getmecabDllLastError()
{	return ( mecabDllLastError );
}


int initial_DevKey()								// aquest
{
#ifdef AQTK10REGISTCODE								// ライセンスコードが未定義ならセットしない 
	AquesTalk_SetDevKey( AQTK10REGISTCODE ); 
#endif
	return ( 0 );
}


int initial_dlls()									// mecab & aquest & wave
{
	if ( OpenMecabDll() == NULL )
	{	return ( getmecabDllLastError());
	}
	OkMecab = FALSE;
	if (( model = mecab_model_new_build_option()) == NULL )
	{	return ( -151 );
	}
	if (( mecab = fpmecab_model_new_tagger( model )) == NULL )
	{	return ( -152 );
	}
	const mecab_dictionary_info_t *pDict = fpmecab_dictionary_info( mecab );
/*
struct mecab_dictionary_info_t {
  const char                     *filename;		// On Windows, filename is stored in UTF-8 encoding
  const char                     *charset;		// character set of the dictionary. e.g., "SHIFT-JIS", "UTF-8"
  unsigned int                    size;			// How many words are registered in this dictionary.
  int                             type;			// this value should be MECAB_USR_DIC, MECAB_SYS_DIC, or MECAB_UNK_DIC.
  unsigned int                    lsize;		// left attributes size
  unsigned int                    rsize;		// right attributes size
  unsigned short                  version;		// version of this dictionary
  struct mecab_dictionary_info_t *next;			// pointer to the next dictionary info.
};
*/
	if (( lattice = fpmecab_model_new_lattice( model )) == NULL )
	{	fpmecab_destroy( mecab );
		return ( -153 );
	}
	OkMecab = TRUE;
	if ( SelectTts == SAPI54 )
	{	CComPtr <ISpObjectToken>		cpAudioOutToken;
		CComPtr <ISpObjectToken>		cpToken;
		CComPtr <IEnumSpObjectTokens>	cpEnum;

		if ( FAILED( ::CoInitialize( NULL )))
		{	return( -201 );
		}
		HRESULT hr = CoCreateInstance( CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, ( void ** )&pVoice );
		if ( SUCCEEDED( hr ))		// Voice を指定します。
		{	int i;

			fgVoiceVox = FALSE;
			for ( i = 0;  pszRegistryEntry[i] != NULL; i++ )
			{	hr = SpEnumTokens( pszRegistryEntry[i], sapi54.pszVoiceName, NULL, &cpEnum );	//Cortana voice
				if ( cpEnum != NULL )
				{	break;
			}	}
			if ( cpEnum == NULL )
			{	// 指定のVoiceが見つからなかったら・・・
			}
			else if ( wcsstr( sapi54.pszVoiceName, L"VOICEVOX" ) != NULL )
			{	fgVoiceVox = TRUE;
			}
//			hr = SpEnumTokens( L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech_OneCore\\Voices", sapi54.pszVoiceName, NULL, &cpEnum );	//Cortana voice
//			hr = SpEnumTokens( SPCAT_VOICES, L"language = 411", NULL, &cpEnum );	// 属性L "language = 411"の音声トークンを列挙します。
//			hr = SpEnumTokens( L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech_OneCore\\Voices", L"Language = 411", NULL, &cpEnum );		//Cortana voice
///			hr = SpEnumTokens( L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech Server\\v11.0\\Voices", L"Language = 411", NULL, &cpEnum ); //Speech Platform v11.0 (Required runtime)
//https://software.nzsw.net/windows-10-%E3%81%AB%E6%90%AD%E8%BC%89%E3%81%95%E3%82%8C%E3%82%8B%E9%9F%B3%E5%A3%B0%E5%90%88%E6%88%90%E3%81%AB%E3%81%A4%E3%81%84%E3%81%A6%E8%AA%BF%E3%81%B9%E3%81%A6%E3%81%BF%E3%81%9F/
		}								// SAPI default voice (OS language pack) 411:Japanese
		if ( SUCCEEDED( hr ))			// 最も一致するトークンを取得します。
		{	hr = cpEnum->Next( 1, &cpToken, NULL );
		}
		if ( SUCCEEDED( hr ))			// 音声を設定します。
		{	hr = pVoice->SetVoice( cpToken );
		}
		if ( SUCCEEDED( hr ))			// 出力をオーディオデバイスに設定します。
		{	hr = pVoice->SetOutput( cpAudioOutToken, TRUE );
		}
		hr = pVoice->SetVolume( 60 );	// 0 - 100
		if ( fgVoiceVox == TRUE )
		{	StartProcess( NULL, szVoiceVoxPath, NULL, &VoiceVoxProcessInfomation );
		}
	}
	else if ( SelectTts == AQTK10 )
	{
#ifdef JTALK
		int count;
		HtsVoiceFilelist *ptr;
		//openjtalk_setVerbose(true);
		OpenJTalk *oj = openjtalk_initialize( NULL, NULL, NULL );
		if ( oj == NULL )
		{	return ( -1001 );
		}
#ifdef _WIN32
		HtsVoiceFilelist *list = openjtalk_getHTSVoiceListSjis( oj );
#else
		HtsVoiceFilelist *list = openjtalk_getHTSVoiceList( oj );
#endif
		for ( count = 0, ptr = list; ptr != NULL; ptr = ptr->succ, count++ )
		{	if ( count == 1 )
			{
#ifdef _WIN32
				openjtalk_setVoiceSjis( oj, ptr->path );
#else
				openjtalk_setVoice( oj, ptr->path );
#endif
//				printf( "current voice: %s\n", ptr->name );
				break;
			}
		}
		openjtalk_clearHTSVoiceList( oj, list );
#else
		voice				= gVoice_F1;							// 声質パラメータ
		voice.spd			= 90;	
		voice.vol			= 20;
		initial_DevKey();
#ifdef AQTK10REGISTCODE												// ライセンスコードが未定義ならセットしない 
		AquesTalk_SetDevKey( AQTK10REGISTCODE ); 
#endif

#if 0		// メインダイヤログのボリューム設定がうまくできないので移動 
		wfe.wFormatTag		= WAVE_FORMAT_PCM;
		wfe.nChannels		= 1;									//モノラル
		wfe.wBitsPerSample	= BITWIDTH;								//量子化ビット数
		wfe.nBlockAlign		= wfe.nChannels * wfe.wBitsPerSample/8;
		wfe.nSamplesPerSec	= SRATE;								//標本化周波数
		wfe.nAvgBytesPerSec	= wfe.nSamplesPerSec * wfe.nBlockAlign;
		for ( i = 0; i < lengthof( bcWave ); i++ )
		{	bcWave[i] = 0;
			lpWave[i] = NULL;
		}
		if (( retstate = waveOutOpen( &hWaveOut, WAVE_MAPPER, &wfe, (DWORD_PTR)StaticWaveOutProc, (DWORD_PTR)0, CALLBACK_FUNCTION )) != MMSYSERR_NOERROR )
		{	switch ( retstate )
			{ case MMSYSERR_ALLOCATED:
				std::cout << "Specified resource is already allocated." << std::endl;
				break;
			  case MMSYSERR_BADDEVICEID:
				std::cout << "Specified device identifier is out of range." << std::endl;
				break;
			  case MMSYSERR_NODRIVER:
				std::cout << "No device driver is present." << std::endl;
				break;
			  case MMSYSERR_NOMEM:
				std::cout << "Unable to allocate or lock memory." << std::endl;
				break;
			  case WAVERR_BADFORMAT:
				std::cout << "Attempted to open with an unsupported waveform-audio format." << std::endl;
				break;
			  case WAVERR_SYNC:
				std::cout << "The device is synchronous but waveOutOpen was called without using the WAVE_ALLOWSYNC flag." << std::endl;
				break;
			  default:
				std::cout << "other error." << std::endl;
				break;
			}
			return ( -1002 );
		}
#endif
#endif
	}
	lpWave[0] = NULL;			// wave buf
	bcWave[0] = 0;				// alloc size
	bufHiraganaJis[0] = EOS;
	InitializeCriticalSection( &cstPlayRec );
	return ( 0 );
}


//　叱叱　屏屏　并并　倶俱　剥剝　呑吞　嘘噓　妍姸　痩瘦　繋繫
//　？？　？？　？？
//JIS 4f54　7e7b　7e7c
void xchgJIS2004( wchar_t *pmbstring )
{	static wchar_t unsafe[][2] =
	{	{	L'俱',	L'倶'	},
		{	L'剝',	L'剥'	},
		{	L'吞',	L'呑'	},
		{	L'噓',	L'嘘'	},
		{	L'姸',	L'妍'	},
		{	L'瘦',	L'痩'	},
		{	L'繫',	L'繋'	}
	};	//辞書未定義？ 
	wchar_t *p;
	int no;

	for ( no = 0; no < lengthof( unsafe ); no++ )
	{	while (( p = wcschr( pmbstring, unsafe[no][0])) != NULL )
		{	*p = unsafe[no][1];
}	}	}


BOOL iscomoji( unsigned char *pszUtf8 )
{	int Utf8chr, inx;

	if ( *pszUtf8 != 0xe3 )
	{	return ( FALSE );
	}
	Utf8chr = ( *( pszUtf8 + 1 ) << 8 ) | *( pszUtf8 + 2 );
	if ( 0x82a1 <= Utf8chr )
	{	if (( 0x8380 <= Utf8chr ) && ( Utf8chr <= 0x83bc ))
		{	inx = Utf8chr - 0x8380 + 0x80;
		}
		else if ( Utf8chr <= 0x82bf )
		{	inx = Utf8chr - 0x82a0 + 0x60;
		}
		else
		{	return ( FALSE );
	}	}
	else
	{	if (( 0x8181 <= Utf8chr ) && ( Utf8chr <= 0x81bf ))
		{	inx = Utf8chr - 0x8180;
		}
		else if (( 0x8280 <= Utf8chr ) && ( Utf8chr <= 0x8296 ))
		{	inx = Utf8chr - 0x8280 + 0x40;
		}
		else
		{	return ( FALSE );
	}	}
	return ( tblErrorStart[inx]);
}


BOOL isLastCharoomoji( unsigned char *pszUtf8 )	// バッファ末は大文字でない？ 
{	unsigned char *psz;
	int len;

	for ( psz = pszUtf8 + ( len = strlen(( const char * )pszUtf8 )) - 1; len > 0; len--, psz-- )
	{	if ( !isU8nonTop( *psz ))
		{	return( iscomoji( psz ));
	}	}
	return ( FALSE );
}


int lineCvtAndPlay( wchar_t *pmbstring, struct PLAYINGSENTENCE *ps )
{	static int lnSurface = 0;
	static char szSurface[MAX_LINE], numbers[MAX_LINE];
	char HatuonBuff[MAX_LINE], *p;
	int i, len, retry;
	int size, fgNxtKana;
	BOOL fgComma;
	unsigned char *wav;
	const mecab_node_t *node;

	if (( hMecabDLL == NULL ) || ( OkMecab == FALSE ) || ( lattice == NULL ))
	{	return ( 0 );
	}
	if ( wcscmp( pmbstring, L"\n" ) == 0 )		// utf8でもおなじ？
	{	return ( 0 );
	}
	xchgJIS2004( pmbstring );					// JIS2004追加漢字を以前の漢字に置き換え 
	memset( bufUTF8, 0, sizeof( bufUTF8 ));
	UniToUTF8(( char * )pmbstring, &( bufUTF8[0]), sizeof( bufUTF8 ));
	lnSurface = 0;
	fpmecab_lattice_set_sentence( lattice, bufUTF8 );
	fpmecab_parse_lattice( mecab, lattice );
	node = fpmecab_lattice_get_bos_node( lattice );
	node = node->next;

	for ( numbers[0] = bufHiraganaJis[0] = EOS; node != NULL; node = node->next )
	{	if ( iPlayState == STOP )
		{	break;
		}
		if ( node->posid == 0 )
		{	if ( numbers[0] == EOS )		// 数字列バッファは空？ 
			{	continue;
			}
			strcpy_s( HatuonBuff, sizeof( HatuonBuff ) - 1, numbers );
		}
		else
		{	//UTF8ToShiftJis(( char * )node->feature, bufShiftJis, sizeof( bufShiftJis ));
			strcpy(( char * )bufShiftJis, ( char * )node->feature );
			if ( extractHatuonfeature( HatuonBuff, ( char * )bufShiftJis ) == 0 )
			{	strncpy_s( szSurface, sizeof( szSurface ), ( char * )node->surface, node->length );
				szSurface[node->length] = EOS;
				UTF8ToUni(( char * )szSurface, bufShiftJis, sizeof( bufShiftJis ));
				wcscat_s(( wchar_t * )bufShiftJis, sizeof( bufShiftJis ) / sizeof( wchar_t ), L"\n" );
				pCorpasDlg->OnDsplayOutputDlg();
				if ( *bufShiftJis == L'0' )
				{	_sleep( 0 );
				}
				pSarchCsv->CatUnnounStr(( wchar_t * )bufShiftJis );
//				UTF8ToShiftJis(( char * )szSurface, bufShiftJis, sizeof( bufShiftJis ));
				strcpy( bufShiftJis, szSurface );
//				printf( "node->surface:[%s]\n",  bufShiftJis );
				extractHatuonsurface( HatuonBuff, ( char * )bufShiftJis );
			}
			if ( HatuonBuff[0] == '\0' )
			{	continue;
			}
			if (( *numbers != EOS ) && ( *bufShiftJis == ',' ) &&	// 数字列のカンマ区切り 
						( isdigit( *((( char * )node->surface ) + 1 ) )))
			{	continue;
			}
			else if ( isdigit(( *( unsigned char * )&HatuonBuff[0])))
			{	if ( numbers[0] == EOS )
				{	fgComma = FALSE;
				}
				strcat_s( numbers, sizeof( numbers ) - 1, HatuonBuff );
				continue;
			}
			else if (( HatuonBuff[0] == '.' ) && ( fgComma == FALSE ))	// 小数点対応 
			{	fgComma = TRUE;
				strcat_s( numbers, sizeof( numbers ) - 1, HatuonBuff );
				continue;
			}
			else if ( HatuonBuff[0] == ':' )							// ':' 対応  
			{	strcat_s( numbers, sizeof( numbers ) - 1, HatuonBuff );
				continue;
			}
			else if ( numbers[0] != EOS )								// 半角数字でない文字出現時に数字列バッファが空でなければ 
			{	strcat_s( numbers, sizeof( numbers ) - 1, HatuonBuff );
				strcpy_s( HatuonBuff, sizeof( HatuonBuff ) - 1, numbers );
				numbers[0] = EOS;
		}	}
#if 0		// 最終行が区切り記号で終わっていないときこちらが必要 
		conv2Hiragana(( unsigned char * )&( bufHiraganaJis[strlen( bufHiraganaJis )]),( unsigned char * )&( HatuonBuff[0]));
#else		// 行頭がアクエストークのエラー文字で始まっている際に弾く 
		if ( iscomoji(( unsigned char * )&( HatuonBuff[0])))					// 追記する文字が小文字か？ 
		{	if ( !isLastCharoomoji(( unsigned char * )&( bufHiraganaJis[0])))	// 追記直前文字が大文字でない？ 
			{	continue;														// AquesTalkが落ちるのでスキップ 
		}	}
		conv2Hiragana(( unsigned char * )&( bufHiraganaJis[strlen( bufHiraganaJis )]),( unsigned char * )&( HatuonBuff[0]));
		if (( node->next )->posid != 0 ) //|| ( *pmbstring == '\0' ))
		{	if ( node->next != NULL )	// 次の語彙が小文字で始まるかチェック 
			{	if ( iscomoji(( unsigned char * )( node->next )->surface ))
				{	continue;
				}
#if 0
				if (( fgNxtKana = *(( unsigned char * )( node->next )->surface )) == 0xe3 )
				{	fgNxtKana = ( *(( unsigned char * )( node->next )->surface + 1 ) << 8 ) | *(( unsigned char * )( node->next )->surface + 2 );
//					else if (( 0x82a1 <= fgNxtKana ) && ( fgNxtKana <= 0x82bf ))
//					else if (( 0x8380 <= fgNxtKana ) && ( fgNxtKana <= 0x83bc ))
					if ( 0x82a1 <= fgNxtKana )
					{	if (( 0x8380 <= fgNxtKana ) && ( fgNxtKana <= 0x83bc ))
						{	if ( tblErrorStart[fgNxtKana - 0x8380 + 0x80] != 0 )		// UTF ひらがな カタカナ
							{	continue;
						}	}
						else if ( fgNxtKana <= 0x82bf )
						{	if ( tblErrorStart[fgNxtKana - 0x82a0 + 0x60] != 0 )		// UTF ひらがな カタカナ
							{	continue;
					}	}	}
					else
					{	// if (( 0x8181 <= fgNxtKana ) && ( fgNxtKana <= 0x81bf ))
						// else if (( 0x8280 <= fgNxtKana ) && ( fgNxtKana <= 0x8296 ))
						if (( 0x8181 <= fgNxtKana ) && ( fgNxtKana <= 0x81bf ))
						{	if ( tblErrorStart[fgNxtKana - 0x8180] != 0 )				// UTF ひらがな カタカナ
							{	continue;
						}	}
						else if (( 0x8280 <= fgNxtKana ) && ( fgNxtKana <= 0x8296 ))
						{	if ( tblErrorStart[fgNxtKana - 0x8280 + 0x40] != 0 )		// UTF ひらがな カタカナ
							{	continue;
					}	}	}
#if 0
					if (( 0x8181 <= fgNxtKana ) && ( fgNxtKana <= 0x81bf ))
					{	if ( tblErrorStart[fgNxtKana - 0x8180] != 0 )				// UTF ひらがな カタカナ
						{	continue;
					}	}
					else if (( 0x8280 <= fgNxtKana ) && ( fgNxtKana <= 0x8296 ))
					{	if ( tblErrorStart[fgNxtKana - 0x8280 + 0x40] != 0 )		// UTF ひらがな カタカナ
						{	continue;
					}	}
					else if (( 0x82a1 <= fgNxtKana ) && ( fgNxtKana <= 0x82bf ))
					{	if ( tblErrorStart[fgNxtKana - 0x82a0 + 0x60] != 0 )		// UTF ひらがな カタカナ
						{	continue;
					}	}
					else if (( 0x8380 <= fgNxtKana ) && ( fgNxtKana <= 0x83bc ))
					{	if ( 　[fgNxtKana - 0x8380 + 0x80] != 0 )		// UTF ひらがな カタカナ
						{	continue;
					}	}
#endif
				}
#endif
		}	}
#endif
		if ((( len = ( int )strlen( bufHiraganaJis )) < 32 ) && (( node->next )->posid != 0 ))
		{	continue;
		}
		if ( len <= 0 )
		{	continue;
		}
		colectErrorString( bufHiraganaJis );
//		printf( "%-16s %s\n", bufHiraganaJis, bufShiftJis );
//		std::cout << bufHiraganaJis << "\t\t" << bufShiftJis << std::endl;
		if (( SelectTts == AQTK10 )  && !fgSkipspeak )
		{	for ( retry = 0; ; retry++ )
			{	if (( wav = AquesTalk_Synthe_Utf8( &voice, bufHiraganaJis, &size )) != NULL )
				{	break;
				}
				if ( retry != 0 )
				{	break;
				}
				if ( strncmp(( p = bufHiraganaJis ), CYOUON, 3 ) == 0 )
				{	for ( p += 3; strncmp( p, CYOUON, 3 ) == 0; p += 3 )
					{	;
					}
					strcpy_s( bufHiraganaJis, sizeof( bufHiraganaJis ), p );
					continue;
				}
				for ( p = bufHiraganaJis; ( p = strstr( p, "\xE3\x81\xA3\xE3\x83\xBC"/*"っー"*/ )) != NULL; )
				{	strcpy( p + 3, p + 6 );		// っの後のーは省略 
				}
				for ( p = bufHiraganaJis; ( p = strstr( p, "\xE3\x81\xA3\xE3\x81\xA3"/*"っっ"*/ )) != NULL; )
				{	strcpy( p + 3, p + 6 );		// っの後のっは省略 
			}	}
			if ( wav == NULL )
			{	wchar_t errStr[512];
				const mecab_dictionary_info_t *pDict = fpmecab_dictionary_info( mecab );

//				ShiftJisToUni( bufHiraganaJis, bufUTF8, sizeof( bufUTF8 ) / 2 );
				UTF8ToUni( bufHiraganaJis, bufUTF8, sizeof( bufUTF8 ) / 2 );
				swprintf_s( errStr, lengthof( errStr ), L"error! AquesTalk_Synthe( \"%s\" ) !\n", bufUTF8 );
				pCorpasDlg->OnDsplayOutputDlg();
				pSarchCsv->SetEditBox( errStr );
				pSarchCsv->SetEditBox( pmbstring );
				pSarchCsv->SetEditBox( L"\n" );
				if ( wcsstr( pmbstring, L"繫" ) != NULL )		// 繫:%E7%B9%AB
				{	swprintf_s( errStr, lengthof( errStr ), L"[繫]!=[繁]\n" );	// 繫!=繁 E7B981
					pSarchCsv->SetEditBox( errStr );
				}
				Sleep( 500 );
				bufHiraganaJis[0] = EOS;	// 何度も同じ文字で引っ掛かるので 
				continue;
			}
			while ( 1 )
			{	EnterCriticalSection( &cstPlayRec );
				i = ctPlayRec;
				LeaveCriticalSection( &cstPlayRec );
				if ( i < lengthof( lpWave ))
				{	break;
				}
				Sleep( 50 );
			}
			Sleep( 50 );
			aqtk10waveInitialize();
			if ( bcWave[ixPlayRec] < size )
			{	bcWave[ixPlayRec] = size * 12 / 10;
				if ( bcWave[ixPlayRec] < DEFSIZE )
				{	bcWave[ixPlayRec] = DEFSIZE;
				}
				if ( lpWave[ixPlayRec] != NULL )
				{	waveOutUnprepareHeader( hWaveOut, &whdr[ixPlayRec], sizeof( WAVEHDR ));
					free( lpWave[ixPlayRec]);
				}
				lpWave[ixPlayRec] = ( LPBYTE )calloc( bcWave[ixPlayRec], sizeof( char ));
				whdr[ixPlayRec].lpData = (LPSTR)lpWave[ixPlayRec];
//				whdr[ixPlayRec].dwBufferLength = size;
				whdr[ixPlayRec].dwFlags = 0;
/* flags for dwFlags field of WAVEHDR */
//#define WHDR_DONE       0x00000001  /* done bit */
//#define WHDR_PREPARED   0x00000002  /* set if this header has been prepared */
//#define WHDR_BEGINLOOP  0x00000004  /* loop start block */
//#define WHDR_ENDLOOP    0x00000008  /* loop end block */
//#define WHDR_INQUEUE    0x00000010  /* reserved for driver */
				whdr[ixPlayRec].dwLoops = 0;
				waveOutPrepareHeader( hWaveOut, &whdr[ixPlayRec], sizeof( WAVEHDR ));
			}
//			memset( lpWave[ixPlayRec], 0, bcWave[ixPlayRec]);
			whdr[ixPlayRec].dwBufferLength = size;
			memcpy( lpWave[ixPlayRec], wav, size );
			AquesTalk_FreeWave( wav );
			ctCnfDone++;
			waveOutWrite( hWaveOut, &whdr[ixPlayRec], sizeof( WAVEHDR ));
			ixPlayRec = ( ixPlayRec + 1 ) % lengthof( lpWave );
			EnterCriticalSection( &cstPlayRec );
			ctPlayRec++;
			LeaveCriticalSection( &cstPlayRec );
//			while ( ctCnfDone != ctRcvDone )	_sleep( 100 );
//			waveOutUnprepareHeader( hWaveOut, &whdr[0], sizeof( WAVEHDR ));
//			free( lpWave[0]);
		}
		bufHiraganaJis[0] = EOS;
	}
	if ( SelectTts == SAPI54 )
	{	if ( pVoice )
		{	SPVOICESTATUS Status;
			__nullterminated WCHAR *pszLastBookmark;

			while ( 1 )
			{	HRESULT hr = pVoice->GetStatus( &Status, &pszLastBookmark );
				if (( Status.ulLastStreamQueued - Status.ulCurrentStream ) < 10 )
				{	break;
				}
				Sleep( 100 );
	}	}	}
	return ( 0 );
}


int fainalize_dlls()
{
	while ( ctPlayRec > 0 )
	{	Sleep( 1000 );
	}
	if ( SelectTts == SAPI54 )
	{	if ( pVoice )
		{	SPVOICESTATUS Status;
			__nullterminated WCHAR *pszLastBookmark;

			while ( 1 )
			{	HRESULT hr = pVoice->GetStatus( &Status, &pszLastBookmark );
				if ( Status.dwRunningState != SPRS_IS_SPEAKING )
				{	break;
				}
				Sleep( 100 );
			}
			pVoice->Release();
			pVoice = NULL;
		}
		::CoUninitialize();
		waveOutUnprepareHeader( hWaveOut, &whdr[0], sizeof( WAVEHDR ));
		if ( lpWave[0] != NULL )	// wave buf
		{	free( lpWave[0]);
			lpWave[0] = NULL;
		}
		bcWave[0] = 0;				// alloc size
		DeleteCriticalSection( &cstPlayRec );
		if ( m_szWFileName != ( wchar_t * )NULL )
		{	free( m_szWFileName );
			m_szWFileName = ( wchar_t * )NULL;
		}
		if ( fgVoiceVox == TRUE )
		{	EndProcess( &VoiceVoxProcessInfomation );
			fgVoiceVox = FALSE;
		}
	}
	else if ( SelectTts == AQTK10 )
	{	waveOutUnprepareHeader( hWaveOut, &whdr[0], sizeof( WAVEHDR ));
		if ( lpWave[0] != NULL )	// wave buf
		{	free( lpWave[0]);
			lpWave[0] = NULL;
		}
		bcWave[0] = 0;				// alloc size
		DeleteCriticalSection( &cstPlayRec );
	}
//	waveOutClose( hWaveOut );
	if ( hMecabDLL != NULL )
	{	fpmecab_destroy( mecab );
		fpmecab_lattice_destroy( lattice );
		fpmecab_model_destroy( model );
		CloseMecabDll();
	}
	return ( 0 );
}


void appVolSet( USHORT vol )
{	ULONG val;

#if 0
	if ( SelectTts == SAPI54 )
	{	if ( pVoice )
		{	pVoice->SetVolume( vol & 0xffff );
	}	}
	else if ( SelectTts == AQTK10 )
#endif
	if ( hWaveOut != 0 )
	{	val = ( vol << 16 ) | ( vol & 0xffff );
		if ( waveOutSetVolume( hWaveOut, val ) != MMSYSERR_NOERROR )
		{	return;
	}	}
}


USHORT appVolGet()
{	DWORD val;

	if ( hWaveOut != 0 )
	{	if ( waveOutGetVolume( hWaveOut, &val ) == MMSYSERR_NOERROR )
		{	return ( val & 0xffff );
	}	}
	return ( 0 );
}


