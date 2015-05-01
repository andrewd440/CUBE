#include "..\Include\Windows\WindowsFile.h"
#include <iostream>
#include "Shlwapi.h"

namespace
{
	void PrintError()
	{
		LPTSTR Error = NULL;
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS
			, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&Error, 0, NULL);

		std::wcerr << Error << std::endl;

		LocalFree(Error);
	}
}

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
	
	PrintError();
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

	PrintError();

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
		PrintError();
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
	SetProgramDirectory();
}

std::unique_ptr<IFileHandle> FWindowsFileSystem::OpenWritable(const wchar_t* Filename, const bool AllowShareRead, const bool CreateNew)
{
	DWORD Access = GENERIC_WRITE;
	DWORD ShareMode = AllowShareRead ? FILE_SHARE_READ : 0;
	DWORD Creation = CreateNew ? CREATE_ALWAYS : OPEN_EXISTING;

	HANDLE FileHandle = CreateFile(Filename, Access, ShareMode, nullptr, Creation, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (FileHandle != INVALID_HANDLE_VALUE)
	{
		return std::make_unique<FWindowsHandle>(FileHandle);
	}

	PrintError();
	return nullptr;
}

std::unique_ptr<IFileHandle> FWindowsFileSystem::OpenReadable(const wchar_t* Filename)
{
	DWORD Access = GENERIC_READ;
	DWORD ShareMode = FILE_SHARE_READ;

	HANDLE FileHandle = CreateFile(Filename, Access, ShareMode, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	
	if (FileHandle != INVALID_HANDLE_VALUE)
	{
		return std::make_unique<FWindowsHandle>(FileHandle);
	}

	PrintError();
	return nullptr;
}

std::unique_ptr<IFileHandle> FWindowsFileSystem::OpenReadWritable(const wchar_t* FileName, const bool AllowShareRead, const bool CreateNew)
{
	DWORD Access = GENERIC_WRITE | GENERIC_READ;
	DWORD ShareMode = AllowShareRead ? FILE_SHARE_READ : 0;
	DWORD Creation = CreateNew ? CREATE_ALWAYS : OPEN_EXISTING;

	HANDLE FileHandle = CreateFile(FileName, Access, ShareMode, nullptr, Creation, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (FileHandle != INVALID_HANDLE_VALUE)
	{
		return std::make_unique<FWindowsHandle>(FileHandle);
	}

	PrintError();
	return nullptr;
}

bool FWindowsFileSystem::DeleteFilename(const wchar_t* Filename)
{
	if (DeleteFile(Filename))
		return true;
	else
	{
		PrintError();
	}

	return false;
}

bool FWindowsFileSystem::CurrentDirectory(wchar_t* DataOut, const uint32_t BufferLength)
{
	const DWORD DataWritten = GetCurrentDirectory(BufferLength, DataOut);
	if (DataWritten == BufferLength)
		return true;

	PrintError();
	return false;
}

bool FWindowsFileSystem::DeleteDirectory(const wchar_t* DirectoryName)
{
	SHFILEOPSTRUCT FileOp;
	FileOp.wFunc = FO_DELETE;

	// From and To must be double-null terminated.
	wchar_t NameBuffer[100];
	wcscpy(NameBuffer, DirectoryName);
	memcpy(NameBuffer + wcslen(NameBuffer), "\0\0", 2 * sizeof(wchar_t));
	FileOp.pFrom = NameBuffer;

	FileOp.fFlags = FOF_NO_UI;
	int Error = SHFileOperation(&FileOp);
	if (Error == 0)
	{
		return true;
	}

	std::wcerr << "Delete directory operation failded on " << DirectoryName << " with code: " << Error << std::endl;
	return false;
}

bool FWindowsFileSystem::RenameDirectory(const wchar_t* CurrentName, const wchar_t* NewName)
{
	SHFILEOPSTRUCT FileOp;
	FileOp.wFunc = FO_RENAME;

	// From and To must be double-null terminated.
	wchar_t CurrentBuffer[100];
	wcscpy(CurrentBuffer, CurrentName);
	memcpy(CurrentBuffer + wcslen(CurrentBuffer), "\0\0", 2 * sizeof(wchar_t));
	FileOp.pFrom = CurrentBuffer;

	wchar_t ToBuffer[100];
	wcscpy(ToBuffer, NewName);
	memcpy(ToBuffer + wcslen(ToBuffer), "\0\0", 2 * sizeof(wchar_t));
	FileOp.pTo = ToBuffer;

	FileOp.fFlags = FOF_NO_UI;
	if (SHFileOperation(&FileOp) == 0)
	{
		return true;
	}

	PrintError();
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
		if (GetLastError() == ERROR_PATH_NOT_FOUND)
			std::wcerr << L"Directory path not found when creating directory." << std::endl;
	}

	return false;
}

bool FWindowsFileSystem::GetProgramDirectory(wchar_t* DataOut, const uint32_t BufferLength)
{
	std::memcpy(DataOut, ProgramDirectory, min(BufferLength, ProgramDirectorySize));
	return BufferLength < ProgramDirectorySize;
}

bool FWindowsFileSystem::SetDirectory(const wchar_t* DirectoryName)
{
	if (SetCurrentDirectory(DirectoryName) != 0)
	{
		return true;
	}
	
	PrintError();
	return false;
}

bool FWindowsFileSystem::FileExists(const wchar_t* Filename)
{
	return !(INVALID_FILE_ATTRIBUTES == GetFileAttributes(Filename) && GetLastError() == ERROR_FILE_NOT_FOUND);
}

bool FWindowsFileSystem::SetToProgramDirectory()
{
	if (SetCurrentDirectory(ProgramDirectory) != 0)
	{
		return true;
	}

	PrintError();
	return false;
}

void FWindowsFileSystem::SetProgramDirectory()
{
	ProgramDirectorySize = GetModuleFileName(NULL, ProgramDirectory, PROGRAM_DIRECTORY_CAP);
	if (ProgramDirectorySize < PROGRAM_DIRECTORY_CAP)
	{
		PathRemoveFileSpec(ProgramDirectory);
		return;
	}

	PrintError();
}

bool FWindowsFileSystem::CopyFileDirectory(const wchar_t* From, const wchar_t* To)
{
	SHFILEOPSTRUCT FileOp;
	FileOp.wFunc = FO_COPY;

	// From and To must be double-null terminated.
	wchar_t FromBuffer[100];
	wcscpy(FromBuffer, From);
	memcpy(FromBuffer + wcslen(FromBuffer), "\0\0", 2 * sizeof(wchar_t));
	FileOp.pFrom = FromBuffer;

	wchar_t ToBuffer[100];
	wcscpy(ToBuffer, To);
	memcpy(ToBuffer + wcslen(ToBuffer), "\0\0", 2 * sizeof(wchar_t));
	FileOp.pTo = ToBuffer;

	FileOp.fFlags = FOF_NO_UI;
	if (SHFileOperation(&FileOp) == 0)
	{
		return true;
	}

	PrintError();
	return false;
}