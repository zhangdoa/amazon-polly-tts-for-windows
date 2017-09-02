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
#include <iostream>
#include <PollyTTSEngine.c>
#include <direct.h>
#include "PollyTTSEngine.h"
#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h>
#include <aws/polly/model/DescribeVoicesRequest.h>
#include <aws/polly/PollyClient.h>
#include "VoiceForSapi.h"


using namespace Aws::Polly;

typedef Aws::Vector<VoiceForSAPI> voice_vec_t;
voice_vec_t ListAvailableVoices(void);

int wmain(int argc, __in_ecount(argc) WCHAR* argv[])
{
	HRESULT hr = S_OK;
	std::cout << "Installing Amazon Polly Voices!" << std::endl;

	Aws::SDKOptions options;
	Aws::InitAPI(options);

	::CoInitialize(NULL);

	//--- Register the new voice file
	//    The section below shows how to programatically create a token for
	//    the new voice and set its attributes.


	if (SUCCEEDED(hr))
	{
		//for (auto& voice : ListAvailableVoices())

		voice_vec_t all_voices = ListAvailableVoices();

		//for (int i = 0; i < 1; i++)
		for (int i = 0; i < all_voices.size(); i++)
		{
			VoiceForSAPI voiceForSapi = all_voices[i];

			std::wcout << i + 1 << ":" << voiceForSapi.tokenKeyName << " - ";
			std::wcout << voiceForSapi.langIndependentName << std::endl;

			CComPtr<ISpObjectToken> cpToken;
			CComPtr<ISpDataKey> cpDataKeyAttribs;

			hr = SpCreateNewTokenEx(
				SPCAT_VOICES,
				voiceForSapi.tokenKeyName,
				&CLSID_PollyTTSEngine,
				voiceForSapi.langDependentName,
				voiceForSapi.langid,
				voiceForSapi.langIndependentName,
				&cpToken,
				&cpDataKeyAttribs);

			//--- Set additional attributes for searching and the path to the
			//    voice data file we just created.
			if (SUCCEEDED(hr))
			{
				hr = cpDataKeyAttribs->SetStringValue(L"Gender", voiceForSapi.gender);
				if (SUCCEEDED(hr))
				{
					hr = cpDataKeyAttribs->SetStringValue(L"Name", voiceForSapi.name);
				}
				if (SUCCEEDED(hr))
				{
					hr = cpDataKeyAttribs->SetStringValue(L"VoiceId", voiceForSapi.voiceId);
				}
				if (SUCCEEDED(hr))
				{
					hr = cpDataKeyAttribs->SetStringValue(L"Language", voiceForSapi.languageText);
				}
				if (SUCCEEDED(hr))
				{
					hr = cpDataKeyAttribs->SetStringValue(L"Age", voiceForSapi.age);
				}
				if (SUCCEEDED(hr))
				{
					hr = cpDataKeyAttribs->SetStringValue(L"Vendor", voiceForSapi.vendor);
				}
			}
		}
	}

	::CoUninitialize();
	Aws::ShutdownAPI(options);
	return FAILED(hr);
}

voice_vec_t ListAvailableVoices()
{
	voice_vec_t polly_voices;
	Aws::Polly::PollyClient pc;

	DescribeVoicesRequest describeVoices;

	auto voicesOutcome = pc.DescribeVoices(describeVoices);
	if (voicesOutcome.IsSuccess())
	{
		for (auto& voice : voicesOutcome.GetResult().GetVoices())
		{
			VoiceForSAPI v4sp(voice);
			polly_voices.push_back(v4sp);
		}
	}
	else
	{
		std::cout << "Error while getting voices" << std::endl;
	}
	return polly_voices;
}
