//
#include "StdAfx.h"
#include <stdlib.h>
#include <IntSafe.h>
#include "PdfRdTxt.h"


// Public domain, May 15 2011, Rich Geldreich, richgel99@gmail.com. See "unlicense" statement at the end of tinfl.c.
#include "miniz.c"

#define EOS	'\0'

//char readBud[4096 * 2];

//#define PdfBufSize	( sizeof( readBuf ))
//#define PdfBufHfSize	( sizeof( readBuf ) / 2 )
extern int CodePageToUni( UINT codepage, char *pszCPstr, int lenCPstr, char *pszUni, int lenUni );
extern void UniToUTF8( char *pszUni, char *pszUtf8, int lenUtf8 );


PdfReadText::PdfReadText(void)
{
	pXrefTbl = NULL;
	pPageTbl = NULL;
	pFontRoot = NULL;
	pPrintInfo = NULL;

	SeekPosXref = 0;
	SeekPosPages = 0;
	selPagePos = 0;
}


void freeInfo( struct TEXT *pInfo )
{	if ( pInfo != NULL )
	{	freeInfo( pInfo->pNext );
		free( pInfo );
}	}


void freeFont( struct FontResouce *pFont )
{	if ( pFont != NULL )
	{	freeFont( pFont->pNext );
		free( pFont );
}	}


PdfReadText::~PdfReadText(void)
{
	freeInfo( pPrintInfo );
	pPrintInfo = NULL;
	freeFont( pFontRoot );
	pFontRoot = NULL;
	if ( pPageTbl != NULL )		// PageTbl
	{	free( pPageTbl );
		pPageTbl = NULL;
	}
	if ( pXrefTbl != NULL )		// XrefTbl
	{	free( pXrefTbl );
		pXrefTbl = NULL;
	}
	PdfClose();
}


DWORD PdfReadText::pdfObjSeek( DWORD ObjNo )
{	return( PdfFile.Seek( pXrefTbl[ObjNo - fstObj].ObjPos ));
}


DWORD PdfReadText::pdfPageSeek( DWORD pageNo )
{	return( PdfFile.Seek( pXrefTbl[pPageTbl[pageNo].ObjNo - fstObj].ObjPos ));
}


HANDLE PdfReadText::PdfOpen( wchar_t *pszFilePath )
{	DWORD size;

	hFile = PdfFile.Open( pszFilePath );
	if( hFile == INVALID_HANDLE_VALUE )
	{	return( hFile );
	}
	SarchPdfInfo();
	if ( GenerateXrefTable( posxref ) == 0 )
	{	PdfFile.Close();
		return( NULL );
	}
	GeneratePageTable();
	AnalizePageResource();
	posYmin = posXmin = INT_MAX;
	posYmax = posXmax = INT_MIN;
	posYminCnt = posYmaxCnt = posXminCnt = posXmaxCnt = 0;
	return ( hFile );
}


void PdfReadText::PdfClose()
{	PdfFile.Close();
}


int PdfReadText::SarchPdfInfo()
{	char *pszToken;

	PdfFile.Seek( PdfFile.Size());
	while ( 1 )
	{	pszToken = PdfFile.getReverseToken();
		if ( strstr( pszToken, "%%EOF" ) != NULL )
		{	break;
	}	}
	while ( 1 )
	{	pszToken = PdfFile.getReverseToken();
		if ( strstr( pszToken, "startxref" ) != NULL )
		{	pszToken = PdfFile.getForwardToken();			// 方向転換時は１回空読み 
			pszToken = PdfFile.getForwardToken();
			posxref = atoi( pszToken );
			break;
	}	}
	while ( 1 )
	{	pszToken = PdfFile.getReverseToken();					//>>
		if ( strncmp( pszToken, "trailer", 7 ) == 0 )
		{	pszToken = PdfFile.getForwardToken();			// 方向転換時は１回空読み 
			pszToken = PdfFile.getForwardToken();
			if ( strncmp( pszToken, "<<", 2 ) == 0 )
			{	while ( strncmp( pszToken, ">>", 2 ) != 0 )
				{	pszToken = PdfFile.getForwardToken();
					if ( strncmp( pszToken, "/Size", 5 ) == 0 )
					{	pszToken = PdfFile.getForwardToken();
						cntXrefEnt = atoi( pszToken );
					}
					else if ( strncmp( pszToken, "/Root", 5 ) == 0 )
					{	pszToken = PdfFile.getForwardToken();
						RootBlk = atoi( pszToken );
						pszToken = PdfFile.getForwardToken();
						RotGene = atoi( pszToken );
						pszToken = PdfFile.getForwardToken();
						if ( *pszToken == 'R' )
						{	// = RootBlk;
							// = RotGene;
					}	}
					else if ( strncmp( pszToken, "/Info", 5 ) == 0 )
					{	pszToken = PdfFile.getForwardToken();
						InfoBlk = atoi( pszToken );
						pszToken = PdfFile.getForwardToken();
						InfGene = atoi( pszToken );
						pszToken = PdfFile.getForwardToken();
						if ( *pszToken == 'R' )
						{	// = InfoBlk;
							// = InfGene;
				}	}	}
				break;
	}	}	}
	return ( 0 );
}


