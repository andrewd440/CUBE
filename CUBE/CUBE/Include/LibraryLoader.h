#pragma once

#include "Utils\Singleton.h"

class ILibraryLoader : public TSingleton<ILibraryLoader>
{
public:

	ILibraryLoader() = default;
	virtual ~ILibraryLoader() = default;

	/**
	* Sets the directory for the application to search for DLLs.
	* @return True if the call succeeded.
	*/
	virtual bool SetDLLDirectory(const wchar_t* Directory) = 0;
};

