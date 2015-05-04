#include "Windows\WindowsLibraryLoader.h"
#include <Windows.h>

FWindowsLibraryLoader::FWindowsLibraryLoader()
{
}


FWindowsLibraryLoader::~FWindowsLibraryLoader()
{
}

bool FWindowsLibraryLoader::SetDLLDirectory(const wchar_t* Directory)
{
	return SetDllDirectory(Directory) != 0;
}