int PdfReadText::GenerateXrefTable( DWORD posxref )
{	char	*pszToken;
	int		cntObj, SeekObj, i;

	PdfFile.Seek( posxref );
	pszToken = PdfFile.getForwardToken();			// xref ?
	if ( strncmp( pszToken, "xref", 4 ) != 0 )
	{	return ( 0 );
	}
	pszToken = PdfFile.getForwardToken();			// 0 9055
	fstObj = atoi( pszToken );
	pszToken = PdfFile.getForwardToken();			// 0 9055
	cntObj = atoi( pszToken );
	if (( pXrefTbl = ( XREF * )calloc( cntObj, sizeof( XREF ))) == NULL )
	{	return ( 0 );
	}
	for ( i = 0; i < cntObj; i++ )
	{	pszToken = PdfFile.getForwardToken();
		SeekObj = atoi( pszToken );
		pszToken = PdfFile.getForwardToken();			// 
		pszToken = PdfFile.getForwardToken();			// 
		pXrefTbl[i].ObjPos = ( *pszToken == 'n' )? SeekObj: 0;
	}
	return ( cntObj );
}


int PdfReadText::GeneratePageTable()
{	char analzBuf[1024], *p, *pszToken;
	int pageNo, PagesObj, ObjNo, relNo;
	BOOL fgOneColumn;

	pdfObjSeek( RootBlk );				// /Pages n 0 R
//	getNextLine( analzBuf );			// 2 0 obj
//	getNextLine( analzBuf );			// <<
//503 0 obj
//<<
///Type /Catalog
///Pages 1 0 R
///OpenAction [3 0 R /FitH null]
///PageLayout /OneColumn
//>>
//endobj
	fgOneColumn = FALSE;
	while (( pszToken = PdfFile.getForwardToken()) != NULL )
	{	if ( strncmp( pszToken, "/Pages", 6 ) == 0 )
		{	pszToken = PdfFile.getForwardToken();
			ObjNo = atoi( pszToken );
			pszToken = PdfFile.getForwardToken();
			relNo = atoi( pszToken );
			pszToken = PdfFile.getForwardToken();
			if ( *pszToken == 'R' )
			{	PagesObj = ObjNo;
		}	}
		else if ( strncmp( pszToken, "/PageLayout", 11 ) == 0 )
		{	pszToken = PdfFile.getForwardToken();
			fgOneColumn = !strncmp( pszToken, "/OneColumn", 10 );
		}
//		else if ( strncmp( pszToken, "/OpenAction", 11 ) == 0 )
//		{	pszToken = PdfFile.getForwardToken();
//			fgOneColumn = strncmp( pszToken, "/OneColumn", 10 );
///OpenAction [3 0 R /FitH null]
//3 0 obj
//<</Type /Page
///Parent 1 0 R
///Resources 2 0 R
///Contents 4 0 R>>
//endobj
//		}
		else if ( strncmp( pszToken, "endobj", 6 ) == 0 )
		{	break;
	}	}
	pdfObjSeek( PagesObj );				// /Pages 1 0 R
//	pdfObjSeek( PagesObj + 1 );			// /Pages 3+1 0 R
//	getPrevLine( analzBuf );			// endobj
//	getPrevLine( analzBuf );			// >>
//1 0 obj
//<</Type /Pages
///Kids [3 0 R 5 0 R 7 0 R 9 0 R 11 0 R 13 0 R 15 0 R 17 0 R 19 0 R 21 0 R 23 0 R 25 0 R 27 0 R 29 0 R 31 0 R 33 0 R 35 0 R 37 0 R 39 0 R 41 0 R 43 0 R 45 0 R 47 0 R 49 0 R 51 0 R 53 0 R 55 0 R 57 0 R 59 0 R 61 0 R 63 0 R 65 0 R 67 0 R 69 0 R 71 0 R 73 0 R 75 0 R 77 0 R 79 0 R 81 0 R 83 0 R 85 0 R 87 0 R 89 0 R 91 0 R 93 0 R 95 0 R 97 0 R 99 0 R 101 0 R 103 0 R 105 0 R 107 0 R 109 0 R 111 0 R 113 0 R 115 0 R 117 0 R 119 0 R 121 0 R 123 0 R 125 0 R 127 0 R 129 0 R 131 0 R 133 0 R 135 0 R 137 0 R 139 0 R 141 0 R 143 0 R 145 0 R 147 0 R 149 0 R 151 0 R 153 0 R 155 0 R 157 0 R 159 0 R 161 0 R 163 0 R 165 0 R 167 0 R 169 0 R 171 0 R 173 0 R 175 0 R 177 0 R 179 0 R 181 0 R 183 0 R 185 0 R 187 0 R 189 0 R 191 0 R 193 0 R 195 0 R 197 0 R 199 0 R 201 0 R 203 0 R 205 0 R 207 0 R 209 0 R 211 0 R 213 0 R 215 0 R 217 0 R 219 0 R 221 0 R 223 0 R 225 0 R 227 0 R 229 0 R 231 0 R 233 0 R 235 0 R 237 0 R 239 0 R 241 0 R 243 0 R 245 0 R 247 0 R 249 0 R 251 0 R 253 0 R 255 0 R 257 0 R 259 0 R 261 0 R 263 0 R 265 0 R 267 0 R 269 0 R 271 0 R 273 0 R 275 0 R 277 0 R 279 0 R 281 0 R 283 0 R 285 0 R 287 0 R 289 0 R 291 0 R 293 0 R 295 0 R 297 0 R 299 0 R 301 0 R 303 0 R 305 0 R 307 0 R 309 0 R 311 0 R 313 0 R 315 0 R 317 0 R 319 0 R 321 0 R 323 0 R 325 0 R 327 0 R 329 0 R 331 0 R 333 0 R 335 0 R 337 0 R 339 0 R 341 0 R 343 0 R 345 0 R 347 0 R 349 0 R 351 0 R 353 0 R 355 0 R 357 0 R 359 0 R 361 0 R 363 0 R 365 0 R 367 0 R 369 0 R 371 0 R 373 0 R 375 0 R 377 0 R 379 0 R 381 0 R 383 0 R 385 0 R 387 0 R 389 0 R 391 0 R 393 0 R 395 0 R 397 0 R 399 0 R 401 0 R 403 0 R 405 0 R 407 0 R 409 0 R 411 0 R 413 0 R 415 0 R 417 0 R 419 0 R 421 0 R 423 0 R 425 0 R 427 0 R 429 0 R 431 0 R 433 0 R 435 0 R 437 0 R 439 0 R 441 0 R 443 0 R 445 0 R 447 0 R 449 0 R 451 0 R 453 0 R 455 0 R 457 0 R 459 0 R 461 0 R 463 0 R 465 0 R 467 0 R 469 0 R 471 0 R 473 0 R 475 0 R ]
///Count 237
///MediaBox [0 0 841.89 595.28]
//>>
//endobj
	for ( PageCnt = 0; ( pszToken = PdfFile.getForwardToken()) != NULL; )
	{	if ( strncmp( pszToken, "/Count", 6 ) == 0 )	// /Count 4512
		{	pszToken = PdfFile.getForwardToken();
			PageCnt = atoi( pszToken );
			break;
		}
		else if ( strncmp( analzBuf, "endobj", 6 ) == 0 )
		{	break;
	}	}
	if (( pPageTbl = ( struct PageTable * )calloc( PageCnt, sizeof( struct PageTable ))) == NULL )
	{	return( 0 );
	}
	memset( pPageTbl, 0, PageCnt * sizeof( struct PageTable ));
	pdfObjSeek( PagesObj );				// /Pages 3 0 R
//	PdfFile.getForwardToken( analzBuf );			// 3 0 obj
//	PdfFile.getForwardToken( analzBuf );			// <<
	for ( pageNo = 0; ( pszToken = PdfFile.getForwardToken()) != NULL; )
	{	if ( strncmp( pszToken, "/Kids", 5 ) != 0 )
		{	continue;
		}
		pszToken = PdfFile.getForwardToken();
		if ( *pszToken == '[' )
		{	pPageTbl[pageNo++].ObjNo = atoi( ++pszToken );	// 3
			while ( 1 )
			{	pszToken = PdfFile.getForwardToken();					// 0
				pszToken = PdfFile.getForwardToken();					// R
				pszToken = PdfFile.getForwardToken();					// 5
				if ( *pszToken == ']' )
				{	break;
				}
				pPageTbl[pageNo++].ObjNo = atoi( pszToken );
			}
			break;
		}
		else if ( strncmp( analzBuf, "endobj", 6 ) == 0 )
		{	break;
	}	}
	return ( PageCnt );
}


