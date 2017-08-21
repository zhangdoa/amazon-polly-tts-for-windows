#pragma once
#include "PollySpeechResponse.h"
#include "PollySpeechMarksResponse.h"
class CSentItem;

class PollyManager
{
public:
	static PollySpeechResponse GenerateSpeech(CSentItem& item);
	static PollySpeechMarksResponse PollyManager::GenerateSpeechMarks(CSentItem& item, std::streamsize streamSize);
};
