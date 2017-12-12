#pragma once
#include <aws/polly/model/Voice.h>

using namespace Aws::Polly::Model;

class VoiceForSAPI
{
	std::pair<int, const wchar_t*> GetVoiceHexValue(LanguageCode code);
	static wchar_t* AWSStringToWchar(Aws::String);
public:
	WCHAR * tokenKeyName;
	const WCHAR * langIndependentName;
	const WCHAR * langDependentName;
	WORD langid; 
	const WCHAR * gender = L"NOT_SET";
	const WCHAR * name;
	const WCHAR * voiceId;
	const WCHAR * languageText;
	const WCHAR * age;
	const WCHAR * vendor = L"Amazon";

	VoiceForSAPI(Aws::Polly::Model::Voice);
	void PrintVoice() const;
};