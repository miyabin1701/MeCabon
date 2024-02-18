
// PlaySapi.cpp : 実装ファイル
//
/*
	・SAPI　再生の問題点と改善試行について
	・最初にAquesTalkへ流す文字列をそのまま流したところセンテンス間に無駄に長い無音時間が生じた。
	　これは渡す文字列がひらがなのため、再度漢字カナ変換処理が走っているためか？
	　1センテンスづつしか渡さないために、再生後に変換処理に時間がかかっている？
	・再生キューに１０程度貯めるように出力したが改善ならず
	・ひらがなを渡しているが、再度漢字カナ変換などをしているせいで時間がかかる？
	　SSMLで日本語発音記号であるカタカナにして渡したが、改善ならず
	・SSML解析処理と発音がシーケンシャルに走っているため時間が空く？
	　一旦SAPIでWaveファイルを生成して、再生を分け、再生中に次のWavw生成してみたが改善ならず
	・上記で生成されたWaveファイルをバイナリーエディタで観たところ、Waveデータの先頭と末尾に
	　無音区間が結構在った。
	　Waveファイルをメモリーに読み込んだ際、再生開始アドレスを後ろにづらし、再生バイト数を減
	　じることで無音区間を減らそうとしたが、あまり減らすと発音しなくなるよう
	　また、システム暴走に繋がるのか？
	・Waveデータの末尾の無音部分のみ削除（データ数のみ減）は安定しているようだけど、発音がおかしい
　	　一旦保留　要再挑戦か？

	SAPIは、どうも再生する文章を全て渡すことを前提に作られているのかもしれない。
	テキストレベルで何十メガものノベルを一括Wave変換は無謀だし、短文節に区切ってSapiを呼び出す
	のは通常の事と考える。SAPIは短文節の連続呼び出しでも問題無いよう改善してほしい。
	また、Wave生成にパイプを使用できるようにもSAPIを改善してほしいものだ。
	名前付きパイプにWave出力したところSpeak()から戻ってこなかった。
//
// VoiceVox SAPIForVOICEVOX 対応 
// SAPIで数字列変換時１０など１桁目が０の時じゅうれいと発声した。修正
// 下位桁が全て０の時を考慮が必要か
//
*/

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


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//#define JTALK	TRUE
//#define SAPI5P4	TRUE
//#define PIPE


extern HINSTANCE			hMecabDLL;				// Handle to DLL
extern mecab_model_t		*model, *another_model;
extern mecab_t				*mecab;
extern mecab_lattice_t		*lattice;
extern BOOL					OkMecab;
extern BOOL					fgSkipspeak;
extern CCorpasDlg			*pCorpasDlg;
extern CSarchCsv			*pSarchCsv;
extern ISpVoice				*pVoice;
extern CComPtr <ISpStream>	cpWavStream;
extern CRITICAL_SECTION		cstPlayRec;
extern volatile int			ctPlayRec;				// waveOutWrite touroku count
extern int					ixPlayRec;				// waveOutWrite touroku index
extern int					ctCnfDone;
extern HWAVEOUT				hWaveOut;
extern enum PLAYSTATE		iPlayState;
extern struct SAPI54SETTING	sapi54;
extern TTSNAME				SelectTts;
extern enum WAVEOUTOPENTATE	StateWaveOutOpen;		// WaveOutOpen wfe設定の種別 
extern WAVEFORMATEX			wfe;

extern char					bufUTF8[MAX_LINE];
extern char					bufShiftJis[MAX_LINE];
extern char					bufHiraganaJis[MAX_LINE];
extern WAVEHDR				whdr[1];				// wave header
extern LPBYTE				lpWave[1];				// wave buf
extern long long int		bcWave[1];				// alloc size


extern void xchgJIS2004( wchar_t *pmbstring );
extern DWORD GetFileSize( wchar_t *Path );
extern void UniToUTF8( char *pszUni, char *pszUtf8, int lenUtf8 );
extern void UTF8ToUni( char *pszUtf8, char *pszUni, int lenUni );
extern BOOL extractHatuonfeature( char *dst, char *src );
extern BOOL extractHatuonsurface( char *dst, char *src );
extern int waveFainalize();
extern void __stdcall StaticWaveOutProc( HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance,
                                           DWORD dwParam1, DWORD dwParam2 );
wchar_t *m_szWFileName = ( wchar_t * )NULL;	// grobal for free


