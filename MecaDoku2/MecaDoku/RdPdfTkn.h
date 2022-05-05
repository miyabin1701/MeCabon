//
//	ReadPdfToken
//
//
//
#pragma once


class RdPdfTkn
{
public:
	RdPdfTkn(void);
	virtual ~RdPdfTkn(void);

	HANDLE	Open( wchar_t *pszFilePath );
	void	Close();
	DWORD	Seek( DWORD pos );
	char	*ReadReverseHalf();
	char	*getReverseToken( char *pBuf = NULL );	// 現在のシークポイントから手前の１行を返す
	char	*ReadForwardHalf();
	char	*getForwardToken( char *pBuff = NULL );	// 現在のシークポイントから次の１行を返す
	char	*ReadStreem( char *pBuf, DWORD len );	// 現在のシークポイントからLenバイト返す
	DWORD	Size();

protected:
	HANDLE	hFile;
	BOOL	updateSeekPos;							// シーク位置更新フラッグ 
	DWORD	PdfSize;
	DWORD	seekPos;								// シーク位置
//	DWORD	currentPos;								// トークン抽出位置 
//	DWORD	BufTopPos;								// readBuf[0] のファイルシーク位置 

	char	readBuf[4096 * 2];						// リードバッファ前後半２分割でロード 

	#define PdfBufSize			( sizeof( readBuf ))
	#define PdfBufHfSize		( sizeof( readBuf ) / 2 )
	#define PdfBufReadReverse	( sizeof( readBuf ) / 8 )
	#define PdfBufReadForward	( sizeof( readBuf ) / 8 * 7 )
	#define LtReadReverse(x)	(( x - BufTopPos ) < PdfBufReadReverse )
	#define GtReadForward(x)	( PdfBufReadForward < ( x - BufTopPos ))
	#define calcClusterTop(x)	((( x ) / PdfBufHfSize ) * PdfBufHfSize )

	#define BufAd2FPos(x)		(( x - &( readBuf[0])) + BufTopPos )
	#define FPos2BufAd(x)		&( readBuf[x - BufTopPos])
	#define BufDat(x)			( readBuf[x - BufTopPos])

public:



};

