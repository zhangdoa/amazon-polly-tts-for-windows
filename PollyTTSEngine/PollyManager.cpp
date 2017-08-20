#include "stdafx.h"
#include "PollyManager.h"
#include "LogUtils.h"
#include <aws/polly/PollyClient.h>
#include <aws/polly/model/OutputFormat.h>
#include <aws/polly/model/TextType.h>
#include <aws/core/utils/Outcome.h>
#include <aws/polly/model/SynthesizeSpeechRequest.h>
#include "TtsEngObj.h"

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