void PdfReadText::AnalizePageResource()
{	char analzBuf[1024], *p, *pszToken;
	struct FontTableTmp fntTbl[32];
	struct FontResouce **ppFonts;
	int		i, j;
	int		fontNo, fontObj;
	WORD	pageNo;

//3 0 obj
//<</Type /Page
///Parent 1 0 R
///Resources 2 0 R
///Contents 4 0 R>>
//endobj
	for ( pageNo = 0; pageNo < PageCnt; pageNo++ )
	{	pdfObjSeek( pPageTbl[pageNo].ObjNo );
		pPageTbl[pageNo].ContentsNo = pPageTbl[pageNo].ResourcesNo = 0;
		for ( ; ; )
		{	pszToken = PdfFile.getForwardToken();
			if ( strncmp( pszToken, "/Contents", 9 ) == 0 )
			{	pszToken = PdfFile.getForwardToken();					// 0
				pPageTbl[pageNo].ContentsNo = atoi( pszToken );
				pszToken = PdfFile.getForwardToken();					// R
				pszToken = PdfFile.getForwardToken();					// 5
			}
			else if ( strncmp( pszToken, "/Resources", 10 ) == 0 )
			{	pszToken = PdfFile.getForwardToken();					// 0
				pPageTbl[pageNo].ResourcesNo = atoi( pszToken );
				pszToken = PdfFile.getForwardToken();					// R
				pszToken = PdfFile.getForwardToken();					// 5
			}
			else if ( strncmp( pszToken, "endobj", 6 ) == 0 )
			{	break;
	}	}	}
	for ( pageNo = 0; pageNo < PageCnt; pageNo++ )
	{	if ( pPageTbl[pageNo].ResourcesNo == 0 )
		{	continue;
		}
		for ( ppFonts = &pFontRoot; *ppFonts != NULL; ppFonts = &(( *ppFonts )->pNext ))
		{	if (( *ppFonts )->ObjNo == pPageTbl[pageNo].ResourcesNo )
			{	break;						// 既出 
		}	}
		if ( *ppFonts != NULL )				// 既出 
		{	continue;
		}
//2 0 obj
//<<
///ProcSet [/PDF /Text /ImageB /ImageC /ImageI]
///Font <<
///F1 477 0 R
///F2 480 0 R
///F3 483 0 R
///F4 486 0 R
///F5 489 0 R
///F6 492 0 R
///F7 495 0 R
///F8 498 0 R
///F9 501 0 R
//>>
///XObject <<
//>>
//>>
//endobj
		pdfObjSeek( pPageTbl[pageNo].ResourcesNo );
		for ( i = 0; ; )
		{	pszToken = PdfFile.getForwardToken();
			if ( strncmp( pszToken, "endobj", 6 ) == 0 )
			{	break;
			}
			else if ( strncmp( pszToken, "/Font", 5 ) == 0 )
			{	memset( fntTbl, 0, sizeof( fntTbl ));
				for ( ; i < lengthof( fntTbl ); )
				{	pszToken = PdfFile.getForwardToken();
					if ( strncmp( pszToken, "/F", 2 ) == 0 )
					{	fntTbl[i].FontNo = atoi( pszToken + 2 );
						pszToken = PdfFile.getForwardToken();
						fntTbl[i++].FontObj = atoi( pszToken );
						pszToken = PdfFile.getForwardToken();
						pszToken = PdfFile.getForwardToken();
					}
					else if ( strncmp( pszToken, ">>", 2 ) == 0 )
					{	break;
				}	}
				break;
		}	}
		if ( i <= 0 )
		{	continue;
		}
//struct FontResouce
//{	struct FontResouce	*pNext;
//	int		ObjNo;				// /Resources %d
//	int		iCntFont;
//	struct FontInfo		tblFont[1];	// iCntFont個のテーブル確保 
//};
		if (( *ppFonts = ( struct FontResouce * )
			malloc( sizeof( FontResouce ) + ( sizeof( FontInfo ) * ( i - 1 )))) == NULL )
		{	return;
		}
		( *ppFonts )->ObjNo = pPageTbl[pageNo].ResourcesNo;
		( *ppFonts )->iCntFont = i;
		( *ppFonts )->pNext = NULL;
		for ( j = 0; j < i; j++ )
		{	pdfObjSeek( fntTbl[j].FontObj );
			( *ppFonts )->tblFont[j].szFontId = fntTbl[j].FontNo;
			for ( ; ; )
			{	pszToken = PdfFile.getForwardToken();
				if ( strncmp( pszToken, "/Encoding", 9 ) == 0 )
				{	pszToken = PdfFile.getForwardToken();
					if ( strncmp( pszToken, "/UniJIS-UTF16-H", 15 ) == 0 )
					{	( *ppFonts )->tblFont[j].CodePage = 1201;	// UTF-16BE
					}
					else if ( strncmp( pszToken, "/UniJIS-UTF16-V", 15 ) == 0 )
					{	( *ppFonts )->tblFont[j].CodePage = 1201;	// UTF-16BE
					}
					else if ( strncmp( pszToken, "/WinAnsiEncoding", 16 ) == 0 )
					{	( *ppFonts )->tblFont[j].CodePage = 65001;	// UTF-8
					}
					else if ( strncmp( pszToken, "90ms?RKSJ?H", 16 ) == 0 )
					{	( *ppFonts )->tblFont[j].CodePage = 932;	// SHIFTJIS
					}
					else if ( strncmp( pszToken, "90ms?RKSJ?V", 16 ) == 0 )
					{	( *ppFonts )->tblFont[j].CodePage = 932;	// SHIFTJIS
					}
					else
					{	( *ppFonts )->tblFont[j].CodePage = 65001;	// UTF-8 default
					}	// 20932	EUC-JP
						// 51932	euc-jp
						// 50220	iso-2022-jp
						// 10001	x-mac-japanese
						// 20290	IBM290
						// 65000	utf-7
					break;
				}
				else if ( strncmp( pszToken, "endobj", 6 ) == 0 )
				{	break;
				}
				else if ( strncmp( pszToken, ">>", 2 ) == 0 )
				{	break;
	}	}	}	}
}
/*
BT				テキストオブジェクトの開始
名前 サイズ Tf	フォントを指定	F13（フォント定義参照）という名前の付いたフォントを使って１２ポイントの大きさで表示します。
x座標 y座標 Td	開始位置を指定	左から４インチ(4x72=288)、下から１０インチ(10x72=720)を開始位置に指定します。
テキスト Tj		テキストを表示	テキスト文字列 「ABC」 を表示します。
ET				テキストオブジェクトの終了
*/


