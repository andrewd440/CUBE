#include "FileIO\WorldFileSystem.h"


FWorldFileSystem::FWorldFileSystem()
	: mWorldName()
	, mRegionFiles()
	, mWorldSize(0)
{

}

FWorldFileSystem::~FWorldFileSystem()
{

}

void FWorldFileSystem::SetWorld(const wchar_t* WorldName)
{
	mWorldName = WorldName;

	IFileSystem& FileSystem = IFileSystem::GetInstance();
	std::wstring Filepath{ WORLDS_DIRECTORY_NAME };
	Filepath += WorldName;

	Filepath += L"/WorldInfo.vgw";

	auto WorldInfoFile = FileSystem.OpenReadable(Filepath.c_str());
	WorldInfoFile->Read((uint8_t*)&mWorldSize, 4);
	ASSERT((mWorldSize & (mWorldSize - 1)) == 0x0 && "World size must be a power of two.");
}

uint32_t FWorldFileSystem::GetWorldSize() const
{
	return mWorldSize;
}

void FWorldFileSystem::SaveWorld()
{

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

	FRegionFile& File = mRegionFiles.at(RegionID).File;

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

	FRegionFile& File = mRegionFiles.at(RegionID).File;
	File.WriteChunkData(RegionPosition, Data.data(), Data.size());

}