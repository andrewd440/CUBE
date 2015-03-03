#include "FileIO/RegionFile.h"
#include "Misc\Assertions.h"
#include <wchar.h>

namespace 
{
	uint32_t GetTableIndex(Vector3i Position)
	{
		const Vector3i PositionToIndex{ (int32_t)FRegionFile::RegionData::REGION_SIZE, (int32_t)FRegionFile::RegionData::REGION_SIZE * FRegionFile::RegionData::REGION_SIZE, 1 };
		return (uint32_t)Vector3i::Dot(Position, PositionToIndex);
	}
}

static const uint8_t FilePadding[sizeof(FRegionFile::RegionData)];

FRegionFile::FRegionFile()
	: mRegionFile()
{
}

FRegionFile::~FRegionFile()
{
	// Write lookup table data back to disk
	mRegionFile->SeekFromStart(0);
	mRegionFile->Write((uint8_t*)&mRegionData, sizeof(RegionData));
}

bool FRegionFile::Load(const wchar_t* WorldName, const Vector3i& RegionPosition)
{
	auto& FileSystem = IFileSystem::GetInstance();

	static const uint32_t DirectoryBufferSize = 300;

	// Get the program directory
	FileSystem.SetToProgramDirectory();
	FileSystem.SetDirectory(L"Worlds");

	// Enter the file directory for this region
	FileSystem.CreateFileDirectory(WorldName);
	FileSystem.SetDirectory(WorldName);

	wchar_t ProgramDirectory[DirectoryBufferSize];
	int32_t CharCount = swprintf(ProgramDirectory, DirectoryBufferSize, L"x%dy%dz%d.vgr", RegionPosition.x, RegionPosition.y, RegionPosition.z);
	ProgramDirectory[CharCount] = L'\0';

	if (FileSystem.FileExists(ProgramDirectory))
	{
		mRegionFile = FileSystem.OpenReadWritable(ProgramDirectory, true);
	}
	else
	{
		// If it doesn't exist, create it
		mRegionFile = FileSystem.OpenReadWritable(ProgramDirectory, true, true);

		// Add empty lookup table
		mRegionFile->Write(FilePadding, sizeof(RegionData));
	}

	mRegionFile->SeekFromStart(0);
	return mRegionFile->Read((uint8_t*)&mRegionData, sizeof(RegionData));
}

void FRegionFile::GetChunkDataInfo(const Vector3i& ChunkPosition, uint32_t& SizeOut, uint32_t& SectorOffsetOut)
{
	uint32_t TableIndex = GetTableIndex(ChunkPosition);

	// If number of sectors for the chunk is 0, the chunk is not in the file yet
	if (mRegionData.ChunkEntry[TableIndex].NumOfSectors <= 0)
	{
		SizeOut = 0;
		return;
	}

	SectorOffsetOut = mRegionData.ChunkEntry[TableIndex].Offset;
	mRegionFile->SeekFromStart(sizeof(RegionData) + SectorOffsetOut * RegionData::SECTOR_SIZE);
	mRegionFile->Read((uint8_t*)&SizeOut, 4);
}

void FRegionFile::GetChunkData(const uint32_t SectorOffset, uint8_t* DataOut, const uint32_t DataSize)
{
	ASSERT(DataSize != 0);

	// Add offset after region data block and offset for chunk data size (4 bytes)
	mRegionFile->SeekFromStart((sizeof(RegionData) + SectorOffset * RegionData::SECTOR_SIZE) + 4);
	mRegionFile->Read(DataOut, DataSize);
}

