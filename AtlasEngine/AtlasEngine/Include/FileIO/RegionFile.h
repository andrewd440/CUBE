#pragma once

#include <cstdint>
#include "Math\Vector3.h"
#include "SystemResources\SystemFile.h"
#include <memory>

/**
* Represents a region file for storing world
* chunk layouts.
*/
class FRegionFile
{
public:
	static Vector3i ChunkToRegionPosition(Vector3i WorldChunkPosition)
	{
		return WorldChunkPosition / (int32_t)RegionData::REGION_SIZE;
	}

	static Vector3i ChunkToRegionPosition(int32_t x, int32_t y, int32_t z)
	{
		return ChunkToRegionPosition(Vector3i{x, y, z}); // vector simd
	}

public:
	/**
	* Constructs a unbound region file.
	*/
	FRegionFile();

	~FRegionFile();

	/**
	* Loads a specific region file. All region files for a world is placed in
	* the Worlds/(world-name)/.vgr directory. 
	* @param WorldName - The name of this world this region is a part of.
	* @param RegionPosition - The position of the region you world to load.
	* @return True if the region file was loaded successfully.
	*/
	bool Load(const wchar_t* WorldName, const Vector3i& RegionPosition);

	/**
	* Retrieve info about a specific chunk.
	* @param ChunkPosition - Position of the chunk within this region.
	* @param SizeOut - To put the size, in bytes, of the data for the chunk.
	* @param SectorOffsetOut - The sector offset for this chunks data.
	*/
	void GetChunkDataInfo(const Vector3i& ChunkPosition, uint32_t& SizeOut, uint32_t& SectorOffsetOut);

	/**
	* Retrieves the data for the layout of a chunk.
 	* @param SectorOffset - The offset of the chunks sector. Can be retrieved from GetChunkDataInfo().
	* @param DataOut - To put the chunk data.
	* @param DataSize - The size of this chunks data. Can be retrieved from GetChunkDataInfo().
	*/
	void GetChunkData(const uint32_t SectorOffset, uint8_t* DataOut, const uint32_t DataSize);

	/**
	* Writes data for a chunk to file.
	* @param ChunkPosition - Position of the chunk within this region.
	* @param Data - Data to write.
	* @param SizeOut - The size of the data to write.
	*/
	void WriteChunkData(const Vector3i& ChunkPosition, const uint8_t* Data, const uint32_t DataSize);

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
	/**
	* Adds a new chunk to the region file.
	*/
	void AddNewChunk(LookupEntry& ChunkEntry, const uint8_t* Data, const uint32_t DataSize);

	/**
	* Shifts chunk data to the left and relocates RelocationEntry to the end of the file.
	*/
	void RelocateAndAddChunkData(LookupEntry& RelocationEntry, const uint8_t* Data, const uint32_t DataSize);

private:
	RegionData mRegionData;
	std::unique_ptr<IFileHandle> mRegionFile;
};

