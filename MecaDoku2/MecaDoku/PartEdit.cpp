// PartEdit.cpp : �����t�@�C��
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



// CPartEdit ���b�Z�[�W �n���h���[




CScrollBar* CPartEdit::GetScrollBarCtrl(int nBar) const
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊�{�N���X���Ăяo���Ă��������B

	return CRichEditCtrl::GetScrollBarCtrl(nBar);
}
