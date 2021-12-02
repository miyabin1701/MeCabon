// Setting_Other.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MecaDoku.h"
#include "Setting_Other.h"
#include "afxdialogex.h"


extern BOOL	fgBgBlack;


// Setting_Other ダイアログ

IMPLEMENT_DYNAMIC(Setting_Other, CDialogEx)

Setting_Other::Setting_Other(CWnd* pParent /*=NULL*/)
	: CDialogEx(Setting_Other::IDD, pParent)
{

}

Setting_Other::~Setting_Other()
{
}

void Setting_Other::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_BGBLACK, m_BgBlack);
}


BEGIN_MESSAGE_MAP(Setting_Other, CDialogEx)
//	ON_BN_KILLFOCUS(IDC_BGBLACK, &Setting_Other::OnKillfocusBgblack)
	ON_BN_CLICKED(IDC_BGBLACK, &Setting_Other::OnClickedBgblack)
END_MESSAGE_MAP()


// Setting_Other メッセージ ハンドラー


void Setting_Other::InitialUpdate()
{	CButton *pBgBlack = ( CButton * )GetDlgItem( IDC_BGBLACK );

	m_BgBlack = fgBgBlack;
	pBgBlack->SetCheck( m_BgBlack );
}


void Setting_Other::OnClickedBgblack()
{	CButton *pBgBlack = ( CButton * )GetDlgItem( IDC_BGBLACK );

	fgBgBlack = m_BgBlack = pBgBlack->GetCheck();
}