int PdfReadText::PdfPagePickupStreams( int pageNo, char *pBuf, int BufLen )
{	char analzBuf[MAXPAGELENGTH], *pszToken;
	mz_ulong	dstLen;
	BOOL fgFlateDecode = FALSE;
	int len = 0;
	int	status;

	pdfObjSeek( pPageTbl[pageNo].ContentsNo );
	for ( ; ; )
	{	pszToken = PdfFile.getForwardToken();
		if ( strstr( pszToken, "/Filter" ) != NULL )
		{	pszToken = PdfFile.getForwardToken();
			if ( strstr( pszToken, "/FlateDecode" ) != NULL )
			{	fgFlateDecode = TRUE;
		}	}
		else if ( strncmp( pszToken, "/Length", 7 ) == 0 )
		{	pszToken = PdfFile.getForwardToken();
			len = atoi( pszToken );
		}
		else if ( strncmp( pszToken, "stream", 6 ) == 0 )
		{	break;
	}	}
	if (( len > BufLen ) || ( fgFlateDecode != TRUE ))
	{	return ( NULL );
	}
	memset( analzBuf, 0, sizeof( analzBuf ));
	PdfFile.ReadStreem( analzBuf, len );
	dstLen = BufLen;
    // Decompress.
	mz_uncompress(( unsigned char * )pBuf, &dstLen, ( const unsigned char * )analzBuf, len );
	return (( dstLen <= 0 )? 0: dstLen );
}