unsigned char *conv2Katakana( unsigned char *pKatakana, unsigned char *pSorceStr )
{	unsigned char *p, *q, zenchr[4];
	unsigned long ch;
//	static char *pszOnseikigou = "゜ ・ ー ヽ ヾ 。 、";
//	static char *pszOnseikigou = "\xE3\x82\x9C \xE3\x83\xBB \xE3\x83\xBC \xE3\x83\xBD \xE3\x83\xBE \xE3\x80\x82 \xE3\x80\x81";
	static char *pszOnseikigou = "\xE3\x82\x9C \xE3\x83\xBB \xE3\x83\xBD \xE3\x83\xBE";
	static char *pszOnseikigouH = "!+.?_|";		// '除外 

	for ( zenchr[3] = '\0', p = pSorceStr, q = pKatakana; *p != EOS; )
	{	if ( isU8Hankaku( *p ))
		{	if ( !isdigit( *p ) && ( *p != '-' ) && ( *p != '.' ) && !isalpha( *p ))
			{	if ( strchr( pszOnseikigouH, *p ) == NULL )
				{	p++;
					continue;
			}	}
			*q++ = *p++;
//	Japanese Phonemes Microsoft Speech API 5.4
//		UNI		SJIS	
//	!	0021	21		Sentence end (exclamation)
//	'	0027	27		Accent position				1重引用符と発音する… 
//	+	002B	2b		Accent boundary
//	.	002E	2e		Sentence end (standard)
//	?	003F	3f		Sentence end (interrogative)
//	_	005F	5f		One mora pause
//	|	007C	7c		Phrase boundary
//	゜	309C	814b	Semi-voiced sound
//	・	30FB	8145	Middle dot
//	ー	30FC	815b	Prolonged sound
//	ヽ	30FD	8152	Iteration
//	ヾ	30FE	8153	Voiced iteration

//	　	3000	8140	space	pouse
//	、	3001	8141		
//	。	3002	8142		
		}
		else // if ( _ismbblead( *p ))
		{	ch = ( zenchr[0] = *p ) << 16 | ( zenchr[1] = *( p + 1 )) << 8 | ( zenchr[2] = *( p + 2 ));
			if (( 0xE382BF/*u8'タ'*/ >= ch ) && ( ch >= 0xE382A1/*u8'ァ'*/ ))		// 元々カタカナ？
			{	;
			}
			else if (( 0xE383Be/*u8'ヾ'*/ >= ch ) && ( ch >= 0xE38380/*u8'ダ'*/ ))
			{	if ( 0xE38382/*u8'ヂ'*/ == ch )
				{	ch = 0xE382B8;		// u8'ジ'
				}
				else if ( 0xE38385/*u8'ヅ'*/ == ch )
				{	ch = 0xE382BA;		// u8'ズ'
				}
				else if ( 0xE383bb/*u8'・'*/ == ch )		// Microsoft Speech API 5.4 Japanese Phonemes 
				{	p += 3;									// Middle dot と記載されているが通すとエラーになる 
					continue;
			}	}
			else if (( 0xE3819F/*u8'た'*/ >= ch ) && ( ch >= 0xE38181/*u8'ぁ'*/ ))	//    'ぁ' ~ 'タ'
			{	ch += 0xE382A1/*u8'ァ'*/ - 0xE38181/*u8'ぁ'*/;
			}
			else if (( 0xE381BF/*u8'み'*/ >= ch ) && ( ch >= 0xE381A0/*u8'だ'*/ ))	//    'ダ' ~ 'み'
			{	ch += 0xE38380/*u8'ダ'*/ - 0xE381A0/*u8'だ'*/;
			}
			else if (( 0xE38296/*u8'ゖ'*/ >= ch ) && ( ch >= 0xE38280/*u8'む'*/ ))	//    'ダ' ~ 'む'
			{	ch += 0xE383A0/*u8'ム'*/ - 0xE38280/*u8'む'*/;
			}
			else if ( 0xE3829C/*u8'゜'*/ == ch )										//    '゜'
			{	;
			}
			else if ( isU8digit( ch ))						// 全角数字は半角数字に直す
			{	ch -= ( 0xEFBC90/*u8'０'*/ - '0' );
				*q++ = ( char )ch;
				p += 3;
				continue;
			} 
			else if ( ch == 0xEFBC8D/*u8'－'*/ )						// -
			{	*q++ = '-';
				p += 3;
				continue;
			} 
			else if ( ch == 0xEFBC8E/*u8'．'*/ )							// .
			{	*q++ = '.';
				p += 3;
				continue;
			} 
			else if ( isU8AlphaL( ch ))					// 全角英字は半角英字に直す
			{	ch -= ( 0xEFBCA1/*u8'Ａ'*/ - 'A' );
				*q++ = ( char )ch;
				p += 3;
				continue;
			} 
			else if ( isU8AlphaS( ch ))					// 全角英字は半角英字に直す
			{	ch -= ( 0xEFBD81/*u8'ａ'*/ - 'a' );		// 読みに小文字区別ある？ 
				*q++ = ( char )ch;
				p += 3;
				continue;
			} 
			else if ( strstr( pszOnseikigou, ( char * )zenchr ) == NULL )
			{	p += 3;
				continue;
			}
			*q++ = ( unsigned char )( ch >> 16 );
			*q++ = ( unsigned char )(( ch >> 8 ) & 0xff );
			*q++ = ( unsigned char )( ch & 0xff );
			p += 3;
		}
	}
	*q++ = EOS;
	return ( pKatakana );
}


