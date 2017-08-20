#pragma once
class SpeechMark
{
public:
	SpeechMark();
	~SpeechMark();
	int TimeInMs;
	int StartByte;
	int EndByte;
	int StartInBytes;
	int LengthInBytes;
};

