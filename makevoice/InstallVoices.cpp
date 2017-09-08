// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//

/******************************************************************************
* InstallVoices.cpp *
*
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

typedef std::map<Aws::Polly::Model::VoiceId, VoiceForSAPI> voice_map_t;
typedef std::set<Aws::Polly::Model::VoiceId> argument_set_t;

voice_map_t SelectedVoicesMap(std::wstring);
void PrintHelp(WCHAR*);
int AddVoice(VoiceForSAPI);
int RemoveVoice(WCHAR*);
argument_set_t ArgumentSet(std::wstring);
Aws::String WStringToAwsString(const std::wstring& s);
std::wstring AwsStringToWString(const Aws::String& s);

int wmain(int argc, __in_ecount(argc) WCHAR* argv[])
{
	HRESULT hr = S_OK;
	if (argc > 3 || argc < 2)
	{
		PrintHelp(argv[0]);
		hr = E_INVALIDARG;
		//return FAILED(hr);
	} else if (argc > 1)
	{
		::CoInitialize(NULL);

		std::wstring voiceList = L"";
		if (argc == 3) {
			voiceList = argv[2];
		}

		voice_map_t voices = SelectedVoicesMap(voiceList);

		if (wcscmp(argv[1], L"install") == 0)
		{			
			
			for (auto& voice : voices)
			{
				std::wcout << L"Installing " << voice.second.tokenKeyName << " - ";
				std::wcout << voice.second.langIndependentName << std::endl;

				AddVoice(voice.second);
			}						
		}
		else if (wcscmp(argv[1], L"uninstall") == 0)
		{			
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

	return FAILED(hr);
}

void PrintHelp(WCHAR* exeName)
{
	printf("Usage to install all voices   : > %ws install \n", exeName);
	printf("Usage to install some voices   : > %ws install Joanna,Filiz\n", exeName);
	printf("Usage to uninstall all voices : > %ws uninstall \n", exeName);
	printf("Usage to uninstall some voices   : > %ws uninstall Joanna,Filiz\n", exeName);
}


voice_map_t SelectedVoicesMap(std::wstring voiceList)
{
	Aws::SDKOptions options;
	Aws::InitAPI(options);
	Aws::Polly::PollyClient pc;
	voice_map_t pollyVoices;
	boolean iCanAdd(false);
	boolean isSelected(false);
	boolean isAllSelected(voiceList.size() < 1);
	DescribeVoicesRequest describeVoices;
	
	auto voicesOutcome = pc.DescribeVoices(describeVoices);
	if (voicesOutcome.IsSuccess())
	{
		auto voiceSet = ArgumentSet(voiceList);
		for (auto& voice : voicesOutcome.GetResult().GetVoices())
		{
			if (!isAllSelected && (voiceSet.find(voice.GetId()) != voiceSet.end())) {
				isSelected = true;
			}
			if (isSelected || isAllSelected)
			{
				VoiceForSAPI v4sp(voice);
				pollyVoices.insert(std::make_pair(voice.GetId(), v4sp));
				isSelected = false;
			}
		}
	}
	else
	{
		std::cout << "Error while getting voices" << std::endl;
	}
	Aws::ShutdownAPI(options);
	return pollyVoices;
}

int AddVoice(VoiceForSAPI voiceForSapi)
{
	HRESULT hr = S_OK;
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
	return SUCCEEDED(hr);
}

int RemoveVoice(WCHAR* tokenKeyName)
{
	std::wstring subKey;
	subKey = L"SOFTWARE\\Microsoft\\Speech\\Voices\\Tokens\\";
	subKey += tokenKeyName;
	long result = SHDeleteKey(HKEY_LOCAL_MACHINE, subKey.c_str());
	if (result == ERROR_SUCCESS) {
		return SUCCEEDED(S_OK);
	}
	return FAILED(result);
}

argument_set_t ArgumentSet(std::wstring str)
{
	argument_set_t argSet;
	std::wstringstream wss(str);

	if (str.compare(L"") == 0) return argSet;

	while (wss.good())
	{
		std::wstring subStr;
		std::getline(wss, subStr, L',');
		Aws::Polly::Model::VoiceId voice = Aws::Polly::Model::VoiceIdMapper::GetVoiceIdForName(WStringToAwsString(subStr));
		argSet.insert(voice);
	}
	return argSet;
}

Aws::String WStringToAwsString(const std::wstring& s)
{
	Aws::String temp(s.length(), ' ');
	std::copy(s.begin(), s.end(), temp.begin());
	return temp;
}

std::wstring AwsStringToWString(const Aws::String& s)
{
	std::wstring temp(s.length(), L' ');
	std::copy(s.begin(), s.end(), temp.begin());
	return temp;
}