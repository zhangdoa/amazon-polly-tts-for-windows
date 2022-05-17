/*  Copyright 2017 - 2018 Amazon.com, Inc. or its affiliates.All Rights Reserved.
Licensed under the Amazon Software License(the "License").You may not use
this file except in compliance with the License.A copy of the License is
located at

http://aws.amazon.com/asl/

and in the "LICENSE" file accompanying this file.This file is distributed
on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, express
or implied.See the License for the specific language governing
permissions and limitations under the License. */

#include "stdafx.h"
#include "aws/polly/PollyClient.h"
#include "VoiceForSapi.h"
#include <codecvt>

VoiceForSAPI::VoiceForSAPI(const Voice& voice, bool _isNeural, bool _isNews, bool _isConversational)
{
	age = L"Adult"; //Polly doesn't have age attribute for voices, setting Adult as default.

	std::pair<int, std::wstring> languagePair = GetVoiceHexValue(voice.GetLanguageCode());
	langid = languagePair.first;
	languageText = languagePair.second;
	VoiceId id = voice.GetId();
	hasNeural = _isNeural;
	hasNewscasterStyle = _isNews;
	hasConversationalStyle = _isConversational;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	gender = converter.from_bytes(GenderMapper::GetNameForGender(voice.GetGender()).c_str());

	std::wstring voiceName = converter.from_bytes(VoiceIdMapper::GetNameForVoiceId(id).c_str()); // "Joanna"
	std::wstring voiceNameUpper = converter.from_bytes(Aws::Utils::StringUtils::ToUpper(VoiceIdMapper::GetNameForVoiceId(id).c_str()).c_str()); // "JOANNA"
	
	std::wstring langCode = converter.from_bytes(LanguageCodeMapper::GetNameForLanguageCode(voice.GetLanguageCode()).c_str()); // "en-us"
	std::wstring langCodeUpper = converter.from_bytes(Aws::Utils::StringUtils::ToUpper(LanguageCodeMapper::GetNameForLanguageCode(voice.GetLanguageCode()).c_str()).c_str()); // "EN-US"
	
	std::wstring languageName = converter.from_bytes(voice.GetLanguageName().c_str()); // "US English"
	
	this->voiceId = voiceName;
	
	std::wstring prefix;
	if (_isConversational) {
		prefix = L"TTS_AMZN_CONVERSATIONAL";
	}
	else if (_isNews) {
		prefix = L"TTS_AMZN_NEWS";
	}
	else if (_isNeural) {
		prefix = L"TTS_AMZN_NEURAL";
	}
	else
	{
		prefix = L"TTS_AMZN";
	}
	// constructing TTS_AMZN_EN-US_JOANNA
	std::wstring voiceToken = prefix;
	voiceToken.append(langCodeUpper);
	voiceToken.append(L"_");
	voiceToken.append(voiceNameUpper);
	tokenKeyName = voiceToken;

	// constructing L"Amazon Polly Joanna"
	std::wstring nameForSAPI = L"Amazon Polly ";
	nameForSAPI.append(voiceName);
	name = nameForSAPI;

	// constructing L"Amazon Polly Joanna - British English"
	std::wstring langName;
	langName = L"Amazon Polly - ";
	langName.append(converter.from_bytes(voice.GetLanguageName().c_str()));
	langName.append(L" - ");
	langName.append(voiceName);
	if (_isConversational) {
		langName.append(L" (Conversational)");
	}
	else if (_isNews) {
		langName.append(L" (Newscaster)");
	}
	else if (_isNeural) {
		langName.append(L" (Neural)");
	}
	else
	{
		langName.append(L" (Standard)");
	}
	langDependentName = langIndependentName = langName;
}


