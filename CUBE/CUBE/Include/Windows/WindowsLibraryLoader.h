#pragma once
#include "LibraryLoader.h"

class FWindowsLibraryLoader : public ILibraryLoader
{
public:
	FWindowsLibraryLoader();
	~FWindowsLibraryLoader();

	bool SetDLLDirectory(const wchar_t* Directory) override;
};

using FLibraryLoader = FWindowsLibraryLoader;