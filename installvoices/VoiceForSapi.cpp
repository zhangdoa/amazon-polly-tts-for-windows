#include "stdafx.h"
#include "VoiceForSapi.h"
#include <aws/polly/model/VoiceId.h>
#include <aws/polly/model/DescribeVoicesRequest.h>
#include <aws/polly/PollyClient.h>
#include <aws/polly/model/Gender.h>
#include <aws/polly/model/LanguageCode.h>

using namespace Aws::Polly::Model;

VoiceForSAPI::VoiceForSAPI(Aws::Polly::Model::Voice voice)
{
	age = L"Adult"; //Polly doesn't have age attribute for voices, setting Adult as default.
	
	gender = AWSStringToWchar(GenderMapper::GetNameForGender(voice.GetGender()));

	std::pair<int, const wchar_t*> languagePair = GetVoiceHexValue(voice.GetLanguageCode());
	langid = languagePair.first;
	languageText = languagePair.second;

	Aws::String a_voiceName = VoiceIdMapper::GetNameForVoiceId(voice.GetId()); // "Joanna"
	Aws::String a_voiceNameUpper = Aws::Utils::StringUtils::ToUpper(a_voiceName.c_str()); // "JOANNA"
	const wchar_t* voiceName = AWSStringToWchar(a_voiceName); //L"Joanna"
	const wchar_t* voiceNameUpper = AWSStringToWchar(a_voiceNameUpper); //L"JOANNA"

	Aws::String a_langCode = LanguageCodeMapper::GetNameForLanguageCode(voice.GetLanguageCode()); // "en-us"
	Aws::String a_langCodeUpper = Aws::Utils::StringUtils::ToUpper(a_langCode.c_str()); // "EN-US"
	const wchar_t* langCode = AWSStringToWchar(a_langCode); // L"en-us"
	const wchar_t* langCodeUpper = AWSStringToWchar(a_langCodeUpper); // L"EN-US"

	Aws::String a_languageName = voice.GetLanguageName(); // "US English"
	const wchar_t* languageName = AWSStringToWchar(a_languageName); // L"US English"

	this->voiceId = voiceName;
	wchar_t* prefix = L"TTS_AMZN_";

	// constructing TTS_AMZN_EN-US_JOANNA
	wchar_t* voiceToken = new wchar_t[wcslen(voiceNameUpper) + 
		wcslen(langCodeUpper) + 
		wcslen(L"_") +
		wcslen(prefix)];
	wcscpy(voiceToken, prefix);
	wcscat(voiceToken, langCodeUpper);
	wcscat(voiceToken, L"_");
	wcscat(voiceToken, voiceNameUpper);
	tokenKeyName = voiceToken;
	
	// constructing L"Amazon Polly Joanna"
	wchar_t* nameForSAPI = new wchar_t[100];
	wcscpy(nameForSAPI, L"Amazon Polly ");
	wcscat(nameForSAPI, voiceName);
	name = nameForSAPI;
	
	// constructing L"Amazon Polly Joanna - British English"
	wchar_t* langName = new wchar_t[100];
	wcscpy(langName, L"Amazon Polly ");
	wcscat(langName, voiceName);	
	wcscat(langName, L" - ");
	wcscat(langName, AWSStringToWchar(voice.GetLanguageName()));
	langDependentName = langIndependentName = langName;
}


std::pair<int, const wchar_t*> VoiceForSAPI::GetVoiceHexValue(Aws::Polly::Model::LanguageCode code) {
	wchar_t* langText;
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
		langText = L"40009";
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
	default:
		std::cout << "No Language value matched,returning English" << "\n";
		langHex = 0x0409;
		langText = L"409";
	}
	return std::make_pair(langHex, langText);
}

void VoiceForSAPI::PrintVoice()
{
	std::wcout << L"_________Printing Voice Attributes_________" << std::endl;
	std::wprintf(L"Token name: %ls\n", tokenKeyName);
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

wchar_t * VoiceForSAPI::AWSStringToWchar(Aws::String awsString)
{
	const char* sourcestring = awsString.c_str();
	size_t newsize = strlen(sourcestring) + 1;
	wchar_t * wcstring = new wchar_t[newsize];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wcstring, newsize, sourcestring, _TRUNCATE);
	return wcstring;
}
