#pragma once
#include "PollySpeechResponse.h"
class PollyManager
{
public:
	static PollySpeechResponse GenerateSpeech(const wchar_t* text);
};