#pragma once

#include <string>
#include <unordered_map>

#include "RegionFile.h"
#include "Math\Vector3.h"

class FWorldFileSystem
{
public:
	static const wchar_t TEMP_DIRECTORY_NAME[];
	static const wchar_t WORLDS_DIRECTORY_NAME[];
	static const wchar_t TEMP_DIRECTORY_PATH[];

public:
	FWorldFileSystem();
	~FWorldFileSystem();

	FWorldFileSystem(const FWorldFileSystem& Other) = delete;
	FWorldFileSystem& operator=(const FWorldFileSystem& Other) = delete;

	/**
	* Sets the specified world as the one currently being operated
	* on.
	* @return False if the world file could not be loaded, true otherwise.
	*/
	bool SetWorld(const wchar_t* WorldName);
	
	/**
	* Chunk size of the current world.
	*/
	uint32_t GetWorldSize() const;

	/**
	* The name of the currently loaded world.
	*/
	std::wstring GetWorldName() const;

	/**
	* Saves the current world data to it's original location on file.
	*/
	void SaveWorld();

	/**
	* Adds a reference the a region file in the region map. If the
	* region file is not present, one is created.
	* @param X, Y, Z Coordinates of the chunk.
	*/
	void AddRegionFileReference(const Vector3i& ChunkPosition);

	/**
	* Removes a reference the a region file in the region map.
	* @param X, Y, Z Coordinates of the chunk.
	*/
	void RemoveRegionFileReference(const Vector3i& ChunkPosition);

	/**
	* Retrieves data for a chunk within the currently loaded world.
	* @param ChunkPosition - The chunk space position of the chunk.
	* @param DataOut - Buffer to place chunk data.
	*/
	void GetChunkData(const Vector3i& ChunkPosition, std::vector<uint8_t>& DataOut);

	/**
	* Writes data for a chunk within the currently loaded world.
	* @param ChunkPosition - The chunk space position of the chunk.
	* @param Data - Buffer containing chunk data.
	*/
	void WriteChunkData(const Vector3i& ChunkPosition, const std::vector<uint8_t>& Data);

private:
	struct RegionFileRecord
	{
		FRegionFile File;
		uint32_t ReferenceCount;
	};

	// Hash functor for file table
	struct Vector3iHash
	{
		std::size_t operator()(const Vector3i& Val)
		{
			return ((Val.x * 73856093) ^ (Val.y * 19349663) ^ (Val.z * 83492791));
		}
	};

private:
	std::wstring mWorldName;
	std::unordered_map<Vector3i, RegionFileRecord, Vector3iHash> mRegionFiles;
	uint32_t mWorldSize;
};