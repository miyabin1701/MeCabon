// Setting_Sapi54.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MecaDoku.h"
#include "Setting_Sapi54.h"
#include "afxdialogex.h"
#include <sapi.h>
#include <sphelper.h>

extern struct SAPI54SETTING	sapi54;

// CSetting_Sapi54 ダイアログ

IMPLEMENT_DYNAMIC(CSetting_Sapi54, CDialogEx)

CSetting_Sapi54::CSetting_Sapi54(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetting_Sapi54::IDD, pParent)
{

}

CSetting_Sapi54::~CSetting_Sapi54()
{	if ( sapi54.pszVoiceName != NULL )
	{	free( sapi54.pszVoiceName );
		sapi54.pszVoiceName = NULL;
	}
}

void CSetting_Sapi54::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPEED, m_Speed);
	DDX_Control(pDX, IDC_SPEEDVAL, m_SpeedVal);
	DDX_Control(pDX, IDC_VOICESEL, m_VoiceSel);
	DDX_Control(pDX, IDC_VOLUME, m_Volume);
	DDX_Control(pDX, IDC_VOLUMEVAL, m_VolumeVal);
	DDX_Control(pDX, IDC_MODESEL, m_ModeSel);
	DDX_Control(pDX, IDC_AFTMUONEDIT, m_AftMuonms);
}


BEGIN_MESSAGE_MAP(CSetting_Sapi54, CDialogEx)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_VOICESEL, &CSetting_Sapi54::OnSelchangeVoicesel)
	ON_CBN_SELCHANGE(IDC_MODESEL, &CSetting_Sapi54::OnSelchangeModesel)
	ON_EN_CHANGE(IDC_AFTMUONEDIT, &CSetting_Sapi54::OnChangeAftmuonedit)
END_MESSAGE_MAP()

//	CComboBox m_VoiceSel;


//wchar_t *s = RegGetString( HKEY_CURRENT_USER, subkey, L"MyStringValue" );
wchar_t *RegGetString( HKEY hKey, const wchar_t *subKey, const wchar_t *value )
{	DWORD dataSize;
	wchar_t *pszStr;
	LONG retCode;

	if ( ERROR_SUCCESS == ( retCode = ::RegGetValue( hKey, subKey, value, RRF_RT_REG_SZ, NULL, NULL, &dataSize )))
	{	if ( NULL != ( pszStr = ( wchar_t * )malloc( dataSize + sizeof( wchar_t ))))
		{	if ( ERROR_SUCCESS == ( retCode = ::RegGetValue( hKey, subKey, value, RRF_RT_REG_SZ, NULL, pszStr, &dataSize )))
			{	return ( pszStr );
	}	}	}
	return ( NULL );
}