void FRegionFile::WriteChunkData(const Vector3i& ChunkPosition, const uint8_t* Data, const uint32_t DataSize)
{
	uint32_t TableIndex = GetTableIndex(ChunkPosition);
	LookupEntry& ChunkEntry = mRegionData.ChunkEntry[TableIndex];

	// If number of sectors are 0, the chunk is not in the file yet.
	if (ChunkEntry.NumOfSectors != 0)
	{
		// If we have enough space with the current amount of sectors.
		if (ChunkEntry.NumOfSectors * RegionData::SECTOR_SIZE >= DataSize + 4)
		{
			// Add offset after region data block and offset for chunk data size (4 bytes)
			mRegionFile->SeekFromStart((sizeof(RegionData) + ChunkEntry.Offset * RegionData::SECTOR_SIZE));
			mRegionFile->Write((uint8_t*)&DataSize, 4);
			mRegionFile->Write(Data, DataSize);
		}
		else
		{
			RelocateAndAddChunkData(ChunkEntry, Data, DataSize);
		}
	}
	else
	{
		// Chunk is not in file, add it
		AddNewChunk(ChunkEntry, Data, DataSize);
	}
}

void FRegionFile::AddNewChunk(LookupEntry& ChunkEntry, const uint8_t* Data, const uint32_t DataSize)
{
	// Determine how many sectors are currently in this region file.
	uint32_t SectorCount = (mRegionFile->GetFileSize() - sizeof(RegionData)) / RegionData::SECTOR_SIZE;

	// Set this chunks offset at the end of the file and write the data
	ChunkEntry.Offset = SectorCount;
	ChunkEntry.NumOfSectors = 1 + ((DataSize + 4) / RegionData::SECTOR_SIZE); // add 4 bytes for data size

	mRegionFile->SeekFromStart((sizeof(RegionData) + ChunkEntry.Offset * RegionData::SECTOR_SIZE));
	mRegionFile->Write((uint8_t*)&DataSize, 4); // Write size of data
	mRegionFile->Write(Data, DataSize); // Write chunk data

	// Add padding to the rest of the sector
	mRegionFile->Write(FilePadding, (ChunkEntry.NumOfSectors * RegionData::SECTOR_SIZE) - DataSize - 4);
}

void FRegionFile::RelocateAndAddChunkData(LookupEntry& RelocationEntry, const uint8_t* Data, const uint32_t DataSize)
{
	// Copy all data after this sector, then write it starting at this sector
	uint32_t RelocationDataStart = sizeof(RegionData) + ((RelocationEntry.Offset + RelocationEntry.NumOfSectors) * RegionData::SECTOR_SIZE);
	uint32_t RelocationDataSize = mRegionFile->GetFileSize() - RelocationDataStart;

	// Get data to shift left
	std::unique_ptr<uint8_t> RelocationData{ new uint8_t[RelocationDataSize] };
	mRegionFile->SeekFromStart(RelocationDataStart);
	mRegionFile->Read(RelocationData.get(), RelocationDataSize);

	// Seek to the point where this chunk sector started and shift data to this point
	mRegionFile->SeekFromStart(RelocationDataStart - (RelocationEntry.NumOfSectors * RegionData::SECTOR_SIZE));
	mRegionFile->Write(RelocationData.get(), RelocationDataSize);

	// Decrement all offsets in lookup table that were effected
	for (auto& Entry : mRegionData.ChunkEntry)
	{
		if (Entry.Offset > RelocationEntry.Offset)
		{
			Entry.Offset -= RelocationEntry.NumOfSectors;
		}
	}

	//////////////////////////////////////////////////////////////////
	// Add this chunk at the end of file, overwritting shifted data

	// Determine how many sectors are currently in this region file.
	uint32_t SectorCount = (mRegionFile->GetFileSize() - sizeof(RegionData)) / RegionData::SECTOR_SIZE;
	RelocationEntry.Offset = SectorCount - RelocationEntry.NumOfSectors;
	RelocationEntry.NumOfSectors = 1 + ((DataSize + 4) / RegionData::SECTOR_SIZE); // add 4 bytes for data size

	// We are already at the end of the old data, so just write from here
	mRegionFile->Write((uint8_t*)&DataSize, 4); // Write size of data
	mRegionFile->Write(Data, DataSize); // Write chunk data

	// Add padding to the rest of the sector
	mRegionFile->Write(FilePadding, (RelocationEntry.NumOfSectors * RegionData::SECTOR_SIZE) - DataSize - 4);
}