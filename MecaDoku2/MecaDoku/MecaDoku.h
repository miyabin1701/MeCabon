
// MecaDoku.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��

#define UM_USERACTION	(WM_USER+1)

enum TTSNAME { SAPI54 = 0, AQTK10 };

struct SAPI54SETTING
{	int	iRate;				// rate
	int	iVol;				// volume
	int iModeSel;			// if mode
	int iVoiceSel;			// Voice selector
	int	iMuonMs;			// muon ms
	wchar_t	*pszVoiceName;	// Voice name
};


// CMecaDokuApp:
// ���̃N���X�̎����ɂ��ẮAMecaDoku.cpp ���Q�Ƃ��Ă��������B
//

class CMecaDokuApp : public CWinApp
{
public:
	CMecaDokuApp();
	void LoadProfile();
	void SaveProfile();
	int GetProfCharHight();


// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CMecaDokuApp theApp;