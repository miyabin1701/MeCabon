

#pragma once

#include "RdPdfTkn.h"

#define	MAXPAGELENGTH	4096


struct XREF
{
	DWORD	ObjPos;
//	WORD	ver;
//	char	fgUsed;
};


struct TEXT
{	int		codePage;
	int		posX;
	int		posY;
	int		fontNo;
	int		fontSize;
	int		Unilen;
	wchar_t	*pszUniStr;
	struct TEXT	*pNext;
};


struct PageTable
{	DWORD	ObjNo;
	DWORD	ContentsNo;
	DWORD	ResourcesNo;
	struct TEXT	textTbl[64];
};


//#define	BUFFSIZE	4096


struct FontInfo
{	int		szFontId;		// /Fn
	DWORD	CodePage;		// そのフォント使用時の文字列コードセット 
};


struct FontResouce
{	struct FontResouce	*pNext;
	int		ObjNo;				// /Resources %d
	int		iCntFont;
	struct FontInfo		tblFont[1];	// iCntFont個のテーブル確保 
};


struct FontTableTmp
{	int		FontNo;				// /Resources %d
	int		FontObj;
//	DWORD	CodePage;			// そのフォント使用時の文字列コードセット 
};


class PdfReadText
{
public:
	PdfReadText(void);
	virtual ~PdfReadText(void);
//	DWORD pdfSeek( DWORD pos );
	DWORD pdfObjSeek( DWORD ObjNo );
	DWORD pdfPageSeek( DWORD pageNo );
//	char *ReadPrevHalf();
//	char *getPrevToken( char *pBuf = NULL );	// 現在のシークポイントから手前の１行を返す
//	char *ReadNextHalf();
//	char *getNextToken( char *pBuff = NULL );	// 現在のシークポイントから次の１行を返す
	HANDLE	PdfOpen( wchar_t *pszFilePath );
	void	PdfClose();
//	char	*ReadStreem( char *pBuf, DWORD len );
	int		PicupTextString( int pageNo, char *pSrc, int lenSrc, char *pDst, int lenDst, struct TEXT **ppPrintInfo );
	int		PdfPagePickupStreams( int pageNo, char *pBuf, int BufLen );
	int		PdfReadPage( DWORD pageNo, char *pBuf, DWORD BufLen );
	int		SarchPdfInfo();
	int 	GenerateXrefTable( DWORD posxref );
	int 	GeneratePageTable();
	void	AnalizePageResource();
	void	SarchRubiPageno();
	int		sarchFontCodePage( int pageNo, int fontNo );
	void	repgetForwardToken( char *ReadBuf );
	void	repgetPrevToken( char *ReadBuf );
	void	AnalysisTotal( struct TEXT **ppPrintInfo, struct TEXT *pText );

	RdPdfTkn	PdfFile;
	HANDLE	hFile;
//	BOOL	updateSeekPos;	// シーク位置更新フラッグ 
//	DWORD	seekPos;
//	DWORD	PdfSize;
	DWORD	PageCnt;
	DWORD	posxref;

	WORD	nXref;
	XREF	*pXrefTbl;		// XrefTbl
	WORD	cntXrefEnt;
	WORD	RootBlk;
	WORD	RotGene;
	WORD	InfoBlk;
	WORD	InfGene;
	WORD	fstObj;
//	char	readBuf[BUFFSIZE + 1];
//	DWORD	BufHalf;		// = sizeof( readBuf ) / 2;
//	fontlist *pFonts;
	struct PageTable	*pPageTbl;
	long	SeekPosXref;
	long	SeekPosPages;
//	char	*bp;
//	int		minmumFont;
//	int		minmumYpos;
//	int		maxmumYpos;
	int		MajorFontSize;
	int		posYmin;
	int		posXmin;
	int		posYmax;
	int		posXmax;
	int		posYminCnt;
	int		posYmaxCnt;
	int		posXminCnt;
	int		posXmaxCnt;
	int		selPagePos;
	struct FontResouce *pFontRoot;
	struct TEXT *pPrintInfo;

//	char *BufBtm;
//	char readBuf[4096 * 2];

//	#define PdfBufSize		( sizeof( readBuf ))
//	#define PdfBufHfSize	( sizeof( readBuf ) / 2 )


};

