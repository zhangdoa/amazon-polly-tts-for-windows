/*  Copyright 2017 - 2018 Amazon.com, Inc. or its affiliates.All Rights Reserved.
Licensed under the Amazon Software License(the "License").You may not use
this file except in compliance with the License.A copy of the License is
located at

http://aws.amazon.com/asl/

and in the "LICENSE" file accompanying this file.This file is distributed
on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, express
or implied.See the License for the specific language governing
permissions and limitations under the License. */

#pragma once
#include "PollySpeechResponse.h"
#include "PollySpeechMarksResponse.h"
#include "aws/polly/model/VoiceId.h"
#include <unordered_map>
#include "spdlog/spdlog.h"
namespace spd = spdlog;

class CSentItem;
using namespace Aws::Polly::Model;

class PollyManager
{
public:
	PollyManager(LPWSTR voiceName, bool isNeural, bool isNews, bool isConversational);
	PollySpeechResponse GenerateSpeech(CSentItem& item);
	std::string ParseXMLOutput(std::string& xmlBuffer);
	PollySpeechMarksResponse PollyManager::GenerateSpeechMarks(CSentItem& item, std::streamsize streamSize);
	void SetVoice(LPWSTR voiceName);

private:
	std::wstring m_sVoiceName;
	std::shared_ptr<spd::logger> m_logger;
	VoiceId m_vVoiceId;
	bool m_isNeural = false;
	bool m_isNews = false;
	bool m_isConversational = false;
	std::unordered_map<std::wstring, VoiceId> vm = {
		{ L"Aditi",		   VoiceId::Aditi },
		{ L"Amy",		   VoiceId::Amy },
		{ L"Amy (Neural)", VoiceId::Amy },
		{ L"Astrid",       VoiceId::Astrid },
		{ L"Brian",		   VoiceId::Brian },
		{ L"Brian, (Neural)", VoiceId::Brian },
		{ L"Carla",        VoiceId::Carla },
		{ L"Carmen",	   VoiceId::Carmen },
		{ L"Celine",       VoiceId::Celine },
		{ L"Chantal",      VoiceId::Chantal },
		{ L"Conchita",     VoiceId::Conchita },
		{ L"Cristiano",    VoiceId::Cristiano },
		{ L"Dora",         VoiceId::Dora },
		{ L"Emma",         VoiceId::Emma },
		{ L"Emma (Neural)",VoiceId::Emma },
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
		{ L"Joanna",	   VoiceId::Joanna },
		{ L"Joanna (Conversatiosnal)",	   VoiceId::Joanna },
		{ L"Joey",		   VoiceId::Joey },
		{ L"Justin",	   VoiceId::Justin },
		{ L"Karl",		   VoiceId::Karl },
		{ L"Kevin",        VoiceId::Kevin }
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
		{ L"Matthew",	   VoiceId::Matthew },
		{ L"Matthew (Conversational)",	   VoiceId::Matthew },
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
		{ L"Seoyeon",	   VoiceId::Seoyeon },
		{ L"Takumi",	   VoiceId::Takumi },
		{ L"Tatyana",	   VoiceId::Tatyana },
		{ L"Vicki",	       VoiceId::Vicki },
		{ L"Vitoria",	   VoiceId::Vitoria },
		{ L"Zeina",		   VoiceId::Zeina },
		{L"Zhiyu",		   VoiceId::Zhiyu}
	};

};