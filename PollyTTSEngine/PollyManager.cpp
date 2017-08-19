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

void PollyManager::GenerateSpeech(const wchar_t* text) const
{
	Aws::Polly::PollyClient p;
	LogUtils log;
	Aws::Polly::Model::SynthesizeSpeechRequest speech_request;
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
		MessageBoxA(NULL, error.str().c_str(), "AWS Error", 0);
	}
	auto &r = speech.GetResult();

	auto& stream = r.GetAudioStream();
	std::streamsize amountRead(0);
	unsigned char buffer[1000000];
	stream.read((char*)buffer, 1000000);
	auto read = stream.gcount();
}
