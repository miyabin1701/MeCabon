// SettingSapi54.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MecaDoku.h"
#include "SettingSapi54.h"
#include "afxdialogex.h"


// CSettingSapi54 �_�C�A���O

IMPLEMENT_DYNAMIC(CSettingSapi54, CDialogEx)

CSettingSapi54::CSettingSapi54(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettingSapi54::IDD, pParent)
{

}

CSettingSapi54::~CSettingSapi54()
{
}

void CSettingSapi54::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPEED, m_Speed);
	DDX_Control(pDX, IDC_SPEEDVAL, m_SpeedVal);
	DDX_Control(pDX, IDC_VOICESEL, m_VoiceSel);
	DDX_Control(pDX, IDC_VOLUME, m_Volume);
	DDX_Control(pDX, IDC_VOLUMEVAL, m_VolumeVal);
}


BEGIN_MESSAGE_MAP(CSettingSapi54, CDialogEx)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CSettingSapi54 ���b�Z�[�W �n���h���[


void CSettingSapi54::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h���[ �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
