#pragma once
#include <vector>
#include "SpeechMark.h"

class PollySpeechMarksResponse
{
public:
	std::vector<SpeechMark> SpeechMarks = std::vector<SpeechMark>(10000);
	std::string ErrorMessage;
};
