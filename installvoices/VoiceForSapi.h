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
	std::pair<int, const wchar_t*> GetVoiceHexValue(LanguageCode code);
	static wchar_t* AWSStringToWchar(Aws::String);
public:
	WCHAR * tokenKeyName;
	const WCHAR * langIndependentName;
	const WCHAR * langDependentName;
	WORD langid; 
	const WCHAR * gender = L"NOT_SET";
	const WCHAR * name;
	const WCHAR * voiceId;
	const WCHAR * languageText;
	const WCHAR * age;
	const WCHAR * vendor = L"Amazon";

	VoiceForSAPI(Aws::Polly::Model::Voice);
	void PrintVoice() const;
};