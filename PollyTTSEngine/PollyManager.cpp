#include "stdafx.h"
#include "PollyManager.h"
#include "LogUtils.h"
#include <aws/polly/PollyClient.h>
#include <aws/polly/model/OutputFormat.h>
#include <aws/polly/model/TextType.h>
#include <aws/core/utils/Outcome.h>
#include <aws/polly/model/SynthesizeSpeechRequest.h>
#include "TtsEngObj.h"
#include "PollySpeechMarksResponse.h"
#include "rapidjson/document.h"

#ifdef _WIN32
#include <Windows.h>
#endif
#define MAX_SIZE 600000

PollySpeechResponse PollyManager::GenerateSpeech(CSentItem& item)
{
	PollySpeechResponse response;
	Aws::Polly::PollyClient p;
	LogUtils log;
	Aws::Polly::Model::SynthesizeSpeechRequest speech_request;
	Aws::String speech_text = Aws::Utils::StringUtils::FromWString(item.pItem);
	log.Debug("%s: Asking Polly for '%s'", __FUNCTION__, speech_text.c_str());
	speech_request.SetOutputFormat(Aws::Polly::Model::OutputFormat::pcm);
	speech_request.SetVoiceId(Aws::Polly::Model::VoiceId::Joanna);
	speech_request.SetText(speech_text);
	speech_request.SetTextType(Aws::Polly::Model::TextType::text);
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
	Aws::Polly::Model::SynthesizeSpeechRequest speechMarksRequest;
	PollySpeechMarksResponse response;
	Aws::Polly::PollyClient p;
	auto text = Aws::Utils::StringUtils::FromWString(item.pItem);
	log.Debug("%s: Asking Polly for '%s'", __FUNCTION__, text.c_str());
	speechMarksRequest.SetOutputFormat(Aws::Polly::Model::OutputFormat::json);
	speechMarksRequest.SetVoiceId(Aws::Polly::Model::VoiceId::Joanna);
	speechMarksRequest.SetText(text);
	speechMarksRequest.AddSpeechMarkTypes(Aws::Polly::Model::SpeechMarkType::word);
	speechMarksRequest.SetTextType(Aws::Polly::Model::TextType::text);
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
	while (std::getline(m_stream, json_str)) {
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
