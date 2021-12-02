

#include "stdafx.h"
#include "MecaDoku.h"
#include "MecaDokuDlg.h"
#include "EditCsv.h"


// CUserDic ダイアログ

IMPLEMENT_DYNAMIC(CEditCsv, CEdit)


BEGIN_MESSAGE_MAP(CEditCsv, CEdit)
//	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


CEditCsv::CEditCsv()
{	CEdit::CEdit();
}


CEditCsv::~CEditCsv()
{	CEdit::~CEdit();
}


//void CEditCsv::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。
//
//	CEdit::OnLButtonDown(nFlags, point);
//	GetParent()->PostMessage( UM_USERACTION );
//}


void CEditCsv::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	GetParent()->PostMessage( UM_USERACTION );
}


BOOL CEditCsv::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_LBUTTONDOWN )
	{	GetParent()->PostMessage( UM_USERACTION );
	}
	return CEdit::PreTranslateMessage(pMsg);
}
