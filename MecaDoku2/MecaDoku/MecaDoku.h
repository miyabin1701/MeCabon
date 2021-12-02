
// MecaDoku.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル

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
// このクラスの実装については、MecaDoku.cpp を参照してください。
//

class CMecaDokuApp : public CWinApp
{
public:
	CMecaDokuApp();
	void LoadProfile();
	void SaveProfile();
	int GetProfCharHight();


// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CMecaDokuApp theApp;