void num2yomi( char *pszYomi, int cbYomi, char *szNumbers )
{	static char *szDigitYomi[] =
	{	"\xE3\x83\xAC\xE3\x83\xBC",	// 0 レー
		"\xE3\x82\xA4\xE3\x83\x81",	// 1 イチ
		"\xE3\x83\x8B",				// 2 ニー
		"\xE3\x82\xB5\xE3\x83\xB3",	// 3 サン
		"\xE3\x83\xA8\xE3\x83\xB3",	// 4 ヨン
		"\xE3\x82\xB4",				// 5 ゴー
		"\xE3\x83\xAD\xE3\x82\xAF",	// 6 ロク
		"\xE3\x83\x8A\xE3\x83\x8A",	// 7 ナナ
		"\xE3\x83\x8F\xE3\x83\x81",	// 8 ハチ
		"\xE3\x82\xAD\xE3\x83\xA5\xE3\x83\xBC"	// 9 キュー
	};
	static char *szKetaYomi[] =
	{	"\xE3\x82\xB8\xE3\x83\xA5\xE3\x83\xBC",	//		10 ジュー
		"\xE3\x83\x92\xE3\x83\xA3\xE3\x82\xAF",	//		百 ヒャク
		"\xE3\x82\xBB\xE3\x83\xB3",				// ^3	千 セン
		"\xE3\x83\x9E\xE3\x83\xB3",				// ^4	万マン
		"\xE3\x82\xB8\xE3\x83\xA5\xE3\x83\xBC"
			"\xE3\x83\x9E\xE3\x83\xB3",			//		十万ジューマン
		"\xE3\x83\x92\xE3\x83\xA3\xE3\x82\xAF"
			"\xE3\x83\x9E\xE3\x83\xB3",			//		百万ヒャクマン
		"\xE3\x82\xBB\xE3\x83\xB3"
			"\xE3\x83\x9E\xE3\x83\xB3",			//		千万センマン
		"\xE3\x82\xAA\xE3\x82\xAF",				// ^8	億オク
		"\xE3\x82\xB8\xE3\x83\xA5\xE3\x83\xBC"
			"\xE3\x82\xAA\xE3\x82\xAF",			//		十億ジューオク
		"\xE3\x83\x92\xE3\x83\xA3\xE3\x82\xAF"
			"\xE3\x82\xAA\xE3\x82\xAF",			//		百億ヒャクオク
		"\xE3\x82\xBB\xE3\x83\xB3"
			"\xE3\x82\xAA\xE3\x82\xAF",			//		千億センオク
		"\xE3\x83\x81\xE3\x83\xA7\xE3\x83\xBC",	// ^12	兆チョー
			"\xE3\x82\xB8\xE3\x83\xA5\xE3\x83\xBC"
		"\xE3\x83\x81\xE3\x83\xA7\xE3\x83\xBC",	//		十兆ジュッチョー
			"\xE3\x83\x92\xE3\x83\xA3\xE3\x82\xAF"
		"\xE3\x83\x81\xE3\x83\xA7\xE3\x83\xBC",	//		百兆ヒャクチョー
			"\xE3\x82\xBB\xE3\x83\xB3"
		"\xE3\x83\x81\xE3\x83\xA7\xE3\x83\xBC",	//		千兆センチョー
		"\xE3\x82\xB1\xE3\x82\xA4",				// ^16	京ケイ
		"\xE3\x82\xB8\xE3\x83\xA5\xE3\x83\xBC"
			"\xE3\x82\xB1\xE3\x82\xA4",			//		十京ジュッケイ
		"\xE3\x83\x92\xE3\x83\xA3\xE3\x82\xAF"
			"\xE3\x82\xB1\xE3\x82\xA4",			//		百京ヒャッケイ
		"\xE3\x82\xBB\xE3\x83\xB3"
			"\xE3\x82\xB1\xE3\x82\xA4",			//		千京センケン
		"\xE3\x82\xAC\xE3\x82\xA4",				// ^20	垓ガイ
		"\xE3\x82\xB8\xE3\x83\xA5\xE3\x83\xBC"
			"\xE3\x82\xAC\xE3\x82\xA4",			//		十垓ジュッガイ
		"\xE3\x83\x92\xE3\x83\xA3\xE3\x82\xAF"
			"\xE3\x82\xAC\xE3\x82\xA4",			//		百垓ヒャクガイ
		"\xE3\x82\xBB\xE3\x83\xB3"
			"\xE3\x82\xAC\xE3\x82\xA4",			//		千垓センガイ
//		"",										// ^24	
	};
	int cbSeisuubu, cbSeisuubuCpy, cbStore;
	char *pszSrc, *pszDst, *p;

	for ( pszSrc = szNumbers, cbStore = cbSeisuubu = 0;
		( *pszSrc != '.' ) && ( *pszSrc != EOS ); cbSeisuubu++, pszSrc++ )
	{	;
	}
	*( pszDst = pszYomi ) = EOS;
	cbYomi--;			// 終端分 
	cbSeisuubuCpy = cbSeisuubu;
	for ( pszSrc = szNumbers; *pszSrc != EOS; pszSrc++ )
	{	if ( isdigit( *pszSrc ))
		{	if ( cbSeisuubu > 1 )
			{	if ( *pszSrc != '0' )
				{	if ( *pszSrc != '1' )
					{	p = szDigitYomi[*pszSrc - '0'];
						if (( cbStore += ( int )strlen( p )) >= cbYomi )
						{	break;
						}
						strcat( pszDst, p );
					}
//					if ( cbSeisuubu >= 2 )
					{	p = szKetaYomi[cbSeisuubu - 2];
						if (( cbStore += ( int )strlen( p )) >= cbYomi )
						{	break;
						}
						strcat( pszDst, p );
				}	}
				cbSeisuubu--;
			}
			else
			{	if (( *pszSrc != '0' ) || ( cbSeisuubuCpy == 1 ))	// '0'で無いか、1桁の'0'なら発音 
				{	p = szDigitYomi[*pszSrc - '0'];
					if (( cbStore += ( int )strlen( p )) >= cbYomi )
					{	break;
					}
					strcat( pszDst, szDigitYomi[*pszSrc - '0']);
		}	}	}
		else if ( *pszSrc == '.' )
		{	strcat( pszDst, "\xE3\x83\x86\xE3\x83\xB3"/*u8"テン"*/ );
		}
		else if ( *pszSrc == '-' )
		{	strcat( pszDst, "\xE3\x83\x9E\xE3\x82\xA4\xE3\x83\x8A\xE3\x82\xB9"/*u8"マイナス"*/ );
		}
	}
}


