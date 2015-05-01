#include "..\..\Include\Debugging\ConsoleOutput.h"

namespace FDebug
{
	int32_t VPrintF(const char* Format, va_list ArgList)
	{
		const uint32_t MAX_CHARS = 2048;
		static char DebugBuffer[MAX_CHARS];

		// Format the arglist with specific Format
		const int32_t CharsWritten = vsnprintf_s(DebugBuffer, MAX_CHARS, Format, ArgList);

		OutputDebugStringA(DebugBuffer);
		OutputDebugStringA("\n");

		return CharsWritten;
	}

	int32_t PrintF(const char* Format, ...)
	{
		va_list ArgList;
		va_start(ArgList, Format);

		int32_t CharsWritten = VPrintF(Format, ArgList);

		va_end(ArgList);
		return CharsWritten;
	}
}