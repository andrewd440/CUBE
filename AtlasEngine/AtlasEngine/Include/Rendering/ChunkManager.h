#pragma once

#include <vector>
#include <queue>
#include <unordered_map>
#include <string>

#include "Chunk.h"
#include "noise\noise.h"
#include "noise\noiseutils.h"
#include "Singleton.h"
#include "FileIO\RegionFile.h"

class FPhysicsSystem;
class FRenderSystem;

/**
* Class for managing a world.
*/
class FChunkManager
{
public:
	static const int32_t VISIBILITY_DISTANCE;
	static const int32_t CHUNKS_TO_LOAD_PER_FRAME;

public:
	FChunkManager();
	~FChunkManager();

	/**
	* Updates chunk information. This function should be called
	* during each game loop.
	*/
	void Update();

	/**
	* Renders all current visible world geometry.
	* @brief This function will render what is visible to the 
	*			main camera.
	*/
	void Render(FRenderSystem& Renderer, const GLenum RenderMode = GL_TRIANGLES);

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

	/**
	* Retrieves the size of the world in chunks.
	*/
	int32_t GetWorldSize() const { return mWorldSize; }

	/**
	* Loads a new world from file.
	* @param WorldName - The name of the world to load.
	* @brief A folder by the name of the world will be search for in
	*        the World folder in the program's root directory.
	*/
	void LoadWorld(const wchar_t* WorldName);

	/**
	* Saves all current world data and closes
	* needed services. This should be called before closing the
	* program to save current world data properly.
	*/
	void Shutdown();

	/**
	* Sets the physics system used by the chunk manager.
	*/
	void SetPhysicsSystem(FPhysicsSystem& Physics);

private:
	// Updates the current unload list
	void UpdateUnloadList();

	// Updates the current load list
	void UpdateLoadList();

	/**
	* Rebuilds the chunks that have
	* changed during the last update.
	*/
	void UpdateRebuildList();

	/**
	* Updates the currently visible chunks in the scene.
	*/
	void UpdateVisibleList();

	/**
	* Updates the render list.
	*/
	void UpdateRenderList();

	/**
	* Reconstructs internal data to function with
	* a new world size. Called when a new world is loaded.
	*/
	void ResizeWorld();

	/**
	* Unloads all chunks that are currently loaded.
	*/
	void UnloadAllChunks();

	/**
	* Obtains data needed after a new world has been loaded.
	*/
	void PostWorldSetup();

	/**
	* Converts a 3D chunk position into an index into the chunks array.
	*/
	int32_t ChunkIndex(Vector3i Position) const;

	/**
	* Converts a 3D chunk position into an index into the chunks array.
	*/
	int32_t ChunkIndex(int32_t X, int32_t Y, int32_t Z) const;

	/**
	* Convertes a chunk index into a chunk space 3D coordinate.
	*/
	Vector3i IndexToChunkPosition(int32_t Index) const;

	/**
	* Convertes a chunk index into a world space 3D position.
	*/
	Vector3i IndexToWorldPosition(int32_t Index) const;

	/**
	* Adds a reference the a region file in the region map. If the
	* region file is not present, one is created.
	* @param X, Y, Z Coordinates of the chunk.
	*/
	void AddRegionFileReference(int32_t X, int32_t Y, int32_t Z);
	
	/**
	* Removes a reference the a region file in the region map.
	* @param X, Y, Z Coordinates of the chunk.
	*/
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

	// Hash functor for Vector3i
	struct Vector3iHash
	{
		std::size_t operator()(const Vector3i& Val)
		{	
			const int32_t MaxOpenFiles = 10;
			return ((Val.x * 73856093) ^ (Val.y * 19349663) ^ (Val.z * 83492791)) % MaxOpenFiles;
		}
	};

	std::unordered_map<Vector3i, RegionFileRecord, Vector3iHash> mRegionFiles;

	// Rendering data
	Vector3i mLastCameraChunk;
	Vector3f mLastCameraPosition;
	Vector3f mLastCameraDirection;
	int32_t mWorldSize;
	std::wstring mWorldName;

	// Physics Data
	FPhysicsSystem* mPhysicsSystem;
};


inline int32_t FChunkManager::ChunkIndex(Vector3i Position) const 
{
	ASSERT(Position.x >= 0 && Position.x < mWorldSize &&
		Position.y >= 0 && Position.y < mWorldSize &&
		Position.z >= 0 && Position.z < mWorldSize)

	const Vector3i PositionToIndex{ mWorldSize, mWorldSize * mWorldSize, 1 };
	return Vector3i::Dot(Position, PositionToIndex);
}

inline int32_t FChunkManager::ChunkIndex(int32_t X, int32_t Y, int32_t Z) const 
{
	return ChunkIndex(Vector3i{ X, Y, Z });
}

inline Vector3i FChunkManager::IndexToChunkPosition(int32_t Index) const
{
	const int32_t X = (Index / mWorldSize % mWorldSize);
	const int32_t Y = (Index / (mWorldSize * mWorldSize));
	const int32_t Z = (Index % mWorldSize);

	return Vector3i(X, Y, Z);
}

inline Vector3i FChunkManager::IndexToWorldPosition(int32_t Index) const
{
	return IndexToChunkPosition(Index) * FChunk::CHUNK_SIZE;
}