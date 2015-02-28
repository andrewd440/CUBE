#pragma once

#include <cstdint>
#include "Math\Vector3.h"
#include "SystemResources\SystemFile.h"
#include <memory>

class FRegionFile
{
public:
	FRegionFile();

	~FRegionFile();

	bool Load(const wchar_t* RegionDirectory, const Vector3ui& RegionPosition);

	void GetChunkDataInfo(const Vector3ui& ChunkPosition, uint32_t& SizeOut, uint32_t& SectorOffsetOut);

	void GetChunkData(const uint32_t SectorOffset, uint8_t* DataOut, const uint32_t DataSize);

	void WriteChunkData(const Vector3ui& ChunkPosition, const uint8_t* Data, const uint32_t DataSize);

public:
	/**
	* Lookup table entry for a chunk in the region file.
	* If NumOfSectors is 0, then this chunk is currently not in the
	* region file and needs to be appended.
	*/
	struct LookupEntry
	{
		uint32_t Offset : 24;       // Sector offset of a chunk
		uint32_t NumOfSectors : 8;  // Number of sectors a chunk consumes
	};

	struct RegionData
	{
		static const uint32_t REGION_SIZE = 16;
		static const uint32_t SECTOR_SIZE = 4096;
		LookupEntry ChunkEntry[REGION_SIZE * REGION_SIZE * REGION_SIZE];
		// Sectors will follow the lookup table.
		// Each sector is 4KiB and contains RLE chunk data.
		// Chunk data starts with 4 byte data size at the beginning
		// of the sector.
	};

private:
	void AddNewChunk(LookupEntry& ChunkEntry, const uint8_t* Data, const uint32_t DataSize);
	void RelocateAndAddChunkData(LookupEntry& RelocationsEntry, const uint8_t* Data, const uint32_t DataSize);

private:
	RegionData mRegionData;
	std::unique_ptr<IFileHandle> mRegionFile;
};

