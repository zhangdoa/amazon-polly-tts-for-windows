#pragma once
#include <string.h>
#include <string>

class SapiToPolly
{
public:
	SapiToPolly(const char* xml);
	void ToPolly();
	void Print();
};