int makeVoiceList( CComboBox *p_VoiceSel )
{	static wchar_t *pszRegistryEntry[] =
	{	SPCAT_VOICES,
		L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech_OneCore\\Voices",
		L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech Server\\v11.0\\Voices"
	};
	static wchar_t *pszRef[] =
	{	L"", L"OneCore", L"Server"
	};
	ISpVoice						*pVoice = NULL;
//	CComPtr <ISpObjectToken>		cpToken[8];
	CComPtr<ISpObjectToken>			cpToken[8];
	CComPtr <IEnumSpObjectTokens>	cpEnum;
//	CComPtr<ISpObjectTokenCategory> cpSpCategory = NULL;
//	CComPtr<IEnumSpObjectTokens> cpSpEnumTokens;
//	CComPtr<ISpObjectToken> pSpTok;
	CComPtr<ISpObjectTokenCategory> cpSpCategory = NULL;
	CComPtr<ISpObjectTokenCategory> cpSpCategory2 = NULL;
    CComPtr<IEnumSpObjectTokens>	cpSpEnumTokens;
    CComPtr<IEnumSpObjectTokens>	cpSpEnumTokens2;
	CComPtr<ISpObjectToken>			tok[8], tok2[16];
	WCHAR *pName, *pGender, *pLanguage, *pAge, buf[1024];
	HRESULT hr;
	ULONG i, n, cnt;
    LPWSTR ws;

	if ( FAILED( ::CoInitialize( NULL )))
	{	return( -1 );
	}
	if ( S_OK != ( hr = CoCreateInstance( CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, ( void ** )&pVoice )))
	{	return( -1 );
	}
//	if ( S_OK != ( hr = SpGetCategoryFromId( SPCAT_VOICES, &cpSpCategory )))
	if ( S_OK != ( hr = SpGetCategoryFromId( L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech_OneCore\\Voices", &cpSpCategory )))
	{	return( -1 );
	}
#if 1
	if ( S_OK != ( hr = cpSpCategory->EnumTokens( NULL, NULL, &cpSpEnumTokens )))
	{	return( -1 );
	}
	hr = cpSpEnumTokens->GetCount( &n );
	for ( cnt = i = 0; i < n; i++ )
	{	hr = cpSpEnumTokens->Item( i, &tok[i]);
		hr = tok[i]->GetId( &ws );
		swprintf( buf, lengthof( buf ) - 1, L"%s\\Attributes", ws );
		CoTaskMemFree( ws );
		pName = RegGetString( HKEY_LOCAL_MACHINE, &buf[19], L"Name" );
		pGender = RegGetString( HKEY_LOCAL_MACHINE, &buf[19], L"Gender" );
		pLanguage = RegGetString( HKEY_LOCAL_MACHINE, &buf[19], L"Language" );
		pAge = RegGetString( HKEY_LOCAL_MACHINE, &buf[19], L"Age" );
		if ( p_VoiceSel )
		{	swprintf( buf, lengthof( buf ) - 1, L"%s[%s %s %s]", pName, pGender, pAge, pLanguage );
			p_VoiceSel->AddString( buf );
			cnt++;
		}
		free( pAge );
		free( pLanguage );
		free( pGender );
		free( pName );
#if 0
		if ( S_OK != ( hr = SpGetCategoryFromId( buf, &cpSpCategory2 )))
		{	continue;
		}
		if ( S_OK != ( hr = cpSpCategory2->EnumTokens( NULL, NULL, &cpSpEnumTokens2 )))
		{	continue;
		}
		hr = cpSpEnumTokens2->GetCount( &n2 );
		for ( i2 = 0; i < n2; i2++ )
		{	hr = cpSpEnumTokens->Item( i2, &tok2[i2]);
			hr = tok2[i2]->GetId( &ws );
			pName = pGender = pLanguage = NULL;
			hr = tok2[i2]->GetStringValue( L"Name", &pName );
			hr = tok2[i2]->GetStringValue( L"Gender", &pGender );
			hr = tok2[i2]->GetStringValue( L"Language", &pLanguage );
			if ( p_VoiceSel )
			{	swprintf( buf, lengthof( buf ) - 1, L"%s %s %s", pName, pGender, pLanguage );
				p_VoiceSel->AddString( buf );
				cnt++;
			}
			CoTaskMemFree( ws );
		}
#endif
	}
#endif
#if 0
//	if ( S_OK != ( hr = SpCategorySetId( SPCAT_VOICES, &cpSpCategory )))
//	{	return( -1 );
//	}
//	if ( S_OK != ( hr = cpSpCategory->EnumTokens( NULL, NULL, &cpSpEnumTokens )))
//	{	return( -1 );
//	}
	for ( i = 0; i < lengthof( pszRegistryEntry ); i++ )
	{	if ( S_OK != ( hr = cpSpCategory->EnumTokens( NULL, NULL, &cpSpEnumTokens )))
//		if ( S_OK != ( hr = cpSpCategory->EnumTokens( NULL, NULL, &cpSpEnumTokens )))
//		if ( S_OK != ( hr = cpSpCategory->EnumTokens( pszRegistryEntry[i], L"language = 411", &cpSpEnumTokens )))
//		if ( S_OK != ( hr = SpEnumTokens( pszRegistryEntry[i], L"language = 411", NULL, &cpEnum )))
//cat->EnumTokens(L"name=Microsoft Sayaka", 0, &toks);
		{	continue;
		}
#if 0
	//And enumerate
    unsigned long i, n;
    hr = cpSpEnumTokens->GetCount(&n);
    LPWSTR ws;
	CComPtr<ISpObjectToken>	tok[8];
//    hr = pVoice->GetVoice(&tok); //Retrieve the default voice
	for (i = 0; i < n; i++)
    {
        hr = cpSpEnumTokens->Item(i, &tok[i]);
        hr = tok[i]->GetId(&ws);
 			pName = pGender = pLanguage = NULL;
			hr = cpSpEnumTokens->GetStringValue( L"411", &pName );
			hr = tok[i]->GetStringValue( L"411", &pName );
//			hr = tok[i]->GetStringValue( L"CLSID", &pName );
//			hr = tok[i]->GetStringValue( L"name", &pName );
			hr = tok[i]->GetStringValue( L"gender", &pGender );
			hr = tok[i]->GetStringValue( L"language", &pLanguage );
       CoTaskMemFree(ws);
    }
#endif
		while (( hr = cpSpEnumTokens->Next( 1, &cpToken[i], NULL )) == S_OK )
		{	pName = pGender = pLanguage = NULL;
//			hr = cpToken[i]->GetStringValue( L"411", &pName );
			hr = cpToken[i]->GetStringValue( L"Attributes\\Name", &pName );
			hr = cpToken[i]->GetStringValue( L"Gender", &pGender );
			hr = cpToken[i]->GetStringValue( L"Language", &pLanguage );
//			swprintf( buf, lengthof( buf ) - 1, L"%d %s %s %s %s", i, pName, pGender, pLanguage, pszRef[i]);
//			p_VoiceSel->AddString( buf );
            LONG index = 0;
            WCHAR *key = NULL;
            while ( S_OK == ( hr = cpToken[i]->EnumKeys( index, &key )))
            {
                // Gets some elements
                WCHAR* pValue = NULL;
                cpToken[i]->GetStringValue( key, &pValue );
                // Loops once, key value is "Attributes"
                index++;
            }
            index = 0;
			key = L"Attributes";
            while ( S_OK == ( hr = cpToken[i]->EnumValues( index, &key ) ) )
            {
                // Loops many times, but none of these have what I need
                WCHAR* pValue = NULL;
                cpToken[i]->GetStringValue( key, &pValue );
                index++;
            }
		}
	}
#endif
	return ( i );
}


void CSetting_Sapi54::InitialUpdate()
{	wchar_t szVal[16];

	m_Speed.SetRange( -10, 10, TRUE );
	m_Volume.SetRange( 0, 100, TRUE );
	m_Speed.SetTicFreq( 5 );
	m_Volume.SetTicFreq( 20 );
//	SetRate( sapi54.iRate );		//
//	SetVolume( sapi54.iVol );		//
	m_Speed.SetPos( sapi54.iRate );	// 話速 -10 - +10
	m_Volume.SetPos( sapi54.iVol );	// 音量 0 - 100

	swprintf( szVal, lengthof( szVal ), L"%3d", sapi54.iRate );
	m_SpeedVal.SetWindowText( szVal );
	swprintf( szVal, lengthof( szVal ), L"%3d", sapi54.iVol );
	m_VolumeVal.SetWindowText( szVal );

	m_ModeSel.AddString(( LPCTSTR )L"カタカナダイレクト" );
	m_ModeSel.AddString(( LPCTSTR )L"SSML" );
	m_ModeSel.SetCurSel( sapi54.iModeSel );

	swprintf( szVal, lengthof( szVal ), L"%4d", sapi54.iMuonMs );
	m_AftMuonms.SetWindowText( szVal );

	makeVoiceList( &m_VoiceSel );
	m_VoiceSel.SetCurSel( sapi54.iVoiceSel );
	SetVoiceName();
}


// CSetting_Sapi54 メッセージ ハンドラー


void CSetting_Sapi54::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{	CSliderCtrl *pSlider;
	wchar_t szVal[16];

	pSlider = ( CSliderCtrl * )pScrollBar;
	if ( pSlider == &m_Speed )
	{	sapi54.iRate = m_Speed.GetPos();	// 話速
//		SetRate( sapi54.iRate );			// -10 - +10
		swprintf( szVal, lengthof( szVal ), L"%3d", sapi54.iRate );
		m_SpeedVal.SetWindowText( szVal );
		return;
	}
	else if ( pSlider == &m_Volume )
	{	sapi54.iVol = m_Volume.GetPos();	// 音量
//		SetVolume( sapi54.iVol );			// 0 - 100
		swprintf( szVal, lengthof( szVal ), L"%3d", sapi54.iVol );
		m_VolumeVal.SetWindowText( szVal );
		return;
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CSetting_Sapi54::SetVoiceName()
{	int n;
	wchar_t *pszSep;

	if ( sapi54.pszVoiceName != NULL )
	{	free( sapi54.pszVoiceName );
		sapi54.pszVoiceName = NULL;
	}
	n = ( m_VoiceSel.GetLBTextLen( sapi54.iVoiceSel ) + 6 ) * sizeof( wchar_t );
	if (( sapi54.pszVoiceName = ( wchar_t * )malloc( n )) != NULL )
	{	wcscpy( sapi54.pszVoiceName, L"Name=" );
		m_VoiceSel.GetLBText( sapi54.iVoiceSel, &( sapi54.pszVoiceName[5]));
		if (( pszSep = wcschr( sapi54.pszVoiceName, L'[' )) != NULL )
		{	*pszSep = L'\0';
	}	}
}	//"Name=%s Language = 411"


void CSetting_Sapi54::OnSelchangeVoicesel()
{	sapi54.iVoiceSel = m_VoiceSel.GetCurSel();	// 
	SetVoiceName();
}


void CSetting_Sapi54::OnSelchangeModesel()
{	sapi54.iModeSel = m_ModeSel.GetCurSel();	// if mode
}


void CSetting_Sapi54::OnChangeAftmuonedit()
{	int MuonMs;
	wchar_t szEdit[32];
	// TODO:  これが RICHEDIT コントロールの場合、このコントロールが
	// この通知を送信するには、CDialogEx::OnInitDialog() 関数をオーバーライドし、
	// CRichEditCtrl().SetEventMask() を
	// OR 状態の ENM_CHANGE フラグをマスクに入れて呼び出す必要があります。

	m_AftMuonms.GetWindowText( szEdit, lengthof( szEdit ) - 1 );
	if (( MuonMs = _wtoi( szEdit )) > 1000 )
	{	MuonMs = 1000;
		m_AftMuonms.SetWindowText( L"1000" );
		m_AftMuonms.SetSel( 4, -1 );
	}
	else if ( MuonMs < 1 )
	{	MuonMs = 1;
		m_AftMuonms.SetWindowText( L"1" );
		m_AftMuonms.SetSel( 1, -1 );
	}
	sapi54.iMuonMs = MuonMs;	// 
}