//
//	SAPI合成音声波形前後の長い無音データを省く 
//
LPSTR SkipMuonWave( int sps, int Byteps, LPSTR pSrc, DWORD *cbWave )
{	DWORD cw, cu;
	DWORD BefMuon, AftMuon;
	WORD *pw;
	DWORD *pd;
	BYTE *pb;

	if ( sapi54.iMuonMs >= 1000 )							// 1000 >= non cut
	{	return ( pSrc );
	}
//	BefMuon = sps / 5;										// 50mSec 要調整 
	AftMuon = sps / ( 1000 / sapi54.iMuonMs );				// 20 = 50mSec 要調整 
	if ( Byteps == 2 )										// 16bit 
	{	pw = (( WORD * )( pSrc + *cbWave )) - 1;			// 末尾ポインタ 
		for ( cu = cw = *cbWave / sizeof( WORD ); *pw == 0 && cw > 0; pw-- )
		{	cw--;
		}
		if (( cu - cw ) > AftMuon )							// 末尾無音が閾値を超えていたら 
		{	cw += AftMuon;
			*cbWave = cw * sizeof( WORD );					// 末尾無音を*cbwaveを減じる 
		}
/*															// 先頭無音のカット　ポインタの移動＋データ数減 
		for ( pw = ( WORD * )pSrc, cu = cw = *cbWave / sizeof( WORD ); *pw == 0 && cw > 0; pw++ )
		{	cw--;
		}
		if (( cu - cw ) > BefMuon )							// 先頭無音が閾値を超えていたら 
		{	pSrc = ( LPSTR )( pw - BefMuon );				// 先頭無音をポインタを移動する
			*cbWave -= ( cw - BefMuon ) * sizeof( WORD );	// 先頭無音を*cbwaveを減じる 
		}
*/
	}
	else if ( Byteps == 4 )									// 32bit 
	{	pd = ( DWORD * )( pSrc + *cbWave ) - 1;				// 末尾ポインタ 
		for ( cu = cw = *cbWave / sizeof( DWORD ); *pd == 0 && cw > 0; pd-- )
		{	cw--;
		}
		if (( cu - cw ) > AftMuon )							// 末尾無音が閾値を超えていたら 
		{	cw += AftMuon;
			*cbWave = cw * sizeof( DWORD );					// 末尾無音を*cbwaveを減じる 
		}
/*
		for ( pd = ( DWORD * )pSrc, cu = cw = *cbWave / sizeof( DWORD ); *pd == 0 && cw > 0; pd++ )
		{	cw--;
		}
		if (( cu - cw ) > BefMuon )							// 先頭無音が閾値を超えていたら 
		{	pSrc = ( LPSTR )( pd - BefMuon );				// 先頭無音をポインタを移動する
			*cbWave -= ( cw - BefMuon ) * sizeof( DWORD );	// 先頭無音を*cbwaveを減じる 
		}
*/
	}
	else if ( Byteps == 1 )									// 8bit 
	{	pb = ( BYTE * )( pSrc + *cbWave ) - 1;				// 末尾ポインタ 
		for ( cu = cw = *cbWave / sizeof( BYTE ); *pb == 0 && cw > 0; pb-- )
		{	cw--;
		}
		if (( cu - cw ) > AftMuon )							// 末尾無音が閾値を超えていたら 
		{	cw += AftMuon;
			*cbWave = cw * sizeof( BYTE );					// 末尾無音を*cbwaveを減じる 
		}
/*
		for ( pb = ( BYTE * )pSrc, cu = cw = *cbWave / sizeof( BYTE ); *pb == 0 && cw > 0; pb++ )
		{	cw--;
		}
		if (( cu - cw ) > BefMuon )							// 先頭無音が閾値を超えていたら 
		{	pSrc = ( LPSTR )( pb - BefMuon );				// 先頭無音をポインタを移動する
			*cbWave -= ( cw - BefMuon ) * sizeof( BYTE );	// 先頭無音を*cbwaveを減じる 
		}
*/
	}
	return ( pSrc );
}


