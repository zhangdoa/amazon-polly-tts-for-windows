#include "stdafx.h"
#include "PollyManager.h"
#include "LogUtils.h"
#include <aws/polly/PollyClient.h>
#include <aws/polly/model/OutputFormat.h>
#include <aws/polly/model/TextType.h>
#include <aws/core/utils/Outcome.h>
#include <aws/core/utils/StringUtils.h>
#include <aws/polly/model/SynthesizeSpeechRequest.h>
#include "TtsEngObj.h"
#include "PollySpeechMarksResponse.h"
#include "rapidjson/document.h"
#include <unordered_map>

#define NOMINMAX
#ifdef _WIN32
#include <Windows.h>
#endif
#define MAX_SIZE 600000
using namespace Aws::Polly::Model;

void PollyManager::SetVoice (LPWSTR voiceName)
{
	m_sVoiceName = voiceName;
	auto voiceId = vm.find(voiceName);
	m_vVoiceId = voiceId->second ;
}

PollyManager::PollyManager(LPWSTR voiceName)
{
	SetVoice(voiceName);
}

PollySpeechResponse PollyManager::GenerateSpeech(CSentItem& item)
{
	PollySpeechResponse response;
	Aws::Polly::PollyClient p;
	LogUtils log;
	SynthesizeSpeechRequest speech_request;
	auto speech_text = Aws::Utils::StringUtils::FromWString(item.pItem);
	log.Debug("%s: Asking Polly for '%s'", __FUNCTION__, speech_text.c_str());
	speech_request.SetOutputFormat(OutputFormat::pcm);
	speech_request.SetVoiceId(m_vVoiceId);
	speech_request.SetText(speech_text);
	if (Aws::Utils::StringUtils::ToLower(speech_text.c_str()).find("<speak>")==0)
	{
		speech_request.SetTextType(TextType::ssml);
	}
	else
	{
		speech_request.SetTextType(TextType::text);
	}

	speech_request.SetSampleRate("16000");
	auto speech = p.SynthesizeSpeech(speech_request);
	if (!speech.IsSuccess())
	{
		std::stringstream error;
		error << "Unable to generate voice audio: " << speech.GetError().GetMessageW();
		response.ErrorMessage = error.str();
		return response;
	}
	auto &r = speech.GetResult();

	auto& stream = r.GetAudioStream();
	stream.read(reinterpret_cast<char*>(&response.AudioData[0]), MAX_SIZE);
	response.Length = stream.gcount();
	return response;
}

PollySpeechMarksResponse PollyManager::GenerateSpeechMarks(CSentItem& item, std::streamsize streamSize)
{
	LogUtils log;
	SynthesizeSpeechRequest speechMarksRequest;
	PollySpeechMarksResponse response;
	Aws::Polly::PollyClient p;
	auto text = Aws::Utils::StringUtils::FromWString(item.pItem);
	log.Debug("%s: Asking Polly for '%s'", __FUNCTION__, text.c_str());
	speechMarksRequest.SetOutputFormat(OutputFormat::json);
	speechMarksRequest.SetVoiceId(m_vVoiceId);
	speechMarksRequest.SetText(text);
	speechMarksRequest.AddSpeechMarkTypes(SpeechMarkType::word);
	speechMarksRequest.SetTextType(TextType::text);
	speechMarksRequest.SetSampleRate("16000");
	auto speech_marks = p.SynthesizeSpeech(speechMarksRequest);
	if (!speech_marks.IsSuccess())
	{
		std::stringstream error;
		error << "Unable to generate speech marks: " << speech_marks.GetError().GetMessageW();
		response.ErrorMessage = error.str();
		return response;
	}
	auto &m = speech_marks.GetResult();
	auto& m_stream = m.GetAudioStream();
	std::string json_str;
	std::vector<SpeechMark> speechMarks;
	auto firstWord = true;
	long bytesProcessed = 0;
	while (getline(m_stream, json_str)) {
		SpeechMark sm;
		rapidjson::Document d;
		d.Parse(json_str.c_str());
		assert(d.HasMember("end"));
		assert(d["end"].GetInt());
		sm.StartInMs = d["time"].GetInt();
		sm.StartByte = d["start"].GetInt();
		sm.EndByte = d["end"].GetInt();
		sm.Text = d["value"].GetString();
		SpeechMark displaySpeechMark;
		if (!firstWord)
		{
			auto currentSm = speechMarks[speechMarks.size()-1];
			currentSm.TimeInMs = sm.StartInMs - currentSm.StartInMs;
			currentSm.LengthInBytes = 32 * currentSm.TimeInMs;
			displaySpeechMark = currentSm;
			bytesProcessed += currentSm.LengthInBytes;
			speechMarks[speechMarks.size() - 1] = currentSm;
		}
		speechMarks.push_back(sm);
		if (!firstWord)
		{
			log.Debug("Word: %s, Start: %i, End: %i, Time: %i\n", displaySpeechMark.Text.c_str(), displaySpeechMark.StartInMs,
				displaySpeechMark.EndByte,
				displaySpeechMark.TimeInMs);
		}
		firstWord = false;
	}
	auto sm = speechMarks[speechMarks.size() - 1];
	sm.LengthInBytes = streamSize - bytesProcessed;
	sm.TimeInMs = sm.LengthInBytes / 32;
	speechMarks[speechMarks.size() - 1] = sm;
	response.SpeechMarks = speechMarks;
	return response;
}
