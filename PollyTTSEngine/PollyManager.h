#pragma once
#include "PollySpeechResponse.h"
#include "PollySpeechMarksResponse.h"
#include <aws/polly/model/VoiceId.h>
#include <unordered_map>
#include "spdlog/spdlog.h"
namespace spd = spdlog;

class CSentItem;
using namespace Aws::Polly::Model;

class PollyManager
{
public:
	PollyManager(LPWSTR voiceName);
	PollySpeechResponse GenerateSpeech(CSentItem& item);
	std::string ParseXMLOutput(std::string& xmlBuffer);
	PollySpeechMarksResponse PollyManager::GenerateSpeechMarks(CSentItem& item, std::streamsize streamSize);
	void SetVoice(LPWSTR voiceName);

private:
	std::wstring m_sVoiceName;
	std::shared_ptr<spd::logger> m_logger;
	VoiceId m_vVoiceId;
	std::unordered_map<std::wstring, VoiceId> vm = {
		{ L"Aditi",		   VoiceId::Aditi },
		{ L"Amy",		   VoiceId::Amy },
		{ L"Astrid",       VoiceId::Astrid },
		{ L"Brian",		   VoiceId::Brian },
		{ L"Carla",        VoiceId::Carla },
		{ L"Carmen",	   VoiceId::Carmen },
		{ L"Celine",       VoiceId::Celine },
		{ L"Chantal",      VoiceId::Chantal },
		{ L"Conchita",     VoiceId::Conchita },
		{ L"Cristiano",    VoiceId::Cristiano },
		{ L"Dora",         VoiceId::Dora },
		{ L"Emma",         VoiceId::Emma },
		{ L"Enrique",      VoiceId::Enrique },
		{ L"Ewa",          VoiceId::Ewa },
		{ L"Filiz",        VoiceId::Filiz },
		{ L"Geraint",      VoiceId::Geraint },
		{ L"Giorgio",      VoiceId::Giorgio },
		{ L"Gwyneth",      VoiceId::Gwyneth },
		{ L"Hans",         VoiceId::Hans },
		{ L"Ines",         VoiceId::Ines },
		{ L"Ivy"    ,      VoiceId::Ivy },
		{ L"Jacek",        VoiceId::Jacek },
		{ L"Jan",		   VoiceId::Jan },
		{ L"Joanna",	   VoiceId::Joanna},
		{ L"Joey",		   VoiceId::Joey },
		{ L"Justin",	   VoiceId::Justin },
		{ L"Karl",		   VoiceId::Karl },
		{ L"Kendra",	   VoiceId::Kendra },
		{ L"Kimberly",	   VoiceId::Kimberly },
		{ L"Liv",	       VoiceId::Liv },
		{ L"Lotte",	       VoiceId::Lotte },
		{ L"Mads",	       VoiceId::Mads },
		{ L"Maja",	       VoiceId::Maja },
		{ L"Marlene",	   VoiceId::Marlene },
		{ L"Mathieu",	   VoiceId::Mathieu },
		{ L"Matthew",	   VoiceId::Matthew },
		{ L"Maxim",	       VoiceId::Maxim },
		{ L"Miguel",	   VoiceId::Miguel },
		{ L"Mizuki",	   VoiceId::Mizuki },
		{ L"Naja",	       VoiceId::Naja },
		{ L"Nicole",	   VoiceId::Nicole },
		{ L"Penelope",	   VoiceId::Penelope },
		{ L"Raveena",	   VoiceId::Raveena },
		{ L"Ricardo",	   VoiceId::Ricardo },
		{ L"Ruben",	       VoiceId::Ruben },
		{ L"Russell",	   VoiceId::Russell },
		{ L"Salli",	       VoiceId::Salli },
		{ L"Seoyeon",	   VoiceId::Seoyeon},
		{ L"Takumi",	   VoiceId::Takumi },
		{ L"Tatyana",	   VoiceId::Tatyana },
		{ L"Vicki",	       VoiceId::Vicki },
		{ L"Vitoria",	   VoiceId::Vitoria }
	};

};
