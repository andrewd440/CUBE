#pragma once

#include "..\FileIO\GenericFile.h"
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
	FWindowsHandle(HANDLE FileHandle = INVALID_HANDLE_VALUE);

	~FWindowsHandle();

	FWindowsHandle& operator=(const FWindowsHandle& Other);

	bool Read(uint8_t* DataOut, uint32_t NumBytesToRead) override;

	bool Write(const uint8_t* Data, const uint32_t NumBytesToWrite) override;

	bool Seek(const uint64_t Distance) override;

	bool SeekFromEnd(const uint64_t Distance) override;

	bool SeekFromStart(const uint64_t Distance) override;

	uint32_t GetFileSize() const override;

private:
	/**
	* Moves the current file pointer a specified distance based on
	* and specified move method.
	* @return True if the seek succeeded.
	*/
	bool FileSeek(const uint64_t Distance, const DWORD MoveMethod);

private:
	HANDLE mFileHandle;
};


/**
* Wrapper class for file operations on the Windows platform.
*/
class FWindowsFileSystem : public IFileSystem
{
public:
	FWindowsFileSystem();
	~FWindowsFileSystem() = default;

	std::unique_ptr<IFileHandle> OpenWritable(const wchar_t* FileName, const bool AllowShareRead = false, const bool CreateNew = false) override;
	std::unique_ptr<IFileHandle> OpenReadable(const wchar_t* Filename) override;
	std::unique_ptr<IFileHandle> OpenReadWritable(const wchar_t* FileName, const bool AllowRead = false, const bool CreateNew = false) override;

	bool DeleteFilename(const wchar_t* Filename) override;

	bool CurrentDirectory(wchar_t* DataOut, const uint32_t BufferLength) override;

	bool DeleteDirectory(const wchar_t* DirectoryName) override;

	bool RenameDirectory(const wchar_t* CurrentName, const wchar_t* NewName) override;

	bool CreateFileDirectory(const wchar_t* DirectoryName) override;

	bool GetProgramDirectory(wchar_t* DataOut, const uint32_t BufferLength) override;

	bool SetDirectory(const wchar_t* DirectoryName) override;

	bool FileExists(const wchar_t* Filename) override;

	bool SetToProgramDirectory() override;

	bool CopyFileDirectory(const wchar_t* From, const wchar_t* To) override;

private:
	void SetProgramDirectory();
};

using FFileHandle = FWindowsHandle;
using FFileSystem = FWindowsFileSystem;