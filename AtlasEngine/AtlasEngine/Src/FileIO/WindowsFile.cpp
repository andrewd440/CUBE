#include "..\Include\FileIO/WindowsFile.h"
#include <iostream>

FWindowsHandle::FWindowsHandle(HANDLE FileHandle)
	: mFileHandle(FileHandle)
{
	ASSERT(mFileHandle != INVALID_HANDLE_VALUE && "FWindowsHandle constructed with invalid handle.");
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

bool FWindowsHandle::Seek(const uint64_t Distance)
{
	return FileSeek(Distance, FILE_CURRENT);
}

bool FWindowsHandle::SeekFromEnd(const uint64_t Distance)
{
	return FileSeek(Distance, FILE_END);
}

bool FWindowsHandle::SeekFromStart(const uint64_t Distance)
{
	return FileSeek(Distance, FILE_BEGIN);
}

bool FWindowsHandle::FileSeek(const uint64_t Distance, const DWORD MoveMethod)
{
	LARGE_INTEGER Li;
	Li.QuadPart = Distance;
	Li.LowPart = SetFilePointer(mFileHandle, Li.LowPart, &Li.HighPart, MoveMethod);

	if (Li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
	{
		return false;
	}

	return true;
}

uint32_t FWindowsHandle::GetFileSize() const
{
	return ::GetFileSize(mFileHandle, nullptr);
}

FWindowsHandle::~FWindowsHandle()
{
	CloseHandle(mFileHandle);
	mFileHandle = nullptr;
};

FWindowsFileSystem::FWindowsFileSystem()
	: IFileSystem()
{

}

std::unique_ptr<IFileHandle> FWindowsFileSystem::OpenWritable(const wchar_t* Filename, const bool AllowRead, const bool CreateNew)
{
	DWORD Access = GENERIC_WRITE;
	DWORD ShareMode = AllowRead ? FILE_SHARE_READ : 0;
	DWORD Creation = CreateNew ? CREATE_ALWAYS : OPEN_EXISTING;

	HANDLE FileHandle = CreateFile(Filename, Access, ShareMode, nullptr, Creation, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (FileHandle == INVALID_HANDLE_VALUE)
		return nullptr;

	return std::make_unique<FWindowsHandle>(FileHandle);
}

std::unique_ptr<IFileHandle> FWindowsFileSystem::OpenReadable(const wchar_t* Filename)
{
	DWORD Access = GENERIC_READ;
	DWORD ShareMode = FILE_SHARE_READ;

	HANDLE FileHandle = CreateFile(Filename, Access, ShareMode, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (FileHandle == INVALID_HANDLE_VALUE)
		return nullptr;

	return std::make_unique<FWindowsHandle>(FileHandle);
}

bool FWindowsFileSystem::DeleteFilename(const wchar_t* Filename)
{
	if (DeleteFile(Filename))
		return true;
	else
	{
		const DWORD Error = GetLastError();
		if (Error == ERROR_FILE_NOT_FOUND)
		{
			std::wcerr << L"Attempt to delete file: " << Filename << L" not found." << std::endl;
		}
		else if (Error == ERROR_ACCESS_DENIED)
		{
			std::wcerr << L"Attempt to delete file: " << Filename << L" access denied." << std::endl;
		}
	}

	return false;
}

bool FWindowsFileSystem::CurrentDirectory(wchar_t* DataOut, const uint32_t BufferLength)
{
	const DWORD DataWritten = GetCurrentDirectory(BufferLength, DataOut);
	if (DataWritten == BufferLength)
		return true;

	return false;
}

bool FWindowsFileSystem::DeleteDirectory(const wchar_t* DirectoryName)
{
	if (RemoveDirectory(DirectoryName))
		return true;
	else
	{
		const DWORD Error = GetLastError();
		if (Error == ERROR_DIR_NOT_EMPTY)
		{
			std::wcerr << L"Attempt to delete directory: " << DirectoryName << L" not empty" << std::endl;
		}
		else if (Error == ERROR_PATH_NOT_FOUND)
		{
			std::wcerr << L"Attempt to delete directory: " << DirectoryName << L" not found." << std::endl;
		}
	}

	return false;
}

bool FWindowsFileSystem::CreateFileDirectory(const wchar_t* DirectoryName)
{
	if (CreateDirectory(DirectoryName, nullptr))
	{
		return true;
	}
	else
	{
		const DWORD Error = GetLastError();
		if (Error == ERROR_ALREADY_EXISTS)
		{
			std::wcerr << L"Attempt to create directory: " << DirectoryName << L" already exists." << std::endl;
		}
		else if (Error == ERROR_PATH_NOT_FOUND)
		{
			std::wcerr << L"Attempt to create directory: " << DirectoryName << L" path not found." << std::endl;
		}
	}

	return false;
}