int lineCvtAndPlaySapi( wchar_t *pmbstring, struct PLAYINGSENTENCE *ps )
{	static int lnSurface = 0;
	static char szSurface[MAX_LINE], numbers[MAX_LINE];
	char HatuonBuff[MAX_LINE];//, *p;
	char xmlBuff[MAX_LINE + 1024];	// for debug
	int i, len;
	long long int size;
	BOOL fgComma;
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
				pSarchCsv->CatUnnounStr(( wchar_t * )bufShiftJis );
//				UTF8ToShiftJis(( char * )szSurface, bufShiftJis, sizeof( bufShiftJis ));
				strcpy( bufShiftJis, szSurface );
//				printf( "node->surface:[%s]\n",  bufShiftJis );
				extractHatuonsurface( HatuonBuff, ( char * )bufShiftJis );
			}
			if ( HatuonBuff[0] == '\0' )
			{	continue;
			}
			if (( *numbers != EOS ) && ( *bufShiftJis == ',' ) &&
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
			else if (( HatuonBuff[0] == '.' ) && ( fgComma == FALSE ))
			{	fgComma = TRUE;
				strcat_s( numbers, sizeof( numbers ) - 1, HatuonBuff );
				continue;
			}
			else if ( numbers[0] != EOS )
			{	char szYomi[MAX_LINE * 2];
				num2yomi( szYomi, sizeof( szYomi ), numbers );
				strcat_s( szYomi, sizeof( szYomi ) - 1, HatuonBuff );
				strcpy_s( HatuonBuff, sizeof( HatuonBuff ) - 1, szYomi );
//				strcat_s( numbers, sizeof( numbers ) - 1, HatuonBuff );
//				strcpy_s( HatuonBuff, sizeof( HatuonBuff ) - 1, numbers );
				numbers[0] = EOS;
		}	}
//		if (( node->next )->posid != 0 ) //|| ( *pmbstring == '\0' ))
//		{
			conv2Katakana(( unsigned char * )&( bufHiraganaJis[strlen( bufHiraganaJis )]),( unsigned char * )&( HatuonBuff[0]));
//			if ( node->next != NULL )
//			{	if (( fgNxtKana = *(( unsigned char * )( node->next )->surface )) == 0xe3 )
//				{	fgNxtKana = ( *(( unsigned char * )( node->next )->surface + 1 ) << 8 ) | *(( unsigned char * )( node->next )->surface + 2 );
//					if (( 0x8181 <= fgNxtKana ) && ( fgNxtKana <= 0x83bc ))
//					{	if ( tblErrorStart[fgNxtKana - 0x8181] != 0 )			// UTF ひらがな カタカナ
//						{	continue;
//		}	}	}	}	}
		if ((( len = ( int )strlen( bufHiraganaJis )) < 48 ) && (( node->next )->posid != 0 ))
