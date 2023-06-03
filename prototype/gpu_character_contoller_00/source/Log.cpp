#include "pch.h"

#include "Log.h"

namespace
{
	const std::string FormatString(const char* const pFormat, va_list vaArgs)
	{
		// reliably acquire the size
		// from a copy of the variable argument array
		// and a functionally reliable call to mock the formatting
		va_list vaArgsCopy;
		va_copy(vaArgsCopy, vaArgs);
		const int iLen = std::vsnprintf(NULL, 0, pFormat, vaArgsCopy);
		va_end(vaArgsCopy);

		// return a formatted string without risking memory mismanagement
		// and without assuming any compiler or platform specific behavior
		std::vector<char> zc(iLen + 1);
		std::vsnprintf(zc.data(), zc.size(), pFormat, vaArgs);

		std::string message(zc.data(), iLen); 

		return message + "\n";
	}
}

//https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf/8098080
void Log::AddMessage(const LogTopic topic, const char* const pFormat, ... )
{
	va_list vaArgs;
	va_start(vaArgs, pFormat);
	const std::string message = FormatString(pFormat, vaArgs);
	va_end(vaArgs);

	OutputDebugStringA(message.c_str());
}
