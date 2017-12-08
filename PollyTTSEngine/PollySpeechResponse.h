#pragma once
#include <vector>

class PollySpeechResponse
{
public:
	std::streamsize Length;
	std::vector<unsigned char> AudioData = std::vector<unsigned char>(5000000);
	std::string ErrorMessage ;
	bool IsSuccess;
};
