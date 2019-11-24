/*  Copyright 2017 - 2018 Amazon.com, Inc. or its affiliates.All Rights Reserved.
Licensed under the Amazon Software License(the "License").You may not use
this file except in compliance with the License.A copy of the License is
located at

http://aws.amazon.com/asl/

and in the "LICENSE" file accompanying this file.This file is distributed
on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, express
or implied.See the License for the specific language governing
permissions and limitations under the License. */

#pragma once
#include "aws/polly/model/Voice.h"	

using namespace Aws::Polly::Model;

class VoiceForSAPI
{
	static std::pair<int, const wchar_t*> GetVoiceHexValue(LanguageCode code);
public:
	std::wstring tokenKeyName;
	std::wstring langIndependentName;
	std::wstring langDependentName;
	WORD langid; 
	std::wstring gender = L"NOT_SET";
	std::wstring name;
	std::wstring voiceId;
	std::wstring languageText;
	std::wstring age;
	std::wstring vendor = L"Amazon";
	bool hasNeural = false;
	bool hasNewscasterStyle = false;

	VoiceForSAPI(const Aws::Polly::Model::Voice&, bool _isNeural, bool _isNews, bool _isConversational);
	void PrintVoice() const;
};