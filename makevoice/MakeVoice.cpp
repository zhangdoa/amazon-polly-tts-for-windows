// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright © Microsoft Corporation. All rights reserved

/******************************************************************************
* MakeVoice.cpp *
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

    //--- Check args
    if( argc != 4 )
    {
        printf( "%s", "Usage: > MakeVoice [[in]word list file] [[out]voice file] [voice name]\n" );
        hr = E_INVALIDARG;
    }
    else
    {
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
                    argv[3], 
                    &CLSID_PollyTTSEngine, 
                    L"Amazon Polly Joanna - British English", 
                    0x409, 
                    L"Amazon Polly Joanna - British English", 
                    &cpToken,
                    &cpDataKeyAttribs);

            //--- Set additional attributes for searching and the path to the
            //    voice data file we just created.
            if (SUCCEEDED(hr))
            {
                hr = cpDataKeyAttribs->SetStringValue(L"Gender", L"Male");
                if (SUCCEEDED(hr))
                {
                    hr = cpDataKeyAttribs->SetStringValue(L"Name", L"AmazonPollyTTSVoice");
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
                    hr = cpDataKeyAttribs->SetStringValue(L"Vendor", L"Microsoft");
                }
            }
        }

        ::CoUninitialize();
    }
    return FAILED( hr );
}

