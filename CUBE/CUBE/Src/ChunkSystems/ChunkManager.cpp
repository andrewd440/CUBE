#include "ChunkSystems\ChunkManager.h"
#include "Input\ButtonEvent.h"
#include "Debugging\ConsoleOutput.h"
#include "Rendering\GLUtils.h"
#include "ResourceHolder.h"
#include "Rendering\Camera.h"
#include "Math\Frustum.h"
#include "Rendering\RenderSystem.h"
#include "SFML\Window\Context.hpp"
#include "STime.h"
#include "GL\glew.h"
#include <algorithm>

static const uint32_t DEFAULT_VIEW_DISTANCE = 14;
static const uint32_t MESH_SWAPS_PER_FRAME = 25;
static const int32_t CHUNKS_TO_LOAD_PER_ITERATION = 8;

// Height is half width
static const uint32_t DEFAULT_CHUNK_SIZE = (2 * DEFAULT_VIEW_DISTANCE + 1) * (DEFAULT_VIEW_DISTANCE + 1) * (2 * DEFAULT_VIEW_DISTANCE + 1);

FChunkManager::FChunkManager()
	: mFileSystem()
	, mChunks(nullptr)
	, mChunkPositions()
	, mRenderList()
	, mLoadList()
	, mRebuildList()
	, mBufferSwapQueue()
	, mLoaderThread()
	, mRebuildListMutex()
	, mBufferSwapMutex()
	, mNeedsToRefreshVisibleList()
	, mMustShutdown()
	, mLastCameraChunk()
	, mWorldSize(0)
	, mViewDistance(DEFAULT_VIEW_DISTANCE)
	, mPhysicsSystem(nullptr)
	, mOnBlockDestroy()
	, mOnBlockSet()
{
	mChunks = new FChunk[DEFAULT_CHUNK_SIZE];
	mChunkPositions = new Vector4i[DEFAULT_CHUNK_SIZE];
	mNeedsToRefreshVisibleList = false;
	mMustShutdown = false;
}

FChunkManager::~FChunkManager()
{
	Shutdown();
	delete[] mChunks;
	delete[] mChunkPositions;
}

void FChunkManager::Shutdown()
{	
	mMustShutdown = true;
	if(mLoaderThread.joinable())
		mLoaderThread.join();

	// Finish processing chunks and make sure the correct
	// position are in mChunkPositions
	SwapChunkBuffers();
	UnloadAllChunks();

	mLoadList = std::queue<Vector3i>();
	mRebuildList.clear();
	mRenderList.clear();

	mMustShutdown = false;
}

void FChunkManager::LoadWorld(const wchar_t* WorldName)
{
	Shutdown();
	mFileSystem.SetWorld(WorldName);

	mWorldSize = mFileSystem.GetWorldSize();
	InitializeWorld();
}

void FChunkManager::SaveWorld()
{
	Shutdown();
	mFileSystem.SaveWorld();
	InitializeWorld();
}

void FChunkManager::SetViewDistance(const uint32_t Distance)
{
	Shutdown();
	ReallocateChunkData(Distance);

	InitializeWorld();
}

void FChunkManager::InitializeWorld()
{
	// Set chunk positions to invalid value
	for (uint32_t i = 0; i < ChunkCount(); i++)
	{
		mChunkPositions[i] = Vector4i{ -1, -1, -1 };
	}

	// Activate loader thread
	mNeedsToRefreshVisibleList = true;
	mLoaderThread = std::thread(&FChunkManager::ChunkLoaderThreadLoop, this);
}

void FChunkManager::ReallocateChunkData(const int32_t NewViewDistance)
{
	const uint32_t Size = ChunkCount();
	for (uint32_t i = 0; i < Size; i++)
	{
		mChunks[i].ShutDown(*mPhysicsSystem);
	}

	mViewDistance = NewViewDistance;
	const uint32_t NewSize = ChunkCount();

	// Resize data
	delete[] mChunks;
	delete[] mChunkPositions;
	mChunks = new FChunk[NewSize];
	mChunkPositions = new Vector4i[NewSize];
}