//		if (( node->next )->posid != 0 )
		{	continue;
		}
		len = ( int )strlen( bufHiraganaJis );
		if ( len <= 0 )
		{	continue;
		}
		UTF8ToUni(( char * )bufHiraganaJis, numbers, sizeof( numbers ) / sizeof( wchar_t ));
//		ShiftJisToUni( bufHiraganaJis, numbers, sizeof( numbers ) / sizeof( wchar_t ));
		while ( pVoice && !fgSkipspeak )
		{	HRESULT hr;	// = pVoice->WaitUntilDone( INFINITE );
			CSpStreamFormat OriginalFmt;
			CComPtr <ISpStream> cpWavStream;
			CComPtr <ISpStreamFormat> cpOldStream;
			try
			{
#ifndef PIPE
//			wchar_t *m_szWFileName = L"test.wav";
//			wchar_t *m_szWFileName = ( wchar_t * )NULL;		// grobal for free std::getenv("HOME")	GetTempPath	_wtempnam
			if ( m_szWFileName == ( wchar_t * )NULL )
			{	m_szWFileName = _wtempnam(( wchar_t * )NULL, L"MeCabonWav-" );
			}
#else
//			wchar_t *m_szWFileName = L"\\\\.\\pipe\\test";
			if ( m_szWFileName == ( wchar_t * )NULL )
			{	m_szWFileName = _wcsdup( L"\\\\.\\pipe\\test" );
			}
			HANDLE hPipe = INVALID_HANDLE_VALUE;

			hPipe = CreateNamedPipe( m_szWFileName, PIPE_ACCESS_INBOUND, 
                                PIPE_TYPE_BYTE | PIPE_WAIT, 1, 0, 0, 1000, NULL );
			if ( hPipe == INVALID_HANDLE_VALUE )
			{	DWORD erron = GetLastError();
				break;
			}
#endif
			// wavファイル結合すること、出力ストリームを作成	https://www.codetd.com/ja/article/6564768
			if (( hr = pVoice->GetOutputStream( &cpOldStream )) != S_OK )
			{	break;
			}
			if (( hr = OriginalFmt.AssignFormat( cpOldStream )) != S_OK )
			{	break;
			}
			if (( hr = SPBindToFile( m_szWFileName, SPFM_CREATE_ALWAYS, &cpWavStream,
						&OriginalFmt.FormatId(), OriginalFmt.WaveFormatExPtr())) != S_OK )
			{	break;		// sphelper.hを提供する機能使用してWAVファイルを作成
			}
			pVoice->SetOutput( cpWavStream, TRUE );
			pVoice->SetRate( sapi54.iRate );		// 話速 -10 - +10
			pVoice->SetVolume( sapi54.iVol );		// 音量 0 - 100
			if ( sapi54.iModeSel == 1 )				// SSML?
			{//	char xmlBuff[MAX_LINE + 1024];

//https://docs.microsoft.com/ja-jp/azure/cognitive-services/speech-service/speech-synthesis-markup?tabs=csharp
//
//	現在、 <lang xml:lang> 要素は、prosody および break 要素と互換性がありません。
//	この要素では、ピッチ、輪郭、速さ、期間、ボリュームなど、一時停止や韻律を調整することはできません。
//
//	<prosody pitch="value" contour="value" range="value" rate="value" duration="value" volume="value"></prosody>
//				wcscpy(( wchar_t * )xmlBuff, L"<speak version=\"1.0\" xml:lang=\"ja-JP\"> <prosody pitch=\"high\"> </prosody><phoneme alphabet=\"sapi\" ph=\"" );
//NG			wcscpy(( wchar_t * )xmlBuff, L"<speak version=\"1.0\" xml:lang=\"ja-JP\"> <rate speed=\"9\"> <phoneme alphabet=\"sapi\" ph=\"" );
#if 1
				wcscpy(( wchar_t * )xmlBuff, L"<speak version=\"1.0\" xml:lang=\"ja-JP\"> <phoneme alphabet=\"sapi\" ph=\"" );
//				wcscat(( wchar_t * )xmlBuff, ( wchar_t * )L"・アドルフィーネノソーダン" );
				wcscat(( wchar_t * )xmlBuff, ( wchar_t * )numbers );
				wcscat(( wchar_t * )xmlBuff, L"\"> </phoneme> </speak>" );
				hr = pVoice->Speak(( wchar_t* )xmlBuff, SPF_ASYNC, NULL );
//				hr = pVoice->Speak(( wchar_t* )xmlBuff, SPF_ASYNC | SPF_IS_XML, NULL );
#endif
//NG			hr = pVoice->Speak( L"<pron sym=\"アイウエオ\">あいうえお</pron>", SPF_ASYNC, NULL );
//NG			hr = pVoice->Speak( L"<pron sym=\"ムヘンカン\">無変換</pron>", SPF_ASYNC, NULL );
//NG			hr = pVoice->Speak( L"<p><pron sym=\"ムヘンカン\"/><p>", SPF_ASYNC, NULL );
//NG			hr = pVoice->Speak( L"<speak version= \"1.0\" xml:lang= \"ja-JP\" ><pron sym=\"ムヘンカン\"/>", SPF_ASYNC, NULL );
//NG			hr = pVoice->Speak( L"<speak version=\"1.0\" xml:lang=\"ja-JP\"><pron sym=\"ムヘンカン\"/>", SPF_ASYNC, NULL );
//https://kunsen.net/2020/10/18/post-3343/
//OK			hr = pVoice->Speak( L"<speak version=\"1.0\" xml:lang=\"ja-JP\">関数の呼び出し元に渡す値を「<phoneme alphabet=\"sapi\" ph=\"モドリチ\">戻り値</phoneme>」と呼びます。</speak>", SPF_ASYNC, NULL );
//OK			hr = pVoice->Speak( L"<speak version=\"1.0\" xml:lang=\"ja-JP\"><phoneme alphabet=\"sapi\" ph=\"ムヘンカン\"></phoneme></speak>", SPF_ASYNC, NULL );
//NG			hr = pVoice->Speak( L"<speak version=\"1.0\" xml:lang=\"ja-JP\"><phoneme alphabet=\"sapi\" ph=\"１００００\"></phoneme></speak>", SPF_ASYNC, NULL );
//				Sleep(100);
//<pron sym="h eh 1 l ow & w er 1 l d "/>
//<pron sym="h eh 1 l ow & w er 1 l d"> hello world </pron> 
//<pron sym="ムヘンカン"/>無変換<pron>
			}
			else
			{	hr = pVoice->Speak(( wchar_t* )numbers, SPF_ASYNC, NULL );
			}
//			if ( !pVoice->WaitUntilDone( INFINITE ))		// パイプ出力にすると返ってこない 
			if ( !pVoice->WaitUntilDone( 5000 ))			// 5秒で戻らなければエラー パイプ出力にすると返ってこない 
			{	
			}
			cpWavStream.Release();
//			元のストリームに再配置//出力
			pVoice->SetOutput( cpOldStream, FALSE );
			if ( hr != S_OK )		// SPERR_UNSUPPORTED_PHONEME	0x8004507A
			{	wcscat_s(( wchar_t * )bufUTF8, sizeof( bufUTF8 ) / sizeof( wchar_t ), L"\n" );
				pCorpasDlg->OnDsplayOutputDlg();
				pSarchCsv->CatUnnounStr(( wchar_t * )bufUTF8 );
				if ( sapi54.iModeSel == 1 )				// SSML?
				{	pSarchCsv->CatUnnounStr(( wchar_t * )xmlBuff );	// for debug
				}
//				pVoice->WaitUntilDone( INFINITE );		// パイプ出力にすると返ってこない 
//				cpWavStream.Release();
///				元のストリームに再配置//出力
//				pVoice->SetOutput( cpOldStream, FALSE );
				break;
			}
#ifndef PIPE
			FILE *fp;
//			if (( fp = _wfopen( m_szWFileName, L"rb" )) == NULL )
			if (( fp = _wfopen( m_szWFileName, L"rbD" )) == NULL )
			{	break;
			}
			size = _filelengthi64( _fileno( fp ));			//	ファイルサイズ取得
#else
			if ( !ConnectNamedPipe( hPipe, NULL ))
			{	CloseHandle( hPipe );
				break;
			}
			DWORD HighFileSize;
			size = GetFileSize( hPipe, &HighFileSize );
#endif
			while ( 1 )
			{	EnterCriticalSection( &cstPlayRec );
				i = ctPlayRec;
				LeaveCriticalSection( &cstPlayRec );
				if ( i < lengthof( lpWave ))
				{	break;
				}
				Sleep( 50 );
			}
//			Sleep( 50 );
			if ( bcWave[ixPlayRec] < size )
			{	bcWave[ixPlayRec] = size * 12 / 10;
				if ( bcWave[ixPlayRec] < DEFSIZE )
				{	bcWave[ixPlayRec] = DEFSIZE;
				}
				if ( lpWave[ixPlayRec] != NULL )
				{	waveOutUnprepareHeader( hWaveOut, &whdr[ixPlayRec], sizeof( WAVEHDR ));
					free( lpWave[ixPlayRec]);
				}
				if (( lpWave[ixPlayRec] = ( LPBYTE )calloc( bcWave[ixPlayRec], sizeof( char ))) == NULL )
				{	break;
				}
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
//				waveOutPrepareHeader( hWaveOut, &whdr[ixPlayRec], sizeof( WAVEHDR ));
			}
#ifndef PIPE
			fread( lpWave[ixPlayRec], sizeof( char ), size, fp );
			fclose( fp );
#else
			DWORD nBytes;
			if ( ReadFile( hPipe, lpWave[ixPlayRec], size, &nBytes, NULL ))
			{	CloseHandle( hPipe );
				hPipe = INVALID_HANDLE_VALUE;
				break;
			}
			CloseHandle( hPipe );
			hPipe = INVALID_HANDLE_VALUE;
#endif
#if 0		// テスト　出力したwavファイルをPlaySoundで再生してみる　21/09/12
			//同じだった、ASYNCでは当然発音が終わるまでに次の再生が始まった。
			PlaySound(( LPCTSTR )m_szWFileName, NULL, SND_SYNC );
//			PlaySound(( LPCTSTR )m_szWFileName, NULL, SND_ASYNC );
#else
			if ( strncmp(( const char * )lpWave[ixPlayRec], "RIFF", 4 ) != 0 )
			{	break;
			}
			if ( strncmp(( const char * )lpWave[ixPlayRec] + 8, "WAVEfmt ", 8 ) != 0 )
			{	break;
			}
			int sps = *( DWORD * )( lpWave[ixPlayRec] + 24 );		// nSamplePerSec
			int Byteps = *( WORD * )( lpWave[ixPlayRec] + 32 );		// nBlockAlign
			int ofs = *( WORD * )( lpWave[ixPlayRec] + 16 ) + 20;
			if ( strncmp(( const char * )lpWave[ixPlayRec] + ofs, "data", 4 ) != 0 )
			{	break;
			}
			if ( SelectTts != SAPI54 )
			{	waveFainalize();
			}
			if ( StateWaveOutOpen != WFSAPI )	// WaveOutOpen
			{	MMRESULT retstate;

									// 0x01:PCM
									// 0x06:G.711 A-law
									// 0x07:G.711 μ-law
				wfe.wFormatTag		= *( WORD * )( lpWave[ixPlayRec] + 20 );
				wfe.nChannels		= *( WORD * )( lpWave[ixPlayRec] + 22 );// nChannels									//モノラル
				wfe.wBitsPerSample	= *( WORD * )( lpWave[ixPlayRec] + 34 );//量子化ビット数
				wfe.nBlockAlign		= *( WORD * )( lpWave[ixPlayRec] + 32 );
				wfe.nSamplesPerSec	= sps;									//標本化周波数
				wfe.nAvgBytesPerSec	= wfe.nSamplesPerSec * wfe.nBlockAlign;
				if (( retstate = waveOutOpen( &hWaveOut, WAVE_MAPPER, &wfe, ( DWORD_PTR )StaticWaveOutProc, ( DWORD_PTR )NULL, CALLBACK_FUNCTION )) != MMSYSERR_NOERROR )
				{	return ( retstate );
				}
				StateWaveOutOpen = WFSAPI;
			}
			DWORD cbWave = *( DWORD * )( lpWave[ixPlayRec] + ofs + 4 );
			waveOutUnprepareHeader( hWaveOut, &whdr[ixPlayRec], sizeof( WAVEHDR ));
//			memset( lpWave[ixPlayRec], 0, bcWave[ixPlayRec]);
			whdr[ixPlayRec].lpData = SkipMuonWave( sps, Byteps, ( LPSTR )lpWave[ixPlayRec] + ofs + 8, &cbWave );
//			whdr[ixPlayRec].lpData = ( LPSTR )lpWave[ixPlayRec] + ofs + 8;
			whdr[ixPlayRec].dwBufferLength = ( DWORD )cbWave;
			waveOutPrepareHeader( hWaveOut, &whdr[ixPlayRec], sizeof( WAVEHDR ));
			ctCnfDone++;
			waveOutWrite( hWaveOut, &whdr[ixPlayRec], sizeof( WAVEHDR ));
			ixPlayRec = ( ixPlayRec + 1 ) % lengthof( lpWave );
			EnterCriticalSection( &cstPlayRec );
			ctPlayRec++;
			LeaveCriticalSection( &cstPlayRec );
#endif
			break;
		}	catch( ... )
			{	continue;
			}
		}
		numbers[0] = bufHiraganaJis[0] = EOS;
	}
	if ( pVoice )
	{	SPVOICESTATUS Status;
		__nullterminated WCHAR *pszLastBookmark;

		while ( 1 )
		{	HRESULT hr = pVoice->GetStatus( &Status, &pszLastBookmark );
			if (( Status.ulLastStreamQueued - Status.ulCurrentStream ) < 10 )
			{	break;
			}
			Sleep( 100 );
	}	}
	return ( 0 );
}


