#pragma once

#include <vector>
#include <queue>
#include <unordered_map>
#include <string>
#include <thread>
#include <mutex>

#include "Chunk.h"
#include "LibNoise\noise.h"
#include "LibNoise\noiseutils.h"
#include "Singleton.h"
#include "FileIO\WorldFileSystem.h"

class FPhysicsSystem;
class FRenderSystem;

/**
* Class for managing a world.
*/
class FChunkManager
{
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
	void SetBlock(Vector3i Position, FBlock::Type Type);

	/**
	* Retrieves the type of block in the world at a specific position.
	*/
	FBlock::Type GetBlock(Vector3i Position) const;

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
	* @brief A folder by the name of the world will be searched for in
	*        the World folder in the program's root directory.
	*/
	void LoadWorld(const wchar_t* WorldName);

	/**
	* Save the current world to file. If the world is large, this operation may
	* take some time due to a large amount of data needing to be copied on file.
	*/
	void SaveWorld();

	/**
	* Sets the world view distance. This is in terms
	* of chunk space.
	*/
	void SetViewDistance(const uint32_t Distance);

	/**
	* Sets the physics system used by the chunk manager.
	*/
	void SetPhysicsSystem(FPhysicsSystem& Physics);

private:
	/**
	* Saves all current world data and closes
	* needed services. This should be called before closing the
	* program to save current world data properly.
	*/
	void Shutdown();

	/**
	* The total number of chunks allocated.
	*/
	uint32_t ChunkCount() const;

	void ChunkLoaderThreadLoop();

	/**
	* Processes the buffer swap list for chunks.
	*/
	void SwapChunkBuffers();

	/**
	* Updates the current load list
	*/
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

	void ReallocateChunkData(const int32_t NewViewDistance);

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

private:
	FWorldFileSystem      mFileSystem;
	FChunk*               mChunks;        // All world chunks
	Vector4i*             mChunkPositions;
	std::vector<uint32_t> mRenderList;    // Index list of chunks to render
	std::queue<Vector3i>  mLoadList;      // Index list of chunks to be loaded
	std::deque<uint32_t>  mRebuildList;   // Index list of chunks to be rebuilt
	std::deque<Vector3i>  mBufferSwapQueue;
	std::thread           mLoaderThread;
	std::mutex            mRebuildListMutex;
	std::mutex            mBufferSwapMutex;
	std::atomic_bool      mNeedsToRefreshVisibleList;
	std::atomic_bool      mMustShutdown;

	// Rendering data
	Vector3i mLastCameraChunk;
	int32_t mWorldSize;
	int32_t mViewDistance;

	// Physics Data
	FPhysicsSystem* mPhysicsSystem;
};


inline int32_t FChunkManager::ChunkIndex(Vector3i Position) const 
{
	ASSERT(Position.x >= 0 && Position.x < mWorldSize &&
		Position.y >= 0 && Position.y < mWorldSize &&
		Position.z >= 0 && Position.z < mWorldSize)

	// Normalize the position
	const int32_t HorizontalBounds = 2 * mViewDistance + 1;
	const int32_t VerticalBounds = mViewDistance + 1;

	// Bit shift for mod operation
	Position = Vector3i{ Position.x % HorizontalBounds, Position.y % VerticalBounds, Position.z % HorizontalBounds };

	const Vector3i PositionToIndex{ HorizontalBounds, HorizontalBounds * HorizontalBounds, 1 };
	return Vector3i::Dot(Position, PositionToIndex);
}

inline int32_t FChunkManager::ChunkIndex(int32_t X, int32_t Y, int32_t Z) const 
{
	return ChunkIndex(Vector3i{ X, Y, Z });
}

inline uint32_t FChunkManager::ChunkCount() const
{
	return (2 * mViewDistance + 1) * (mViewDistance + 1) * (2 * mViewDistance + 1);
}