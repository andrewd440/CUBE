#pragma once

#include <cstdio>
#include <cstdint>

#ifdef _WIN32
#include "Windows.h"
#endif

/**
* Namespace for debugging utility functions.
*/
namespace FDebug
{
	/**
	* Prints a formatted output to a debug window. If arguments
	* are not in the form of va_list, use FDebug::PrintF for a more
	* convenient debug print function.
	* @param Format - PrintF like format for the output string.
	* @param ArgList - List of the printed arguments.
	* @return The number of chars written.
	*/
	int32_t VPrintF(const char* Format, va_list ArgList);

	/**
	* Prints a formatted output to a debug window.
	* @param Format - PrintF like format for the output string.
	* @param ... - Variable length of arguments.
	* @return The number of chars written.
	*/
	int32_t PrintF(const char* Format, ...);
}