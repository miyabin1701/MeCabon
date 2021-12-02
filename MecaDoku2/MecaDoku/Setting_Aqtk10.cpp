// Setting_Aqtk10.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MecaDoku.h"
#include "Setting_Aqtk10.h"
#include "afxdialogex.h"

#include "AquesTalk.h"


extern AQTK_VOICE voice;

// CSetting_Aqtk10 �_�C�A���O

IMPLEMENT_DYNAMIC(CSetting_Aqtk10, CDialogEx)

CSetting_Aqtk10::CSetting_Aqtk10(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetting_Aqtk10::IDD, pParent)
{

}

CSetting_Aqtk10::~CSetting_Aqtk10()
{
}

void CSetting_Aqtk10::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ACCENT, m_Accent);
	DDX_Control(pDX, IDC_ONTEI, m_Ontei);
	DDX_Control(pDX, IDC_ONTEI2, m_Ontei2);
	DDX_Control(pDX, IDC_SPEED, m_Speed);
	DDX_Control(pDX, IDC_TAKASA, m_Takasa);
	DDX_Control(pDX, IDC_VOLUME, m_Volume);
	DDX_Control(pDX, IDC_ACCENTVAL, m_AccentVal);
	DDX_Control(pDX, IDC_ONTEI2VAL, m_Ontei2Val);
	DDX_Control(pDX, IDC_ONTEIVAL, m_OnteiVal);
	DDX_Control(pDX, IDC_SPEEDVAL, m_SpeedVal);
	DDX_Control(pDX, IDC_TAKASAVAL, m_TakasaVal);
	DDX_Control(pDX, IDC_VOLUMEVAL, m_VolumeVal);
	DDX_Control(pDX, IDC_VOICESEL, m_VoiceSel);
}


BEGIN_MESSAGE_MAP(CSetting_Aqtk10, CDialogEx)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_VOICESEL, &CSetting_Aqtk10::OnSelchangeVoicesel)
END_MESSAGE_MAP()


// CSetting_Aqtk10 ���b�Z�[�W �n���h���[


void CSetting_Aqtk10::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{	CSliderCtrl *pSlider;
	wchar_t szVal[16];

	pSlider = ( CSliderCtrl * )pScrollBar;
	if ( pSlider == &m_Speed )
	{	voice.spd = m_Speed.GetPos();	// �b�� 	50-300 default:100
		swprintf( szVal, lengthof( szVal ), L"%3d", voice.spd );
		m_SpeedVal.SetWindowText( szVal );
		return;
	}
	else if ( pSlider == &m_Ontei )
	{	voice.lmd = m_Ontei.GetPos();	// �����P 	0-200 default:100
		swprintf( szVal, lengthof( szVal ), L"%3d", voice.lmd );
		m_OnteiVal.SetWindowText( szVal );
		return;
	}
	else if ( pSlider == &m_Takasa )
	{	voice.pit = m_Takasa.GetPos();	// ���� 	20-200 default:��{�f�ЂɈˑ�
		swprintf( szVal, lengthof( szVal ), L"%3d", voice.pit );
		m_TakasaVal.SetWindowText( szVal );
		return;
	}
	else if ( pSlider == &m_Accent )
	{	voice.acc = m_Accent.GetPos();	// �A�N�Z���g 0-200 default:��{�f�ЂɈˑ�
		swprintf( szVal, lengthof( szVal ), L"%3d", voice.acc );
		m_AccentVal.SetWindowText( szVal );
		return;
	}
	else if ( pSlider == &m_Ontei2 )
	{	voice.fsc = m_Ontei2.GetPos();	// �����Q(�T���v�����O���g��) 50-200 default:100
		swprintf( szVal, lengthof( szVal ), L"%3d", voice.fsc );
		m_Ontei2Val.SetWindowText( szVal );
		return;
	}
	else if ( pSlider == &m_Volume )
	{	voice.vol = m_Volume.GetPos();	// �����Q(�T���v�����O���g��) 50-200 default:100
		swprintf( szVal, lengthof( szVal ), L"%3d", voice.vol );
		m_Volume.SetWindowText( szVal );
		return;
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CSetting_Aqtk10::InitialUpdate()
{	wchar_t szVal[16];

	m_Speed.SetRange(  50, 300, TRUE );
	m_Ontei.SetRange(   0, 200, TRUE );
	m_Takasa.SetRange( 20, 200, TRUE );
	m_Accent.SetRange(  0, 200, TRUE );
	m_Ontei2.SetRange( 50, 200, TRUE );
	m_Volume.SetRange(  0, 300, TRUE );
	m_Speed.SetTicFreq( 5 );
	m_Ontei.SetTicFreq( 5 );
	m_Takasa.SetTicFreq( 5 );
	m_Accent.SetTicFreq( 5 );
	m_Ontei2.SetTicFreq( 5 );
	m_Volume.SetTicFreq( 5 );

	m_Speed.SetPos( voice.spd );	// �b�� 	50-300 default:100
	m_Ontei.SetPos( voice.lmd );	// �����P 	0-200 default:100
	m_Takasa.SetPos( voice.pit );	// ���� 	20-200 default:��{�f�ЂɈˑ�
	m_Accent.SetPos( voice.acc );	// �A�N�Z���g 0-200 default:��{�f�ЂɈˑ�
	m_Ontei2.SetPos( voice.fsc );	// �����Q(�T���v�����O���g��) 50-200 default:100
	m_Volume.SetPos( voice.vol );	// ���� 	0-300 default:100

	swprintf( szVal, lengthof( szVal ), L"%3d", voice.spd );
	m_SpeedVal.SetWindowText( szVal );
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.lmd );
	m_OnteiVal.SetWindowText( szVal );
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.pit );
	m_TakasaVal.SetWindowText( szVal );
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.acc );
	m_AccentVal.SetWindowText( szVal );
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.fsc );
	m_Ontei2Val.SetWindowText( szVal );
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.vol );
	m_VolumeVal.SetWindowText( szVal );

	m_VoiceSel.AddString(( LPCTSTR )L"F1E" );
	m_VoiceSel.AddString(( LPCTSTR )L"F2E" );
	m_VoiceSel.AddString(( LPCTSTR )L"F3E" );
	m_VoiceSel.AddString(( LPCTSTR )L"M1E" );
	m_VoiceSel.AddString(( LPCTSTR )L"M2E" );
	m_VoiceSel.AddString(( LPCTSTR )L"R1E" );
	m_VoiceSel.AddString(( LPCTSTR )L"R2E" );
	m_VoiceSel.SetCurSel( voice.bas );
}