void FChunkManager::UnloadAllChunks()
{
	const uint32_t Size = ChunkCount();
	for (uint32_t i = 0; i < Size; i++)
	{
		if (mChunks[i].IsLoaded())
		{
			const Vector3i UnloadChunkPosition = mChunkPositions[i];

			if (UnloadChunkPosition.y != -1)
			{
				// Buffer for all chunk data
				std::vector<uint8_t> ChunkData;

				// Unload the chunk currently in this index
				mChunks[i].Unload(ChunkData);

				// Write the data to file
				mFileSystem.WriteChunkData(UnloadChunkPosition, ChunkData);
			}
		}
	}

	mFileSystem.ClearAllRegionFileReferences();
}

void FChunkManager::Render(FRenderSystem& Renderer, const GLenum RenderMode)
{
	UpdateRenderList();

	// Render everything in the renderlist
	for (const auto& Index : mRenderList)
	{
		if (mChunks[Index].IsLoaded())
		{
			mChunks[Index].Render(RenderMode);
		}
	}
}

void FChunkManager::Update()
{
	// Get the chunk that the camera is currently in.
	const Vector3i CameraChunk = FCamera::Main->Transform.GetWorldPosition() / FChunk::CHUNK_SIZE;

	// Only update visibility list when that camera crosses a chunk boundary
	if (mLastCameraChunk != CameraChunk)
	{
		mLastCameraChunk = CameraChunk;
		mNeedsToRefreshVisibleList = true;
	}

	SwapChunkBuffers();
}

void FChunkManager::SwapChunkBuffers()
{
	std::unique_lock<std::mutex> Lock(mBufferSwapMutex, std::try_to_lock);

	if (Lock.owns_lock())
	{
		int32_t SwapCount = MESH_SWAPS_PER_FRAME;
		while (SwapCount > 0 && !mBufferSwapQueue.empty())
		{
			const Vector3i ChunkPosition = mBufferSwapQueue.front();
			mBufferSwapQueue.pop_front();

			const uint32_t Index = ChunkIndex(ChunkPosition);

			mChunks[Index].SwapMeshBuffer(*mPhysicsSystem);

			mChunkPositions[Index] = Vector4i{ ChunkPosition, 1 };
			SwapCount--;
		}
	}
}

#undef min
#undef max
void FChunkManager::SetBlock(const Vector3i& Position, FBlockTypes::BlockID ID)
{
	int32_t BlockWorldSize = mWorldSize * FChunk::CHUNK_SIZE;

	if (std::min({ Position.x, Position.y, Position.z }) >= 0 && std::max({ Position.x, Position.y, Position.z }) < BlockWorldSize)
	{
		const Vector4i ChunkPosition = Vector4i(Position / FChunk::CHUNK_SIZE, 1);
		const Vector3i LocalPosition = Vector3i{ Position.x % FChunk::CHUNK_SIZE, Position.y % FChunk::CHUNK_SIZE, Position.z % FChunk::CHUNK_SIZE };

		int32_t Index = ChunkIndex(ChunkPosition);

		// Only set if the right chunk is loaded
		if (ChunkPosition == mChunkPositions[Index])
		{
			mChunks[Index].SetBlock(LocalPosition, ID);
			mOnBlockSet.Invoke(Position, ID);

			std::lock_guard<std::mutex> Lock(mRebuildListMutex);
			if (std::find(mRebuildList.begin(), mRebuildList.end(), Index) == mRebuildList.end())
				mRebuildList.push_back(Index);
		}
	}
}

FBlockTypes::BlockID FChunkManager::GetBlock(Vector3i Position) const
{
	int32_t BlockWorldSize = mWorldSize * FChunk::CHUNK_SIZE;

	if (std::min({ Position.x, Position.y, Position.z }) >= 0 && std::max({ Position.x, Position.y, Position.z }) < BlockWorldSize)
	{
		const Vector4i ChunkPosition = Vector4i(Position / FChunk::CHUNK_SIZE, 1);
		Position = Vector3i{ Position.x % FChunk::CHUNK_SIZE, Position.y % FChunk::CHUNK_SIZE, Position.z % FChunk::CHUNK_SIZE };

		int32_t Index = ChunkIndex(ChunkPosition);

		// Only get if the right chunk is loaded
		if (ChunkPosition == mChunkPositions[Index])
		{
			return mChunks[Index].GetBlock(Position);
		}
	}

	return FBlock::AIR_BLOCK_ID;
}

