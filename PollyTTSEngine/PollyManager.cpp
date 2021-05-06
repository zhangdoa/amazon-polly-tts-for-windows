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
#include "PollyManager.h"
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
#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/msvc_sink.h"
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <tinyxml2.h>
namespace spd = spdlog;

#define NOMINMAX
#ifdef _WIN32
#endif
#define MAX_SIZE 6000000
using namespace Aws::Polly::Model;
static const char* ALLOCATION_TAG = "PollyTTSEngine::Main";

void PollyManager::SetVoice (LPWSTR voiceName)
{
	m_logger->debug("{}: Setting voice to {}", __FUNCTION__, Aws::Utils::StringUtils::FromWString(voiceName));
	m_sVoiceName = voiceName;
	auto voiceId = vm.find(voiceName);
	m_vVoiceId = voiceId->second ;
}

PollyManager::PollyManager(LPWSTR voiceName, bool isNeural, bool isNews, bool isConversational)
{
	m_logger = std::make_shared<spd::logger>("msvc_logger", std::make_shared<spd::sinks::msvc_sink_mt>());
#ifdef DEBUG
	m_logger->set_level(spd::level::debug);
#else
	m_logger->set_level(spd::level::info);
#endif
	m_isNeural = isNeural;
	m_isNews = isNews;
	m_isConversational = isConversational;
	SetVoice(voiceName);
}

PollySpeechResponse PollyManager::GenerateSpeech(CSentItem& item)
{
	PollySpeechResponse response;
	
	auto creds = Aws::MakeShared<Aws::Auth::ProfileConfigFileAWSCredentialsProvider>(ALLOCATION_TAG, "polly-windows");
	Aws::Client::ClientConfiguration config = Aws::Client::ClientConfiguration("polly-windows");
	config.userAgent = config.userAgent + " request-source/polly-windows/PRODUCTVERSION";
	Aws::Polly::PollyClient p = Aws::Polly::PollyClient(creds, config);
	SynthesizeSpeechRequest speech_request;
	
	auto speech_text = Aws::Utils::StringUtils::FromWString(item.pItem);
	if (Aws::Utils::StringUtils::ToLower(speech_text.c_str()).find("</voice>") != std::string::npos)
	{
		speech_text = "<speak>" + speech_text.replace(speech_text.find("</voice>"), sizeof("</voice>") - 1, "");
	}
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError res = doc.Parse(speech_text.c_str());
	speech_request.SetTextType(TextType::text);
	if (res == tinyxml2::XML_SUCCESS && strcmp(doc.RootElement()->Name(), "speak") == 0) {
		m_logger->debug("Text type = ssml");
		speech_request.SetTextType(TextType::ssml);
		speech_text = speech_text.replace(speech_text.find("<speak>"), sizeof("<speak>") - 1, "");
	}
	speech_request.SetTextType(TextType::ssml);
	if (m_isNews)
	{
		speech_text = "<speak><amazon:domain name=\"news\">" + speech_text + "</amazon:domain></speak>";
	}
	else if (m_isConversational)
	{
		speech_text = "<speak><amazon:domain name=\"conversational\">" + speech_text + "</amazon:domain></speak>";
	}
	else
	{
		speech_text = "<speak>" + speech_text + "</speak>";
	}
	m_logger->debug("{}: Asking Polly for '{}'", __FUNCTION__, speech_text.c_str());
	speech_request.SetOutputFormat(OutputFormat::pcm);
	speech_request.SetVoiceId(m_vVoiceId);
	m_logger->debug("Generating speech: {}", speech_text);
	speech_request.SetText(speech_text);

	speech_request.SetSampleRate("16000");
	if (m_isNeural) {
		m_logger->debug("Neural voice? Yes");
		speech_request.SetEngine(Engine::neural);
	}
	auto speech = p.SynthesizeSpeech(speech_request);
	response.IsSuccess = speech.IsSuccess();
	if (!speech.IsSuccess())
	{
		std::stringstream error;
		//error << speech.GetError().GetMessageW();
		response.ErrorMessage = error.str();
		return response;
	}
	auto &r = speech.GetResult();

	auto& stream = r.GetAudioStream();
	stream.read(reinterpret_cast<char*>(&response.AudioData[0]), MAX_SIZE);
	response.Length = stream.gcount();
	return response;
}

std::string PollyManager::ParseXMLOutput(std::string &xmlBuffer)
{
	bool copy = true;
	std::string plainString;
	std::stringstream convertStream;

	// remove all xml tags
	for (char i : xmlBuffer)
	{
		convertStream << i;

		if (convertStream.str() == "<") copy = false;
		else if (convertStream.str().compare(">") == 0)
		{
			copy = true;
			convertStream.str(std::string());
			continue;
		}

		if (copy) plainString.append(convertStream.str());

		convertStream.str(std::string());
	}

	return plainString;
}

PollySpeechMarksResponse PollyManager::GenerateSpeechMarks(CSentItem& item, std::streamsize streamSize)
{
	SynthesizeSpeechRequest speechMarksRequest;
	PollySpeechMarksResponse response;
	Aws::Polly::PollyClient p = Aws::MakeShared<Aws::Auth::ProfileConfigFileAWSCredentialsProvider>(ALLOCATION_TAG, "polly-windows");
	auto text = Aws::Utils::StringUtils::FromWString(item.pItem);
	m_logger->debug("{}: Asking Polly for '{}'", __FUNCTION__, text.c_str());
	speechMarksRequest.SetOutputFormat(OutputFormat::json);
	speechMarksRequest.SetVoiceId(m_vVoiceId);
	speechMarksRequest.SetText(text);
	speechMarksRequest.AddSpeechMarkTypes(SpeechMarkType::word);
	if (Aws::Utils::StringUtils::ToLower(text.c_str()).find("<speak") == 0)
	{
		m_logger->debug("Text type = ssml");
		speechMarksRequest.SetTextType(TextType::ssml);
	}
	else
	{
		m_logger->debug("Text type = text");
		speechMarksRequest.SetTextType(TextType::text);
	}
	speechMarksRequest.SetSampleRate("16000");
	auto speech_marks = p.SynthesizeSpeech(speechMarksRequest);
	if (!speech_marks.IsSuccess())
	{
		std::stringstream error;
		//error << "Unable to generate speech marks: " << speech_marks.GetError().GetMessageW();
		response.ErrorMessage = error.str();
		return response;
	}
	auto &m = speech_marks.GetResult();
	auto& m_stream = m.GetAudioStream();
	std::string json_str;
	std::vector<SpeechMark> speechMarks;
	auto firstWord = true;
	long bytesProcessed = 0;
	m_logger->debug("SpeechMarks response:\n\n{}\n\n", json_str);
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
		m_logger->debug("Word: {}, Start: {}, End: {}, Time: {}\n", sm.Text.c_str(), sm.StartInMs,
			sm.EndByte,
			sm.TimeInMs);
		speechMarks.push_back(sm);
		firstWord = false;
	}
	auto sm = speechMarks[speechMarks.size() - 1];
	sm.LengthInBytes = streamSize - bytesProcessed;
	sm.TimeInMs = sm.LengthInBytes / 32;
	speechMarks[speechMarks.size() - 1] = sm;
	m_logger->debug("Word: {}, Start: {}, End: {}, Time: {}\n", sm.Text.c_str(), sm.StartInMs,
		sm.EndByte,
		sm.TimeInMs);
	m_logger->debug("Total words generated: {}", speechMarks.size());
	speechMarks.push_back(sm);
	response.SpeechMarks = speechMarks;
	return response;
}
