#pragma once
#include "Singleton.h"
#include "GenericFile.h"
#include "Windows.h"

#include <cstdint>

/**
* Wrapper class for file handle operations on the Windows platform.
*/
class FWindowsHandle : public IFileHandle
{
public:
	/**
	* Constructs a Windows file handle.
	*/
	FWindowsHandle(HANDLE FileHandle);
	~FWindowsHandle();

	bool Read(uint8_t* DataOut, uint32_t NumBytesToRead) override;
	bool Write(const uint8_t* Data, const uint32_t NumBytesToWrite) override;
	uint32_t GetFileSize() const override;


private:
	HANDLE mFileHandle;
};

/**
* Wrapper class for file operations on the Windows platform.
*/
class FWindowsFile : public IFile
{
public:
	FWindowsFile();
	~FWindowsFile() = default;

	std::unique_ptr<IFileHandle> OpenWritable(const wchar_t* FileName, const bool AllowRead = false, const bool CreateNew = false) override;
	std::unique_ptr<IFileHandle> OpenReadable(const wchar_t* Filename) override;
};