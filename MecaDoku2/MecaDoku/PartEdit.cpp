// PartEdit.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MecaDoku.h"
#include "PartEdit.h"


// CPartEdit

IMPLEMENT_DYNAMIC(CPartEdit, CRichEditCtrl)

CPartEdit::CPartEdit()
{

}

CPartEdit::~CPartEdit()
{
}


BEGIN_MESSAGE_MAP(CPartEdit, CEdit)
END_MESSAGE_MAP()



// CPartEdit メッセージ ハンドラー




CScrollBar* CPartEdit::GetScrollBarCtrl(int nBar) const
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。

	return CRichEditCtrl::GetScrollBarCtrl(nBar);
}
