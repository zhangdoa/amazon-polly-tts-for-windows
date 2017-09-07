// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//

/******************************************************************************
* InstallVoices.cpp *
*-------------*
*   Install Amazon Polly Voices
*
******************************************************************************/
#include "stdafx.h"
#include <iostream>
#include <PollyTTSEngine_i.c>
#include <direct.h>
#include "PollyTTSEngine.h"
#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h>
#include <aws/polly/model/DescribeVoicesRequest.h>
#include <aws/polly/PollyClient.h>
#include "VoiceForSapi.h"


using namespace Aws::Polly;

typedef Aws::Vector<VoiceForSAPI> voice_vec_t;
typedef std::map<Aws::Polly::Model::VoiceId, VoiceForSAPI> voice_map_t;

voice_vec_t ListAvailableVoices(void);
voice_map_t ListAvailableVoicesAsMap(void);
void PrintHelp(WCHAR*);
bool RemoveVoice(WCHAR*);

int wmain(int argc, __in_ecount(argc) WCHAR* argv[])
{
	HRESULT hr = S_OK;

	if (argc > 1)
	{
		::CoInitialize(NULL);

		if (wcscmp(argv[1], L"install") == 0)
		{			
			std::cout << "Installing Amazon Polly Voices!" << std::endl;			

			//--- Register the new voice file

			//for (auto& voice : ListAvailableVoices())
			voice_vec_t all_voices = ListAvailableVoices();

			//for (int i = 0; i < 1; i++)
			for (int i = 0; i < all_voices.size(); i++)
			{
				VoiceForSAPI voiceForSapi = all_voices[i];

				std::wcout << L"Installing " << voiceForSapi.tokenKeyName << " - ";
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
		else if (wcscmp(argv[1], L"uninstall") == 0)
		{
			voice_map_t voices = ListAvailableVoicesAsMap();
			for (auto& voice : voices)
			{
				std::wcout << L"Removing " << voice.second.tokenKeyName << " - ";
				std::wcout << voice.second.langIndependentName << std::endl;

				RemoveVoice(voice.second.tokenKeyName);
			}
		}
		else {
			PrintHelp(argv[0]);
			hr = E_INVALIDARG;
		}
		::CoUninitialize();
	}
	else {
		PrintHelp(argv[0]);
		hr = E_INVALIDARG;
	}
	return FAILED(hr);
}

void PrintHelp(WCHAR* exeName)
{
	printf("Usage to install all voices   : > %ws install \n", exeName);
	printf("Usage to uninstall all voices : > %ws uninstall \n", exeName);
}

voice_vec_t ListAvailableVoices()
{
	Aws::SDKOptions options;
	Aws::InitAPI(options);
	Aws::Polly::PollyClient pc;
	voice_vec_t polly_voices;

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
	Aws::ShutdownAPI(options);
	return polly_voices;
}

voice_map_t ListAvailableVoicesAsMap()
{
	Aws::SDKOptions options;
	Aws::InitAPI(options);
	Aws::Polly::PollyClient pc;
	voice_map_t pollyVoices;

	DescribeVoicesRequest describeVoices;

	auto voicesOutcome = pc.DescribeVoices(describeVoices);
	if (voicesOutcome.IsSuccess())
	{
		for (auto& voice : voicesOutcome.GetResult().GetVoices())
		{
			VoiceForSAPI v4sp(voice);
			pollyVoices.insert(std::make_pair(voice.GetId(), v4sp));
		}
	}
	else
	{
		std::cout << "Error while getting voices" << std::endl;
	}
	Aws::ShutdownAPI(options);
	return pollyVoices;
}


bool RemoveVoice(WCHAR* tokenKeyName)
{
	std::wstring subKey;
	subKey = L"SOFTWARE\\Microsoft\\Speech\\Voices\\Tokens\\";
	subKey += tokenKeyName;
	long result = SHDeleteKey(HKEY_LOCAL_MACHINE, subKey.c_str());
	return (result == ERROR_SUCCESS);
}