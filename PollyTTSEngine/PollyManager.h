#pragma once
#include "PollySpeechResponse.h"
class CSentItem;

class PollyManager
{
public:
	static PollySpeechResponse GenerateSpeech(CSentItem& item);
};