//	CComboBox m_VoiceSel;
//enum VoiceBase { F1E =0, F2E, M1E }; // ��{�f��
// �v���Z�b�g����                 bas  spd  vol  pit  acc  lmd  fsc
//const AQTK_VOICE	gVoice_F1 = { F1E, 100, 100, 100, 100, 100, 100}; // ���� F1
//const AQTK_VOICE	gVoice_F2 = { F2E, 100, 100,  77, 150, 100, 100}; // ���� F2
//const AQTK_VOICE	gVoice_F3 = { F1E,  80, 100, 100, 100,  61, 148}; // ���� F3
//const AQTK_VOICE	gVoice_M1 = { M1E, 100, 100,  30, 100, 100, 100}; // �j�� M1
//const AQTK_VOICE	gVoice_M2 = { M1E, 105, 100,  45, 130, 120, 100}; // �j�� M2
//const AQTK_VOICE	gVoice_R1 = { M1E, 100, 100,  30,  20, 190, 100}; // ���{�b�g R1
//const AQTK_VOICE	gVoice_R2 = { F2E,  70, 100,  50,  50,  50, 180}; // ���{�b�g R2




void CSetting_Aqtk10::OnSelchangeVoicesel()
{	static const AQTK_VOICE *pBaseVoice[] =
	{	&gVoice_F1, &gVoice_F2, &gVoice_F3,
		&gVoice_M1, &gVoice_M2,
		&gVoice_R1, &gVoice_R2
	};
	wchar_t szVal[16];

	voice.bas = m_VoiceSel.GetCurSel();
	m_Speed.SetPos( voice.spd = pBaseVoice[voice.bas]->spd );	// �b�� 	50-300 default:100
	m_Ontei.SetPos( voice.lmd = pBaseVoice[voice.bas]->lmd );	// �����P 	0-200 default:100
	m_Takasa.SetPos( voice.pit = pBaseVoice[voice.bas]->pit );	// ���� 	20-200 default:��{�f�ЂɈˑ�
	m_Accent.SetPos( voice.acc = pBaseVoice[voice.bas]->acc );	// �A�N�Z���g 0-200 default:��{�f�ЂɈˑ�
	m_Ontei2.SetPos( voice.fsc = pBaseVoice[voice.bas]->fsc );	// �����Q(�T���v�����O���g��) 50-200 default:100
	m_Volume.SetPos( voice.vol = pBaseVoice[voice.bas]->vol );	// ���� 	0-300 default:100
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.spd );
	m_SpeedVal.SetWindowText( szVal );
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.lmd );
	m_OnteiVal.SetWindowText( szVal );
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.pit );
	m_TakasaVal.SetWindowText( szVal );
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.acc );
	m_AccentVal.SetWindowText( szVal );
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.fsc );
	m_Ontei2Val.SetWindowText( szVal );
	swprintf( szVal, lengthof( szVal ), L"%3d", voice.vol );
	m_VolumeVal.SetWindowText( szVal );
}