int PdfReadText::sarchFontCodePage( int pageNo, int fontNo )
{	struct FontResouce **ppFonts;

	if ( pPageTbl[pageNo].ResourcesNo != 0 )
	{	for ( ppFonts = &pFontRoot; *ppFonts != NULL; ppFonts = &(( *ppFonts )->pNext ))
		{	if (( *ppFonts )->ObjNo == pPageTbl[pageNo].ResourcesNo )
			{	return (( *ppFonts )->tblFont[fontNo - 1].CodePage );
	}	}	}
	return ( -1 );
}


int delEscpChar( char *pSrc, int lenSrc )
{	char *pDst;
	int lenDst;

	for ( pDst = pSrc, lenDst = lenSrc; lenSrc > 0; lenSrc-- )
	{	if ( *pSrc == '\\' )
		{	pSrc++;
			lenDst--;
			if ( *pSrc == '\x72' )		// 300D  '」'が化ける対策　305C72　と入っているので 
			{	*pSrc = '\x0d';			// 3072  'ひ'になってた 
		}	}
		*pDst++ = *pSrc++;
	}
	return ( lenDst );
}


void PdfReadText::AnalysisTotal( struct TEXT **ppPrintInfo, struct TEXT *pText )
{	struct TEXT **ppInfo = ppPrintInfo;

#if 1
	if ( pText->posY < posYmin )
	{	posYmin = pText->posY;
		posYminCnt = 1;
	}
	else if ( pText->posY == posYmin )
	{	posYminCnt++;
	}
	if ( pText->posY > posYmax )
	{	posYmax = pText->posY;
		posYmaxCnt = 1;
	}
	else if ( pText->posY == posYmax )
	{	posYmaxCnt++;
	}
	if ( pText->posX < posXmin )
	{	posXmin = pText->posX;
		posXminCnt = 1;
	}
	else if ( pText->posX == posXmin )
	{	posXminCnt++;
	}
	if ( pText->posX > posXmax )
	{	posXmax = pText->posX;
		posXmaxCnt = 1;
	}
	else if ( pText->posX == posXmax )
	{	posXmaxCnt++;
	}
	for ( ; *ppInfo != NULL; ppInfo = &(( *ppInfo )->pNext ))
	{	if ((( *ppInfo )->fontNo == pText->fontNo ) && (( *ppInfo )->fontSize == pText->fontSize ))
		{	( *ppInfo )->Unilen++;				// 出現頻度カウント 
			return;
	}	}
	if ( *ppInfo == NULL )
	{	if (( *ppInfo = ( struct TEXT * )malloc( sizeof( struct TEXT ))) != NULL )
		{	memset( *ppInfo, 0, sizeof( struct TEXT ));
			( *ppInfo )->fontNo = pText->fontNo;
			( *ppInfo )->fontSize = pText->fontSize;
			( *ppInfo )->Unilen = 1;			// 出現頻度カウント 
	}	}
#endif
}


