#pragma once
#include <cstdint>
#include <memory>

#include "Singleton.h"

/**
* Interface for platform file handles.
*/
class IFileHandle
{
public:
	IFileHandle() = default;
	
	virtual ~IFileHandle(){};

	/**
	* Reads a specific amount of data from a file.
	* @param DataOut Buffer for data to be written.
	* @param NumBytesToRead Number of bytes to read from the file.
	* @return False if the number of bytes to read counld not be read.
	*/
	virtual bool Read(uint8_t* DataOut, uint32_t NumBytesToRead) = 0;
	
	/**
	* Writes a specific amount of data to a file.
	* @param Data to write.
	* @param NumBytesToWrite Number of bytes to write to the file.
	* @return False if the number of bytes to write could not be written.
	*/
	virtual bool Write(const uint8_t* Data, const uint32_t NumBytesToWrite) = 0;

	/**
	* Seeks the current file pointer from it's current position to
	* some specified distance.
	* @param Distance to seek.
	* @return True if the seek succeeded.
	*/
	virtual bool Seek(const uint64_t Distance) = 0;

	/**
	* Seeks the current file pointer from the end of the file
	* some specified distance.
	* @param Distance to seek.
	* @return True if the seek succeeded.
	*/
	virtual bool SeekFromEnd(const uint64_t Distance) = 0;

	/**
	* Seeks the current file pointer from the beginning of the file to
	* some specified distance.
	* @param Distance to seek.
	* @return True if the seek succeeded.
	*/
	virtual bool SeekFromStart(const uint64_t Distance) = 0;

	/**
	* Retrieves the size of this file.
	*/
	virtual uint32_t GetFileSize() const = 0;
};

/**
* Interface for platform files.
*/
class IFileSystem : public TSingleton<IFileSystem>
{
protected:
	static const uint32_t PROGRAM_DIRECTORY_CAP = 300;
	static uint32_t ProgramDirectorySize;
	static wchar_t ProgramDirectory[PROGRAM_DIRECTORY_CAP];

public:
	IFileSystem()
		: TSingleton()
	{}

	virtual ~IFileSystem() {};

	/**
	* Opens or creates a file in the current working directory.
	* @param Filename to open.
	* @param AllowRead True if share read access is required.
	* @param CreateNew True if a new file should be created, only if one doesn't exist.
	* @return A pointer to a handle for the opened file. Nullptr if the open/create
	* operation failed.
	*/
	virtual std::unique_ptr<IFileHandle> OpenWritable(const wchar_t* FileName, const bool AllowRead = false, const bool CreateNew = false) = 0;

	/**
	* Opens or creates a file in the current working directory.
	* @param Filename to open.
	* @param AllowRead True if read access is required.
	* @param CreateNew True if a new file should be created.
	* @return A pointer to a handle for the opened file. Nullptr if the open/create
	* operation failed.
	*/
	virtual std::unique_ptr<IFileHandle> OpenReadable(const wchar_t* Filename) = 0;
	
	/**
	* Opens or creates a file in the current working directory.
	* @param Filename to open.
	* @param AllowRead True if share read access is required.
	* @param CreateNew True if a new file should be created, only if one doesn't exist.
	* @return A pointer to a handle for the opened file. Nullptr if the open/create
	* operation failed.
	*/
	virtual std::unique_ptr<IFileHandle> OpenReadWritable(const wchar_t* FileName, const bool AllowRead = false, const bool CreateNew = false) = 0;

	/**
	* Deletes a file.
	* @param Filename Name of the file to delete.
	* @return True if the delete succeeded.
	*/
	virtual bool DeleteFilename(const wchar_t* Filename) = 0;

	/**
	* Retrieves the current working file directory.
	* @param DataOut Location for the directory to be written.
	* @param BufferLength The size of DataOut
	* @return True if the complete directory was written to DataOut. False if
	*		DataOut didn't have enough space to hold the directory.
	*/
	virtual bool CurrentDirectory(wchar_t* DataOut, const uint32_t BufferLength) = 0;

	/**
	* Deletes a file directory.
	* @param DirectoryName Directory to delete.
	* @return True if the delete succeeded.
	*/
	virtual bool DeleteDirectory(const wchar_t* DirectoryName) = 0;

	/**
	* Renames a file directory.
	* @param CurrentName - Directory to rename.
	* @param NewName - New name for the directory.
	* @return True if the rename succeeded.
	*/
	virtual bool RenameDirectory(const wchar_t* CurrentName, const wchar_t* NewName) = 0;

	/**
	* Creates a file directory.
	* @param DirectoryName Directory to create.
	* @return True if the creation succeeded.
	*/
	virtual bool CreateFileDirectory(const wchar_t* DirectoryName) = 0;

	/**
	* Retrieves the directory of the program .exe.
	* @param DataOut Location for the directory to be written.
	* @param BufferLength The size of DataOut
	* @return True if the complete directory was written to DataOut. False if
	*		DataOut didn't have enough space to hold the directory.
	*/
	virtual bool GetProgramDirectory(wchar_t* DataOut, const uint32_t BufferLength) = 0;

	/**
	* Changes the current working directory.
	* @param DirectoryName - Directory to go to.
	* @return True if the directory was entered.
	*/
	virtual bool SetDirectory(const wchar_t* DirectoryName) = 0;

	/**
	* Checks if a file exists.
	* @param Filename - File to check.
	* @return True if the file exists.
	*/
	virtual bool FileExists(const wchar_t* Filename) = 0;

	/**
	* Sets the current working directory to the directory of
	* the running program .exe.
	* @return True if the directory successfully changed.
	*/
	virtual bool SetToProgramDirectory() = 0;

	/**
	* Copy to entire contents of a file directory to another location.
	* @param From - The directory to be copied.
	* @param To - The directory to copy to.
	* @return True if the copy was successful.
	*/
	virtual bool CopyFileDirectory(const wchar_t* From, const wchar_t* To) = 0;
};