std::pair<int, std::wstring> VoiceForSAPI::GetVoiceHexValue(LanguageCode code) {
	std::wstring langText;
	int langHex;
	
	switch (code)
	{
	case LanguageCode::cy_GB:
		langHex = 0x0452;
		langText = L"452";
		break;
	case LanguageCode::da_DK:
		langHex = 0x0406;
		langText = L"406";
		break;
	case LanguageCode::de_DE:
		langHex = 0x0C07;
		langText = L"C07";
		break;
	case LanguageCode::en_AU:
		langHex = 0x0C09;
		langText = L"C09";
		break;
	case LanguageCode::en_GB:
		langHex = 0x0809;
		langText = L"809";
		break;
	case LanguageCode::en_GB_WLS: // no exact match
		langHex = 0x0809;
		langText = L"809";
		break;
	case LanguageCode::en_IN:
		langHex = 0x4009;
		langText = L"4009";
		break;
	case LanguageCode::en_US:
		langHex = 0x0409;
		langText = L"409";
		break;
	case LanguageCode::es_ES:
		langHex = 0x2C0A;
		langText = L"2C0A";
		break;
	case LanguageCode::es_US:
		langHex = 0x540A;
		langText = L"540A";
		break;
	case LanguageCode::fr_CA:
		langHex = 0x0C0C;
		langText = L"C0C";
		break;
	case LanguageCode::fr_FR:
		langHex = 0x040C;
		langText = L"40C";
		break;
	case LanguageCode::is_IS:
		langHex = 0x0404;
		langText = L"409";
		break;
	case LanguageCode::it_IT:
		langHex = 0x0410;
		langText = L"410";
		break;
	case LanguageCode::ja_JP:
		langHex = 0x0411;
		langText = L"411";
		break;
	case LanguageCode::nb_NO:
		langHex = 0x0414;
		langText = L"414";
		break;
	case LanguageCode::nl_NL:
		langHex = 0x0813;
		langText = L"813";
		break;
	case LanguageCode::pl_PL:
		langHex = 0x0415;
		langText = L"415";
		break;
	case LanguageCode::pt_BR:
		langHex = 0x0416;
		langText = L"416";
		break;
	case LanguageCode::pt_PT:
		langHex = 0x0816;
		langText = L"816";
		break;
	case LanguageCode::ro_RO:
		langHex = 0x0418;
		langText = L"418";
		break;
	case LanguageCode::ru_RU:
		langHex = 0x0419;
		langText = L"419";
		break;
	case LanguageCode::sv_SE:
		langHex = 0x081D;
		langText = L"81D";
		break;
	case LanguageCode::tr_TR:
		langHex = 0x041F;
		langText = L"41F";
		break;
	case LanguageCode::arb:
		langHex = 0x3001;
		langText = L"3001";
		break;
	case LanguageCode::cmn_CN:
		langHex = 0x804;
		langText = L"804";
		break;
	case LanguageCode::es_MX:
		langHex = 0x080A;
		langText = L"080A";
		break;
	case LanguageCode::ko_KR:
		langHex = 0x0412;
		langText = L"0412";
		break;
	default:
		std::cout << "No Language value matched,returning English" << "\n";
		langHex = 0x0409;
		langText = L"409";
	}
	return std::make_pair(langHex, langText);
}

void VoiceForSAPI::PrintVoice() const
{
	std::wcout << L"_________Printing Voice Attributes_________" << std::endl;
	wprintf(L"Token name: %ls\n", tokenKeyName.c_str());
	//std::wprintf(L"%ls\n", gender);
	std::wcout << L"Language Independent Name: " << langIndependentName << std::endl;
	std::wcout << L"Language Dependent Name: " << langDependentName << std::endl;
	std::wcout << L"Gender: " << gender << std::endl;
	std::wcout << L"Name: " << name << std::endl;
	printf("%s: 0x%04X\n", "Language in Hex: ", langid);
	std::wcout << L"Language in Text: " << languageText << std::endl;
	std::wcout << L"Age: " << age << std::endl;
	std::wcout << L"Vendor: " << vendor << std::endl;
}