int PdfReadText::PicupTextString( int pageNo, char *pSrc, int lenSrc, char *pDst, int lenDst, struct TEXT **ppPrintInfo )
{	char *ps, *pstringStart;
	wchar_t szUniStr[MAXPAGELENGTH];
	int	lenS, fontNo, codePage, len, txtNo, no, fontSize, fontSizep, posX, posY, posXp, posYp;

/*
BT /F2 20.00 Tf ET
BT 290.86 348.67 Td (0ﾏ0・ﾍ0・・・n原eﾏ肪N膿tu) Tj ET
BT /F1 12.00 Tf ET
BT 390.97 240.95 Td (劔g0"	char *
*/
	for ( txtNo = 0, ps = pSrc, lenS = lenSrc; ( lenSrc > 0 ) && ( txtNo < lengthof( pPageTbl[0].textTbl )); )
	{	if (( *ps != 'B' ) || ( *( ps + 1 ) != 'T' ) || ( isspace( *( ps + 2 )) == 0 ))
		{	ps++;
			lenSrc--;
			continue;
		}
		for ( ps += 3, lenSrc -= 3; lenSrc > 0; )
		{	if ( isspace( *ps ) != 0 )
			{	sscanf( ps, "/F%d", &fontNo );
				ps++;
				lenSrc--;
				continue;
			}
			else if (( *ps == 'E' ) && ( *( ps + 1 ) == 'T' ) && ( isspace( *( ps + 2 )) != 0 ))
			{	ps += 3;
				lenSrc -= 3;
				break;
			}
			else if (( *ps == '/' ) && ( *( ps + 1 ) == 'F' ))
			{	sscanf( ps, "/F%d %d.%d", &fontNo, &fontSize, &fontSizep );		// /F2 20.00 Tf
				for ( ps += 4, lenSrc -= 4;!isspace( *ps ); ps++, lenSrc-- )
				{	;
				}
				continue;
			}
			else if ( isdigit( *ps ) != 0 )			// 290.86 348.67 Td
			{	sscanf( ps, "%d.%d %d.%d", &posX, &posXp, &posY, &posYp );
				for ( ; lenSrc > 0; ps++, lenSrc-- )
				{	if (( *ps == 'T' ) && ( *( ps + 1 ) == 'd' ))
					{	ps += 2;
						lenSrc -= 2;
						break;
			}	}	}
			else if ( *ps == '(' )					// (0ﾏ0・ﾍ0・・・n原eﾏ肪N膿tu) Tj
			{	pPageTbl[pageNo].textTbl[txtNo].codePage = codePage = sarchFontCodePage( pageNo, fontNo );
				pPageTbl[pageNo].textTbl[txtNo].posX = posX * 100 + posXp;
				pPageTbl[pageNo].textTbl[txtNo].posY = posY * 100 + posYp;
				pPageTbl[pageNo].textTbl[txtNo].fontNo = fontNo;
				pPageTbl[pageNo].textTbl[txtNo].fontSize = fontSize * 100 + fontSizep;
//				if ( fontSize < minFontSize )	minFontSize = fontSize;
				pstringStart = ++ps;
				for ( len = 0; lenSrc > 0; )
				{	if (( *ps != ')' ) || ( *( ps + 1 ) != ' ' ) ||
						( *( ps + 2 ) != 'T' ) || ( *( ps + 3 ) != 'j' ))
					{	ps++;
						len++;			// char count
						lenSrc--;
						continue;
					}
//					stringEnd = ps;
					if ( codePage == 65001 )
					{	*ps = EOS;
					}
					else
					{	*(( wchar_t	* )ps ) = EOS;
					}
					len = delEscpChar( pstringStart, len );
					pPageTbl[pageNo].textTbl[txtNo].Unilen = len
							= CodePageToUni( codePage, pstringStart, len, ( char * )szUniStr, sizeof( szUniStr ) / sizeof( WCHAR ));
					if ( len > 0 )
					{	if ( ppPrintInfo == NULL )
						{	if (( pPageTbl[pageNo].textTbl[txtNo].pszUniStr = ( wchar_t	* )calloc( len + 1, sizeof( wchar_t ))) == NULL )
							{	;
							}
							memcpy(( char * )pPageTbl[pageNo].textTbl[txtNo].pszUniStr, szUniStr, ( len * sizeof( wchar_t )) + 1 );
						}
						else	// if ( ppPrintInfo != NULL )
						{	AnalysisTotal( ppPrintInfo, &( pPageTbl[pageNo].textTbl[txtNo]));
						}
//						*ps = ')';
						ps += 4;
						lenSrc -= 4;
//						-----------------------------------< debug code
#if 0
						if ( ppPrintInfo == NULL )
						{	UniToUTF8(( char * )pPageTbl[pageNo].textTbl[txtNo].pszUniStr, ( char * )szUniStr, MAXPAGELENGTH );
							fprintf( stderr,  "%5d %2d:(%7.2lf,%7.2lf) %d %6.2lf %6d %5d (%s) \n", pageNo, txtNo,
								pPageTbl[pageNo].textTbl[txtNo].posX / 100.0,
								pPageTbl[pageNo].textTbl[txtNo].posY / 100.0,
								pPageTbl[pageNo].textTbl[txtNo].fontNo,
								pPageTbl[pageNo].textTbl[txtNo].fontSize/ 100.0,
								pPageTbl[pageNo].textTbl[txtNo].codePage,
								pPageTbl[pageNo].textTbl[txtNo].Unilen,
									szUniStr );
//							pPageTbl[pageNo].textTbl[txtNo].pszUniStr );
						}
#endif
//						-----------------------------------< debug code
						txtNo++;
					}
					break;
			}	}
			else
			{	ps++;
				lenSrc--;
			}
			if ( txtNo >= lengthof( pPageTbl[0].textTbl ))
			{	break;
	}	}	}
#if 1
	if ( ppPrintInfo == NULL )
	{	*(( wchar_t * )pDst ) = EOS;
		len = 1;						// sz文字列終端分を先に 
		for ( no = 0; no < txtNo; no++ )
		{	if ( MajorFontSize == pPageTbl[pageNo].textTbl[no].fontSize )
			{	continue;
			}
//			if ( no != 0 )
//			{	if (( pPageTbl[pageNo].textTbl[no - 1].posX <= pPageTbl[pageNo].textTbl[no].posX ) &&
//					( pPageTbl[pageNo].textTbl[no - 1].fontNo == pPageTbl[pageNo].textTbl[no].fontNo ))
//				{	continue;			// ルビ？をスキップ 
//			}	}
			if ( selPagePos == 1 )
			{	if ( pPageTbl[pageNo].textTbl[no].posY <= posYmin )
				{	continue;			// ページ番号をスキップ（仮） posYmin,posYmax…どれがページ番号位置か判断必要 
			}	}
			else if ( selPagePos == 2 )
			{	if ( pPageTbl[pageNo].textTbl[no].posY >= posYmax )
				{	continue;			// ページ番号をスキップ（仮） posYmin,posYmax…どれがページ番号位置か判断必要 
			}	}
			else if ( selPagePos == 3 )
			{	if ( pPageTbl[pageNo].textTbl[no].posY <= posXmin )
				{	continue;			// ページ番号をスキップ（仮） posYmin,posYmax…どれがページ番号位置か判断必要 
			}	}
			else if ( selPagePos == 4 )
			{	if ( pPageTbl[pageNo].textTbl[no].posY >= posXmax )
				{	continue;			// ページ番号をスキップ（仮） posYmin,posYmax…どれがページ番号位置か判断必要 
			}	}
			if (( len + pPageTbl[pageNo].textTbl[no].Unilen + 2 ) >= lenDst )
			{	break;
			}
			wcscat(( wchar_t * )pDst, pPageTbl[pageNo].textTbl[no].pszUniStr );
			wcscat(( wchar_t * )pDst, L"\n" );
			len += pPageTbl[pageNo].textTbl[no].Unilen + 2;
#if 0
			UniToUTF8(( char * )pPageTbl[pageNo].textTbl[no].pszUniStr, ( char * )szUniStr, MAXPAGELENGTH );
			printf( "%5d %2d:(%7.2lf,%7.2lf) %d %6.2lf %6d %5d (%s) \n", pageNo, no,
				pPageTbl[pageNo].textTbl[no].posX / 100.0,
				pPageTbl[pageNo].textTbl[no].posY / 100.0,
				pPageTbl[pageNo].textTbl[no].fontNo,
				pPageTbl[pageNo].textTbl[no].fontSize / 100.0,
				pPageTbl[pageNo].textTbl[no].codePage,
				pPageTbl[pageNo].textTbl[no].Unilen,
				szUniStr );
//				pPageTbl[pageNo].textTbl[no].pszUniStr );
#endif
		}
		for ( no = txtNo - 1; no >= 0; no-- )
		{	if ( pPageTbl[pageNo].textTbl[no].pszUniStr != NULL )
			{	free( pPageTbl[pageNo].textTbl[no].pszUniStr );
				pPageTbl[pageNo].textTbl[no].pszUniStr = NULL;
	}	}	}
#endif
	return ( len );
}


