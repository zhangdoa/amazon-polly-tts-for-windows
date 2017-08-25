// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright © Microsoft Corporation. All rights reserved

/******************************************************************************
* InstallVoices.cpp *
*-------------*
*   This application assembles a simple voice font for the sample TTS engine.
*
******************************************************************************/
#include "stdafx.h"
#include <PollyTTSEngine_i.c>
#include <direct.h>
#include "PollyTTSEngine.h"

int wmain(int argc, __in_ecount(argc) WCHAR* argv[])
{
    static const DWORD dwVersion = { 1 };
    ULONG ulNumWords = 0;
    HRESULT hr = S_OK;

    ::CoInitialize( NULL );

    //--- Register the new voice file
    //    The section below shows how to programatically create a token for
    //    the new voice and set its attributes.
    if( SUCCEEDED( hr ) )
    {
        CComPtr<ISpObjectToken> cpToken;
        CComPtr<ISpDataKey> cpDataKeyAttribs;
        hr = SpCreateNewTokenEx(
                SPCAT_VOICES, 
                L"PollyJoanna", 
                &CLSID_PollyTTSEngine, 
                L"Amazon Polly Joanna - English", 
                0x409, 
                L"Amazon Polly Joanna - English", 
                &cpToken,
                &cpDataKeyAttribs);

        //--- Set additional attributes for searching and the path to the
        //    voice data file we just created.
        if (SUCCEEDED(hr))
        {
            hr = cpDataKeyAttribs->SetStringValue(L"Gender", L"Female");
            if (SUCCEEDED(hr))
            {
                hr = cpDataKeyAttribs->SetStringValue(L"Name", L"PollyJoanna");
            }
            if (SUCCEEDED(hr))
            {
                hr = cpDataKeyAttribs->SetStringValue(L"Language", L"409");
            }
            if (SUCCEEDED(hr))
            {
                hr = cpDataKeyAttribs->SetStringValue(L"Age", L"Adult");
            }
			if (SUCCEEDED(hr))
			{
				hr = cpDataKeyAttribs->SetStringValue(L"VoiceName", L"Joanna");
			}
			if (SUCCEEDED(hr))
            {
                hr = cpDataKeyAttribs->SetStringValue(L"Vendor", L"AWS");
            }
        }

		hr = SpCreateNewTokenEx(
			SPCAT_VOICES,
			L"PollyBrian",
			&CLSID_PollyTTSEngine,
			L"Amazon Polly Brian Test - British English",
			0x409,
			L"Amazon Polly Brian Test - British English",
			&cpToken,
			&cpDataKeyAttribs);

		//--- Set additional attributes for searching and the path to the
		//    voice data file we just created.
		if (SUCCEEDED(hr))
		{
			hr = cpDataKeyAttribs->SetStringValue(L"Gender", L"Male");
			if (SUCCEEDED(hr))
			{
				hr = cpDataKeyAttribs->SetStringValue(L"Name", L"PollyBrian");
			}
			if (SUCCEEDED(hr))
			{
				hr = cpDataKeyAttribs->SetStringValue(L"Language", L"409");
			}
			if (SUCCEEDED(hr))
			{
				hr = cpDataKeyAttribs->SetStringValue(L"Age", L"Adult");
			}
			if (SUCCEEDED(hr))
			{
				hr = cpDataKeyAttribs->SetStringValue(L"VoiceName", L"Brian");
			}

			if (SUCCEEDED(hr))
			{
				hr = cpDataKeyAttribs->SetStringValue(L"Vendor", L"AWS");
			}
		}
		hr = SpCreateNewTokenEx(
			SPCAT_VOICES,
			L"PollyRussell",
			&CLSID_PollyTTSEngine,
			L"Amazon Polly Russell - British English",
			0x409,
			L"Amazon Polly Russell - British English",
			&cpToken,
			&cpDataKeyAttribs);

		//--- Set additional attributes for searching and the path to the
		//    voice data file we just created.
		if (SUCCEEDED(hr))
		{
			hr = cpDataKeyAttribs->SetStringValue(L"Gender", L"Male");
			if (SUCCEEDED(hr))
			{
				hr = cpDataKeyAttribs->SetStringValue(L"Name", L"PollyRussell");
			}
			if (SUCCEEDED(hr))
			{
				hr = cpDataKeyAttribs->SetStringValue(L"Language", L"409");
			}
			if (SUCCEEDED(hr))
			{
				hr = cpDataKeyAttribs->SetStringValue(L"Age", L"Adult");
			}
			if (SUCCEEDED(hr))
			{
				hr = cpDataKeyAttribs->SetStringValue(L"VoiceName", L"Russell");
			}

			if (SUCCEEDED(hr))
			{
				hr = cpDataKeyAttribs->SetStringValue(L"Vendor", L"AWS");
			}
		}
	}



    ::CoUninitialize();
    return FAILED( hr );
}

