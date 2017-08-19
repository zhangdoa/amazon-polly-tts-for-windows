#include "stdafx.h"
#include "PollyManager.h"
#include "LogUtils.h"
#include <aws/polly/PollyClient.h>
#include <aws/polly/model/OutputFormat.h>
#include <aws/polly/model/TextType.h>
#include <aws/polly/model/SynthesizeSpeechRequest.h>
#include <aws/core/utils/Outcome.h>
#ifdef _WIN32
#include <Windows.h>
#endif

PollySpeechResponse PollyManager::GenerateSpeech(const wchar_t* text)
{
	Aws::Polly::PollyClient p;
	LogUtils log;
	Aws::Polly::Model::SynthesizeSpeechRequest speech_request;
	PollySpeechResponse response;
	Aws::String speech_text = Aws::Utils::StringUtils::FromWString(text);
	log.Debug("%s: Asking Polly for '%s'", __FUNCTION__, speech_text.c_str());
	speech_request.SetOutputFormat(Aws::Polly::Model::OutputFormat::pcm);
	speech_request.SetVoiceId(Aws::Polly::Model::VoiceId::Brian);
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
	stream.read(reinterpret_cast<char*>(response.AudioData), 1000000);
	response.Length = stream.gcount();
	return response;
}
