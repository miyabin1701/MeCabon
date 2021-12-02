// SettingAQTK10.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MecaDoku.h"
#include "SettingAQTK10.h"
#include "afxdialogex.h"


// CSettingAQTK10 ダイアログ

IMPLEMENT_DYNAMIC(CSettingAQTK10, CDialogEx)

CSettingAQTK10::CSettingAQTK10(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettingAQTK10::IDD, pParent)
{

}

CSettingAQTK10::~CSettingAQTK10()
{
}

void CSettingAQTK10::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPEED, m_Speed);
	DDX_Control(pDX, IDC_ONTEI, m_Ontei);
	DDX_Control(pDX, IDC_ONTEI2, m_Ontei2);
	DDX_Control(pDX, IDC_TAKASA, m_Takasa);
	DDX_Control(pDX, IDC_VOLUME, m_Volume);
	DDX_Control(pDX, IDC_ACCENT, m_Accent);
	DDX_Control(pDX, IDC_ACCENTVAL, m_AccentVal);
	DDX_Control(pDX, IDC_ONTEI2VAL, m_Ontei2Val);
	DDX_Control(pDX, IDC_ONTEIVAL, m_OnteiVal);
	DDX_Control(pDX, IDC_SPEEDVAL, m_SpeedVal);
	DDX_Control(pDX, IDC_TAKASAVAL, m_TakasaVal);
	DDX_Control(pDX, IDC_VOICESEL, m_VoiceSel);
	DDX_Control(pDX, IDC_VOLUMEVAL, m_VolumeVal);
	DDX_Control(pDX, IDC_USEAQTK10, m_UseAqtk10);
}


BEGIN_MESSAGE_MAP(CSettingAQTK10, CDialogEx)
END_MESSAGE_MAP()


// CSettingAQTK10 メッセージ ハンドラー
