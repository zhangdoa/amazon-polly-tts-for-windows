#pragma once
#include <string.h>
#include <string>

class SpeechMark
{
public:
	SpeechMark();
	~SpeechMark();
	int TimeInMs;
	int StartByte;
	int EndByte;
	int StartInMs;
	long LengthInBytes;
	std::string Text;
};

