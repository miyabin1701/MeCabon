//
//	ReadPdfToken
//
//
//

#include "StdAfx.h"
#include "RdPdfTkn.h"

DWORD	currentPos;								// トークン抽出位置 
DWORD	BufTopPos;								// readBuf[0] のファイルシーク位置 


RdPdfTkn::RdPdfTkn(void)
{	hFile = INVALID_HANDLE_VALUE;
}


RdPdfTkn::~RdPdfTkn(void)
{	Close();
}


DWORD RdPdfTkn::Seek( DWORD pos )
{	updateSeekPos = TRUE;			// シーク位置更新フラッグ 
	return ( seekPos = pos );
}


HANDLE RdPdfTkn::Open( wchar_t *pszFilePath )
{	DWORD size, posxref;

	hFile = ::CreateFile( pszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
	{	return( hFile );
	}
	PdfSize = GetFileSize( hFile, &size );			//	ファイルサイズ取得	から　
	if (( PdfSize <= 0 ) || ( size > 0 ))			// size high word != 0 error
	{	::CloseHandle( hFile );
		return( INVALID_HANDLE_VALUE );
	}
	return ( hFile );
}


void RdPdfTkn::Close()
{	if ( hFile != INVALID_HANDLE_VALUE )
	{	::CloseHandle( hFile );
		hFile = INVALID_HANDLE_VALUE;
	}
}


char *RdPdfTkn::ReadReverseHalf()
{	DWORD nRead;

	BufTopPos = calcClusterTop( BufTopPos - PdfBufHfSize );
	memcpy( &readBuf[PdfBufHfSize], &readBuf[0], PdfBufHfSize );
//	printf( "Read  :%8d readBuf[0] readBuf[0]->readBuf[PdfBufHfSize] size:%8d\n", BufTopPos, PdfSize );
	SetFilePointer( hFile, BufTopPos, NULL, FILE_BEGIN );
	if ( ReadFile( hFile, &readBuf[0], PdfBufHfSize, &nRead, NULL ) != TRUE )
	{	return( NULL );
	}
	seekPos = BufTopPos;
	return ( &readBuf[nRead - 1]);
}


char *RdPdfTkn::ReadForwardHalf()
{	DWORD nRead, pos;

	pos = calcClusterTop( BufTopPos + PdfBufHfSize + PdfBufHfSize );
	if ( pos < PdfSize )
	{	memcpy( &readBuf[0], &readBuf[PdfBufHfSize], PdfBufHfSize );
//		printf( "Read  :%8d readBuf[PdfBufHfSize] readBuf[PdfBufHfSize]->readBuf[0] size:%8d\n", pos, PdfSize );
		SetFilePointer( hFile, pos, NULL, FILE_BEGIN );
		if ( ReadFile( hFile, &readBuf[PdfBufHfSize], PdfBufHfSize, &nRead, NULL ) != TRUE )
		{	return( NULL );
		}
		BufTopPos = ( seekPos = pos ) - PdfBufHfSize;
	}
	return ( &readBuf[0]);
}


char *RdPdfTkn::getReverseToken( char *pBuf )	// 現在のシークポイントから手前の１語を返す
{	DWORD nRead, pos, len;
	char *p, ch;

	if ( updateSeekPos )						// この読み込み時に前の行を参照するのでシーク位置を中央としたデータをロードする必要がある。
	{	pos = calcClusterTop( seekPos );
		if ( pos != BufTopPos )
		{	SetFilePointer( hFile, pos, NULL, FILE_BEGIN );
			if ( ReadFile( hFile, readBuf, PdfBufHfSize, &nRead, NULL ) != TRUE )
			{	return( NULL );
			}
			BufTopPos = pos;
//			currentPos = BufAd2FPos( &readBuf[nRead - 1]);
		}
		currentPos = seekPos;
		updateSeekPos = FALSE;
	}
	for ( ch = BufDat( currentPos ); ( ch == '\n' ) || ( ch == '\r' ) || ( ch == ' ' ) || ( ch == '\0' ); ch = BufDat( --currentPos ))
	{	if ( currentPos <= 0 )
		{	return ( NULL );
		}
		BufDat( currentPos ) ='\0';				// Token セパレータ 文字列終端に置き換え 
		if ( LtReadReverse( currentPos ))
		{	if ( ReadReverseHalf() != NULL )
			{	//currentPos += PdfBufHfSize;
	}	}	}
	for ( len = 0, ch = BufDat( currentPos ); ( ch != '\n' ) && ( ch != '\r' ) && ( ch != ' ' ) && ( ch != '\0' ); ch = BufDat( --currentPos ))
	{	if ( currentPos <= 0 )
		{	return ( NULL );
		}
		len++;
		if ( LtReadReverse( currentPos ))
		{	if ( ReadReverseHalf() != NULL )
			{	//currentPos += PdfBufHfSize;
	}	}	}
	return( FPos2BufAd( currentPos + 1 ));
}


char *RdPdfTkn::getForwardToken( char *pBuf )	// 現在のシークポイントから次の１語を返す
{	DWORD nRead, pos, len;
	char ch;

	if ( updateSeekPos )
	{	pos = calcClusterTop( seekPos );
		if ( pos != ( BufTopPos + PdfBufHfSize ))
		{	SetFilePointer( hFile, pos, NULL, FILE_BEGIN );
			if ( ReadFile( hFile, &( readBuf[PdfBufHfSize]), PdfBufHfSize, &nRead, NULL ) != TRUE )
			{	return( NULL );
			}
//			currentPos = BufAd2FPos( &readBuf[PdfBufHfSize]);
			BufTopPos = pos - PdfBufHfSize;
		}
		currentPos = seekPos;
		updateSeekPos = FALSE;
	}
	for ( ch = BufDat( currentPos ); ( ch == '\n' ) || ( ch == '\r' ) || ( ch == ' ' ) || ( ch == '\0' ); ch = BufDat( ++currentPos ))
	{	if ( PdfSize <= currentPos )
		{	return ( NULL );
		}
		BufDat( currentPos ) ='\0';				// Token セパレータ 文字列終端に置き換え 
		if ( GtReadForward( currentPos ))
		{	if ( ReadForwardHalf() != NULL )
			{	//currentPos -= PdfBufHfSize;
	}	}	}
	pos = currentPos;
	for ( len = 0, ch = BufDat( currentPos ); ( ch != '\n' ) && ( ch != '\r' ) && ( ch != ' ' ) && ( ch != '\0' ); ch = BufDat( ++currentPos ))
	{	if ( PdfSize <= currentPos )
		{	return ( NULL );
		}
		len++;
		if ( GtReadForward( currentPos ))
		{	if ( ReadForwardHalf() != NULL )
			{	//currentPos -= PdfBufHfSize;
	}	}	}
	BufDat( currentPos ) ='\0';					// Token セパレータ 文字列終端に置き換え 
	return( FPos2BufAd( pos ));
}


char *RdPdfTkn::ReadStreem( char *pBuf, DWORD len )	// stream\0 \0部分をcurrentPosは指しているので 
{	DWORD nRead;
	char *pSrc;

	if (( FPos2BufAd( ++currentPos ) + len ) < &( readBuf[sizeof( readBuf )]))	// on memory?
	{	memcpy( pBuf, ( pSrc = FPos2BufAd( currentPos )), len );
		currentPos += len;
	}
	else
	{	SetFilePointer( hFile, currentPos, NULL, FILE_BEGIN );
		if ( ReadFile( hFile, pBuf, len, &nRead, NULL ) != TRUE )
		{	return( NULL );
		}
		currentPos += nRead;
	}
	return( pBuf );
}


DWORD RdPdfTkn::Size()
{	return ( PdfSize );
}