struct TEXT *sarchMinmumSize( struct TEXT *pInfo )
{	struct TEXT *pMajor;

	if ( pInfo->pNext != NULL )						// 最初一番使われているフォントを検索しようと 
	{	pMajor = sarchMinmumSize( pInfo->pNext );	// したが、最小フォントの方がいいかと・・・ 
		return (( pMajor->fontSize < pInfo->fontSize )? pMajor: pInfo );
	}
	return ( pInfo );
}


int PdfReadText::PdfReadPage( DWORD pageNo, char *pBuf, DWORD BufLen )
{	char analzBuf[MAXPAGELENGTH];	//, stringBuf[MAXPAGELENGTH];
	int		page;
	int		perYmin;	// 10300 - 9000
	int		perYmax;	// 10300 - 9000
	int		perXmin;	// 10300 - 9000
	int		perXmax;	// 10300 - 9000
	DWORD len = 0;
	DWORD Ix = 0;

#if 1
	if (( pageNo == 0 ) && ( pPrintInfo == NULL ))	// PAGE印刷位置、フォントサイズなどを解析 
	{	for ( page = 0; page < PageCnt; page++ )
		{	if (( len = PdfPagePickupStreams( page, analzBuf, sizeof( analzBuf ))) > 0 )
			{
#if 0			// hex dump
				for ( Ix = 0; Ix < len; Ix++ )
				{	if ( analzBuf[Ix] != '(' )
					{	putchar( analzBuf[Ix]);
						continue;
					}
					putchar( analzBuf[Ix++]);		// '('
					for( ; Ix < len; Ix++ )
					{	if ( strncmp( &( analzBuf[Ix]), ") Tj", 4 ) == 0 )
						{	putchar( analzBuf[Ix]);
							break;
						}
						printf( "%02x ", ( unsigned char )analzBuf[Ix]);
				}	}
				putchar( '\n' );
#endif
				PicupTextString( pageNo, analzBuf, len, pBuf, BufLen, &pPrintInfo );
		}	}
		perYmin = posYminCnt * 100 / PageCnt - 650;	// 10300 - 9000
		perYmin = abs( perYmin );
		perYmax = posYmaxCnt * 100 / PageCnt - 650;	// 10300 - 9000
		perYmax = abs( perYmax );
		perXmin = posXminCnt * 100 / PageCnt - 650;	// 10300 - 9000
		perXmin = abs( perXmin );
		perXmax = posXmaxCnt * 100 / PageCnt - 650;	// 10300 - 9000
		perXmax = abs( perXmax );
		MajorFontSize = sarchMinmumSize( pPrintInfo )->fontSize;
		if (( perYmin < perYmax ) && ( perYmin < perXmin ) && ( perYmin < perXmax ))
		{	selPagePos = 1;
		}
		else if (( perYmax < perYmin ) && ( perYmax < perXmin ) && ( perYmax < perXmax ))
		{	selPagePos = 2;
		}
		else if (( perXmin < perYmin ) && ( perXmin < perYmax ) && ( perXmin < perXmax ))
		{	selPagePos = 3;
		}
		else if (( perXmax < perYmin ) && ( perXmax < perYmax ) && ( perXmax < perXmin ))
		{	selPagePos = 4;
		}
		else
		{	selPagePos = 0;
	}	}
#endif
	if (( len = PdfPagePickupStreams( pageNo, analzBuf, sizeof( analzBuf ))) > 0 )
	{	len = PicupTextString( pageNo, analzBuf, len, pBuf, BufLen, NULL );
//		UniToUTF8(( char * )pBuf, ( char * )analzBuf, MAXPAGELENGTH );
//		printf( "[%s] \n", analzBuf );
	}
	return ( len );
}

