#include "FileIO\WorldFileSystem.h"

const wchar_t FWorldFileSystem::TEMP_DIRECTORY_NAME[] = L"Temp_World";
const wchar_t FWorldFileSystem::WORLDS_DIRECTORY_NAME[] = L"./Worlds/";
const wchar_t FWorldFileSystem::TEMP_DIRECTORY_PATH[] = L"./Worlds/Temp_World";

FWorldFileSystem::FWorldFileSystem()
	: mWorldName()
	, mRegionFiles()
	, mWorldSize(0)
{

}

FWorldFileSystem::~FWorldFileSystem()
{
	IFileSystem& FileSystem = IFileSystem::GetInstance();
	mRegionFiles.clear();

	// Delete the temp directory
	std::wstring TempPath{ TEMP_DIRECTORY_PATH };
	FileSystem.DeleteDirectory(TempPath.c_str());
}

bool FWorldFileSystem::SetWorld(const wchar_t* WorldName)
{
	mRegionFiles.clear();
	mWorldName = WorldName;

	IFileSystem& FileSystem = IFileSystem::GetInstance();

	// Copy world into temp directory
	std::wstring Filepath{ WORLDS_DIRECTORY_NAME };
	Filepath += WorldName;

	std::wstring TempPath{ TEMP_DIRECTORY_PATH };

	FileSystem.DeleteDirectory(TempPath.c_str());
	FileSystem.CopyFileDirectory(Filepath.c_str(), TempPath.c_str());

	// Get the world size
	TempPath += L"/WorldInfo.vgw";

	auto WorldInfoFile = FileSystem.OpenReadable(TempPath.c_str());
	
	if (WorldInfoFile)
	{
		WorldInfoFile->Read((uint8_t*)&mWorldSize, 4);
		return true;
	}
	
	std::wcerr << L"World file could not be found for " << WorldName << std::endl;
	return false;
}

uint32_t FWorldFileSystem::GetWorldSize() const
{
	return mWorldSize;
}

std::wstring FWorldFileSystem::GetWorldName() const
{
	return mWorldName;
}

void FWorldFileSystem::SaveWorld()
{
	// Delete the original world directory
	IFileSystem& FileSystem = IFileSystem::GetInstance();
	std::wstring Filepath{ WORLDS_DIRECTORY_NAME };
	Filepath += mWorldName;

	FileSystem.DeleteDirectory(Filepath.c_str());

	// Copy temp world into a new world directory
	std::wstring TempPath{ WORLDS_DIRECTORY_NAME };
	TempPath += TEMP_DIRECTORY_NAME;
	FileSystem.CopyFileDirectory(TempPath.c_str(), Filepath.c_str());
}

void FWorldFileSystem::AddRegionFileReference(const Vector3i& ChunkPosition)
{
	// Get region position info
	const Vector3i RegionID = FRegionFile::ChunkToRegionPosition(ChunkPosition);

	// Increment if the file is loaded
	if (mRegionFiles.find(RegionID) != mRegionFiles.end())
	{
		mRegionFiles[RegionID].ReferenceCount++;
	}
	else
	{
		// Add the file if not loaded
		RegionFileRecord& Record = mRegionFiles[RegionID];
		Record.ReferenceCount = 1;
		Record.File.Load(TEMP_DIRECTORY_NAME, RegionID);
	}
}

void FWorldFileSystem::RemoveRegionFileReference(const Vector3i& ChunkPosition)
{
	// Get region position info
	const Vector3i RegionID = FRegionFile::ChunkToRegionPosition(ChunkPosition);

	ASSERT(mRegionFiles.find(RegionID) != mRegionFiles.end() && "Shouldn't be removing a record that is not there.");

	// Decrement reference count and erase if 0
	RegionFileRecord& Record = mRegionFiles[RegionID];
	Record.ReferenceCount--;

	if (Record.ReferenceCount <= 0)
	{
		mRegionFiles.erase(RegionID);
	}
}

void FWorldFileSystem::GetChunkData(const Vector3i& ChunkPosition, std::vector<uint8_t>& DataOut)
{
	const Vector3i RegionID = FRegionFile::ChunkToRegionPosition(ChunkPosition);
	const Vector3i RegionPosition = FRegionFile::LocalRegionPosition(ChunkPosition);

	ASSERT(mRegionFiles.find(RegionID) != mRegionFiles.end());

	FRegionFile& File = mRegionFiles[RegionID].File;

	// Get size and offset
	uint32_t DataSize, SectorOffset;
	File.GetChunkDataInfo(RegionPosition, DataSize, SectorOffset);

	// Fill data buffer
	DataOut.resize(DataSize);
	File.GetChunkData(SectorOffset, DataOut.data(), DataSize);

}

void FWorldFileSystem::WriteChunkData(const Vector3i& ChunkPosition, const std::vector<uint8_t>& Data)
{
	const Vector3i RegionID = FRegionFile::ChunkToRegionPosition(ChunkPosition);
	const Vector3i RegionPosition = FRegionFile::LocalRegionPosition(ChunkPosition);

	ASSERT(mRegionFiles.find(RegionID) != mRegionFiles.end());

	FRegionFile& File = mRegionFiles[RegionID].File;
	File.WriteChunkData(RegionPosition, Data.data(), Data.size());

}