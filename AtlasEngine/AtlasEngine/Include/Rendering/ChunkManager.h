#pragma once

#include <vector>
#include <queue>
#include <unordered_map>

#include "Chunk.h"
#include "noise\noise.h"
#include "noise\noiseutils.h"
#include "Singleton.h"
#include "FileIO\RegionFile.h"

class FRenderSystem;

class FChunkManager : public TSingleton<FChunkManager>
{
public:
	static const int32_t WORLD_SIZE;
	static const int32_t VISIBILITY_DISTANCE;
	static const int32_t CHUNKS_TO_LOAD_PER_FRAME;

public:
	FChunkManager();
	~FChunkManager();

	void Setup();

	void Update();

	void Render(FRenderSystem& Renderer, const GLenum RenderMode = GL_TRIANGLES);

	float GetNoiseHeight(int32_t x, int32_t z);

	/**
	* Set a block in the world at a specific position.
	*/
	void SetBlock(Vector3i Position, FBlock::BlockType BlockType);

	/**
	* Retrieves the type of block in the world at a specific position.
	*/
	FBlock::BlockType GetBlock(Vector3i Position) const;

	/**
	* Destroys a block in the world at a specific position.
	*/
	void DestroyBlock(Vector3i Position);

	Vector3i GetChunkPosition(const FChunk* Chunk) const;

	void Shutdown();

private:
	void UpdateUnloadList();
	void UpdateLoadList();
	void UpdateRebuildList();
	void UpdateVisibleList();
	void UpdateRenderList();

	/**
	* Adds a reference the a region file in the region map. If the
	* region file is not present, one is created.
	* @param X, Y, Z Coordinates of the chunk.
	*/
	void AddRegionFileReference(int32_t X, int32_t Y, int32_t Z);

	void RemoveRegionFileReference(const Vector3i& ChunkPosition);

private:
	std::vector<FChunk> mChunks;        // All world chunks
	std::vector<uint32_t> mVisibleList; // Index list of potentially visible chunks
	std::vector<uint32_t> mRenderList;  // Index list of chunks to render
	std::vector<uint32_t> mIsLoadedList;  // Index list of all chunks that are currently loaded.
	std::vector<uint32_t> mLoadList;     // Index list of chunks to be loaded
	std::vector<uint32_t> mUnloadList;   // Index list of chunks to be unloaded
	std::vector<uint32_t> mRebuildList;  // Index list of chunks to be rebuilt

	struct RegionFileRecord
	{
		FRegionFile File;
		uint32_t ReferenceCount;
	};

	// Hash functor for vector3
	struct Vector3iHash
	{
		std::size_t operator()(const Vector3i& Val)
		{
			return Val.y * FChunkManager::WORLD_SIZE * FChunkManager::WORLD_SIZE + Val.x * FChunkManager::WORLD_SIZE + Val.z;
		}
	};

	std::unordered_map<Vector3i, RegionFileRecord, Vector3iHash> mRegionFiles;

	// Rendering data
	Vector3i mLastCameraChunk;
	Vector3f mLastCameraPosition;
	Vector3f mLastCameraDirection;
	noise::utils::NoiseMap mNoiseMap;
};