/*
891 0 obj
<<
/Type /Font
/Subtype /Type0
/BaseFont /MS-Mincho,BoldItalic-UniJIS-UTF16-H
/Encoding /UniJIS-UTF16-H			<---- UTF-16BE
/DescendantFonts [900 0 R]
>>
endobj
892 0 obj
<<
/Type /Font
/Subtype /Type0
/BaseFont /MS-Mincho-UniJIS-UTF16-V
/Encoding /UniJIS-UTF16-V			<---- UTF-16BE
/DescendantFonts [901 0 R]
>>
endobj
*/

#if 0


extern DWORD	currentPos;								// トークン抽出位置 
extern DWORD	BufTopPos;								// readBuf[0] のファイルシーク位置 


void PdfReadText::repgetForwardToken( char *ReadBuf )
{	int repCnt;
	DWORD	BufTopPosSv;								// readBuf[0] のファイルシーク位置 

	pdfObjSeek( 1 );				// /Pages 3 0 R
	PdfFile.getForwardToken();
	for ( repCnt = 0; repCnt < 5; repCnt++ )
	{
		BufTopPosSv = BufTopPos;
		while ( BufTopPosSv == BufTopPos )
		{	if ( PdfFile.getForwardToken() == NULL )
			{	return;
			}
		}
		printf( "BufTop:%8d Current:%8d\n", BufTopPos, currentPos );
	}
}


void PdfReadText::repgetPrevToken( char *ReadBuf )
{	int repCnt;
	DWORD	BufTopPosSv;								// readBuf[0] のファイルシーク位置 

	pdfObjSeek( 105 );				// /Pages 3 0 R
	PdfFile.getReverseToken();
	for ( repCnt = 0; repCnt < 5; repCnt++ )
	{
		BufTopPosSv = BufTopPos;
		while ( BufTopPosSv == BufTopPos )
		{	if ( PdfFile.getReverseToken() == NULL )
			{	return;
			}
		}
		printf( "BufTop:%8d Current:%8d\n", BufTopPos, currentPos );
	}
}

#endif


