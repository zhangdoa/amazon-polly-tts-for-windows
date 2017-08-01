#include "stdafx.h"
#include "LogUtils.h"
#ifdef _WIN32
#include <Windows.h>
#endif

void LogUtils::Debug(char* pszFormat, ...)
{
		static char s_acBuf[2048]; // this here is a caveat!
		va_list args;
		va_start(args, pszFormat);
		vsprintf(s_acBuf, pszFormat, args);
		OutputDebugStringA(s_acBuf);
		va_end(args);
}
