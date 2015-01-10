#include "..\Include\FileIO/WindowsFile.h"


FWindowsHandle::FWindowsHandle(HANDLE FileHandle)
	: mFileHandle(FileHandle)
{

}

bool FWindowsHandle::Read(uint8_t* DataOut, uint32_t NumBytesToRead)
{
	DWORD BytesRead = 0;

	if (ReadFile(mFileHandle, DataOut, NumBytesToRead, &BytesRead, nullptr))
	{
		if (NumBytesToRead == BytesRead)
			return true;
	}

	return false;
}

bool FWindowsHandle::Write(const uint8_t* Data, const uint32_t NumBytesToWrite)
{
	DWORD BytesWritten = 0;

	if (WriteFile(mFileHandle, Data, NumBytesToWrite, &BytesWritten, nullptr))
	{
		if (BytesWritten == NumBytesToWrite)
			return true;
	}

	return false;
}

uint32_t FWindowsHandle::GetFileSize() const
{
	return ::GetFileSize(mFileHandle, nullptr);
}

FWindowsFile::FWindowsFile()
	: IFile()
{

}

FWindowsHandle::~FWindowsHandle()
{
	CloseHandle(mFileHandle);
	mFileHandle = nullptr;
};

std::unique_ptr<IFileHandle> FWindowsFile::OpenWritable(const wchar_t* Filename, const bool AllowRead, const bool CreateNew)
{
	DWORD Access = GENERIC_WRITE;
	DWORD ShareMode = AllowRead ? FILE_SHARE_READ : 0;
	DWORD Creation = CreateNew ? CREATE_ALWAYS : OPEN_EXISTING;

	HANDLE FileHandle = CreateFile(Filename, Access, ShareMode, nullptr, Creation, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (FileHandle == INVALID_HANDLE_VALUE)
		return nullptr;

	return std::make_unique<FWindowsHandle>(FileHandle);
}

std::unique_ptr<IFileHandle> FWindowsFile::OpenReadable(const wchar_t* Filename)
{
	DWORD Access = GENERIC_READ;
	DWORD ShareMode = FILE_SHARE_READ;

	HANDLE FileHandle = CreateFile(Filename, Access, ShareMode, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (FileHandle == INVALID_HANDLE_VALUE)
		return nullptr;

	return std::make_unique<FWindowsHandle>(FileHandle);
}