void FChunkManager::DestroyBlock(const Vector3i& Position)
{
	int32_t BlockWorldSize = mWorldSize * FChunk::CHUNK_SIZE;

	if (std::min({ Position.x, Position.y, Position.z }) >= 0 && std::max({ Position.x, Position.y, Position.z }) < BlockWorldSize)
	{
		const Vector4i ChunkPosition = Vector4i(Position / FChunk::CHUNK_SIZE, 1);
		const Vector3i LocalPosition = Vector3i{ Position.x % FChunk::CHUNK_SIZE, Position.y % FChunk::CHUNK_SIZE, Position.z % FChunk::CHUNK_SIZE };

		int32_t Index = ChunkIndex(ChunkPosition);

		// Only destroy if the right chunk is loaded
		if (ChunkPosition == mChunkPositions[Index])
		{
			const FBlockTypes::BlockID ID = mChunks[Index].DestroyBlock(LocalPosition);
			mOnBlockDestroy.Invoke(Position, ID);

			std::lock_guard<std::mutex> Lock(mRebuildListMutex);
			if (std::find(mRebuildList.begin(), mRebuildList.end(), Index) == mRebuildList.end())
				mRebuildList.push_back(Index);
		}
	}
}

void FChunkManager::SetPhysicsSystem(FPhysicsSystem& Physics)
{
	mPhysicsSystem = &Physics;
}

void FChunkManager::ChunkLoaderThreadLoop()
{
	while (!mMustShutdown)
	{
		while (!mMustShutdown && !mNeedsToRefreshVisibleList)
		{
			UpdateRebuildList();
			UpdateLoadList();
		}

		mNeedsToRefreshVisibleList = false;
		UpdateVisibleList();
	}
}

void FChunkManager::UpdateLoadList()
{
	uint32_t LoadsLeft = CHUNKS_TO_LOAD_PER_ITERATION;

	std::unique_lock<std::mutex> BufferSwapLock(mBufferSwapMutex, std::defer_lock);

	// Buffer for all chunk data
	std::vector<uint8_t> ChunkData;
	while (!mLoadList.empty() && LoadsLeft > 0)
	{
		Vector3i ChunkPosition = mLoadList.front();
		mLoadList.pop();

		const uint32_t Index = ChunkIndex(ChunkPosition);

		// Get region position info for unloaded chunk
		Vector3i UnloadChunkPosition = mChunkPositions[Index];

		// If this chunk is already in the swap list, remove it. This prevents redundant buffer swaps and
		// tells us which chunk position is really currently loaded within the chunk data.
		BufferSwapLock.lock();
		auto InSwapList = std::find(mBufferSwapQueue.begin(), mBufferSwapQueue.end(), ChunkPosition);
		if (InSwapList != mBufferSwapQueue.end())
		{
			UnloadChunkPosition = *InSwapList;
			mBufferSwapQueue.erase(InSwapList);
		}
		BufferSwapLock.unlock();

		///// Unload Chunk ////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////
		if (mChunks[Index].IsLoaded())
		{
			// Unload the chunk currently in this index
			mChunks[Index].Unload(ChunkData);

			ASSERT(UnloadChunkPosition.y != -1);
			// Write the data to file
			mFileSystem.WriteChunkData(UnloadChunkPosition, ChunkData);
			mFileSystem.RemoveRegionFileReference(UnloadChunkPosition);
		}

		///// Load Chunk /////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////
		
		// Get info for chunk data within its region
		ChunkData.clear();
		mFileSystem.AddRegionFileReference(ChunkPosition);
		mFileSystem.GetChunkData(ChunkPosition, ChunkData);
	
		// Load and build the chunk
		Vector3i WorldPosition = ChunkPosition * FChunk::CHUNK_SIZE;
		bool DoesntNeedRebuild = mChunks[Index].Load(ChunkData);

		if (!DoesntNeedRebuild)
			mChunks[Index].RebuildMesh(WorldPosition);

		BufferSwapLock.lock();
			mBufferSwapQueue.push_back(ChunkPosition);
		BufferSwapLock.unlock();

		ChunkData.clear();
		LoadsLeft--;
	}
}

