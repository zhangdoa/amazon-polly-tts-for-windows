#pragma once
#include <string>

class PollySpeechResponse
{
public:
	std::streamsize Length;
	unsigned char AudioData[1000000];
	std::string ErrorMessage;
};