void FChunkManager::UpdateRebuildList()
{
	std::unique_lock<std::mutex> RebuildLock(mRebuildListMutex);
	std::unique_lock<std::mutex> BufferSwapLock(mBufferSwapMutex, std::defer_lock);

	while (!mRebuildList.empty())
	{
		int32_t ChunkIndex = mRebuildList.front();
		mRebuildList.pop_front();
		RebuildLock.unlock();

		const Vector3i ChunkPosition = mChunkPositions[ChunkIndex];
		if (ChunkPosition.y != -1)
		{
			// Check if its already in the swap list and remove if it is.
			BufferSwapLock.lock();
			auto InSwapList = std::find(mBufferSwapQueue.begin(), mBufferSwapQueue.end(), ChunkPosition);
			if (InSwapList != mBufferSwapQueue.end())
				mBufferSwapQueue.erase(InSwapList);
			BufferSwapLock.unlock();

			mChunks[ChunkIndex].RebuildMesh(ChunkPosition * FChunk::CHUNK_SIZE);

			BufferSwapLock.lock();
				mBufferSwapQueue.push_back(mChunkPositions[ChunkIndex]);
			BufferSwapLock.unlock();
		}
		RebuildLock.lock();
	}
}

void FChunkManager::UpdateVisibleList()
{
	// Offset the camera chunk position so the loop centers the camera
	Vector3i CameraChunkOffset = mLastCameraChunk - Vector3i{ mViewDistance, 0, mViewDistance };

	// Get the total range of visible area.
	const int32_t HorizontalBounds = 2 * mViewDistance + 1;

	// Clear previous load and visibile list when moving across chunks.
	mLoadList = std::queue<Vector3i>();

	// Add all chunks in the visible range to the visible list.
	// First add the xz plane that the camera is currently on.
	if (CameraChunkOffset.y < mWorldSize && CameraChunkOffset.y >= 0)
	{
		for (int32_t x = 0; x < HorizontalBounds; x++)
		{
			const int32_t xPosition = CameraChunkOffset.x + x;
			if (xPosition >= mWorldSize || xPosition < 0)
				continue;

			for (int32_t z = 0; z < HorizontalBounds; z++)
			{
				const int32_t zPosition = CameraChunkOffset.z + z;
				if (zPosition >= mWorldSize || zPosition < 0)
					continue;

				const Vector4i ChunkPosition{ xPosition, CameraChunkOffset.y, zPosition, 1 };
				const int32_t VisibleChunkIndex = ChunkIndex(ChunkPosition);

				// If this visible chunk is not loaded, load it.
				if (mChunkPositions[VisibleChunkIndex] != ChunkPosition)
				{
					mLoadList.push(ChunkPosition);
				}
			}
		}
	}

	// Now add the world by alternating the xz planes from below to above the camera's chunk
	for (int32_t v = 1; v <= mViewDistance / 2; v++)
	{
		for (int32_t y = -v; y < v + 1; y += 2 * v)
		{
			const int32_t yPosition = CameraChunkOffset.y + y;

			if (yPosition >= mWorldSize || yPosition < 0)
				continue;

			for (int32_t x = 0; x < HorizontalBounds; x++)
			{
				const int32_t xPosition = CameraChunkOffset.x + x;

				if (xPosition >= mWorldSize || xPosition < 0)
					continue;

				for (int32_t z = 0; z < HorizontalBounds; z++)
				{
					const int32_t zPosition = CameraChunkOffset.z + z;

					if (zPosition >= mWorldSize || zPosition < 0)
						continue;

					const Vector4i ChunkPosition{ xPosition, yPosition, zPosition, 1 };
					const int32_t VisibleChunkIndex = ChunkIndex(ChunkPosition);

					// If this visible chunk is not loaded, load it.
					if (mChunkPositions[VisibleChunkIndex] != ChunkPosition)
					{
						mLoadList.push(ChunkPosition);
					}
				}
			}
		}
	}
}

void FChunkManager::UpdateRenderList()
{
	// Start with a fresh list
	mRenderList.clear();

	// The the current view frustum in chunk coord
	FMatrix4 ToChunkCoord;
	ToChunkCoord.Scale(1.0f / (float)FChunk::CHUNK_SIZE);
	ToChunkCoord.SetOrigin(-Vector3f{ 0.5f, 0.5f, 0.5f }); // align with center of chunks

	FFrustum ViewFrustum = FCamera::Main->GetWorldViewFrustum();
	ViewFrustum.TransformBy(ToChunkCoord);

	// Check each visible chunk against the frustum
	const uint32_t ListSize = ChunkCount();

	for (uint32_t i = 0; i < ListSize; i++)
	{
		Vector4f CenterFloats{mChunkPositions[i]};

		if (!mChunks[i].IsEmpty() && ViewFrustum.IsUniformAABBVisible(CenterFloats, 1.0f))
		{
			mRenderList.push_back(i);
		}
	}
}