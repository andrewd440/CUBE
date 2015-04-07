#include "Rendering\ChunkManager.h"
#include "Input\ButtonEvent.h"
#include "Debugging\ConsoleOutput.h"
#include "Rendering\GLUtils.h"
#include "ResourceHolder.h"
#include "Rendering\Camera.h"
#include "Math\Frustum.h"
#include "Rendering\RenderSystem.h"
#include "SFML\Window\Context.hpp"

const int32_t FChunkManager::CHUNKS_TO_LOAD_PER_FRAME = 8;
static const uint32_t DEFAULT_VIEW_DISTANCE = 8;

// Height is half width
static const uint32_t DEFAULT_CHUNK_SIZE = (2 * DEFAULT_VIEW_DISTANCE + 1) * (DEFAULT_VIEW_DISTANCE + 1) * (2 * DEFAULT_VIEW_DISTANCE + 1);

FChunkManager::FChunkManager()
	: mFileSystem()
	, mChunks(nullptr)
	, mChunkPositions(DEFAULT_CHUNK_SIZE)
	, mRenderList()
	, mLoadList()
	, mRebuildList()
	, mBufferSwapQueue()
	, mLoaderThread()
	, mRebuildListMutex()
	, mLoadListMutex()
	, mBufferSwapMutex()
	, mIsLoadListRefreshing()
	, mMustShutdown()
	, mLastCameraChunk()
	, mLastCameraPosition()
	, mLastCameraDirection()
	, mWorldSize(0)
	, mViewDistance(DEFAULT_VIEW_DISTANCE)
	, mPhysicsSystem(nullptr)
{
	mChunks = new FChunk[DEFAULT_CHUNK_SIZE];
	mIsLoadListRefreshing = false;
	mMustShutdown = false;
}

FChunkManager::~FChunkManager()
{
	Shutdown();
	delete[] mChunks;
}

void FChunkManager::Shutdown()
{
	mMustShutdown.store(true);

	// Finish processing chunks and make sure the correct
	// position are in mChunkPositions
	SwapChunkBuffers();

	if(mLoaderThread.joinable())
		mLoaderThread.join();

	mLoadList = std::queue<Vector3i>();
	mRebuildList.clear();
	mRenderList.clear();

	const uint32_t Size = ChunkCount();
	for (uint32_t i = 0; i < Size; i++)
	{
		mChunks[i].ShutDown(*mPhysicsSystem);
	}

	mMustShutdown.store(false);
}

void FChunkManager::LoadWorld(const wchar_t* WorldName)
{
	Shutdown();
	std::fill(mChunkPositions.begin(), mChunkPositions.end(), Vector3i{ -1, -1, -1 });

	mFileSystem.SetWorld(WorldName);

	mWorldSize = mFileSystem.GetWorldSize();

	mLoaderThread = std::thread(&FChunkManager::ChunkLoaderThreadLoop, this);
}

void FChunkManager::SetViewDistance(const uint32_t Distance)
{
	Shutdown();

	mViewDistance = Distance;

	const int32_t NewBounds = ChunkCount();

	// Resize data
	delete [] mChunks;
	mChunks = new FChunk[NewBounds];
	mChunkPositions.resize(NewBounds);
	std::fill(mChunkPositions.begin(), mChunkPositions.end(), Vector3i{ -1, -1, -1 });

	UpdateVisibleList();
	mLoaderThread = std::thread(&FChunkManager::ChunkLoaderThreadLoop, this);
}

void FChunkManager::UnloadAllChunks()
{
	const uint32_t ChunksSize = ChunkCount();
	for (uint32_t i = 0; i < ChunksSize; i++)
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

				// Get region position info for unloaded chunk
				const Vector3i UnloadRegionPosition = FRegionFile::ChunkToRegionPosition(UnloadChunkPosition);
				const Vector3i UnloadLocalRegionPosition = FRegionFile::LocalRegionPosition(UnloadChunkPosition);

				// Write the data to file
				mFileSystem.WriteChunkData(UnloadChunkPosition, ChunkData);
				mFileSystem.RemoveRegionFileReference(UnloadChunkPosition);
			}
		}
	}
}

void FChunkManager::Render(FRenderSystem& Renderer, const GLenum RenderMode)
{
	// Get camera data
	const Vector3f CameraDirection = FCamera::Main->Transform.GetRotation() * -Vector3f::Forward;
	const Vector3f CameraPosition = FCamera::Main->Transform.GetWorldPosition();

	// Update the renderlist if the camera position or direction has changed.
	if (mLastCameraPosition != CameraPosition || mLastCameraDirection != CameraDirection)
	{
		mLastCameraPosition = CameraPosition;
		UpdateRenderList();
	}

	// Render everything in the renderlist
	for (const auto& Index : mRenderList)
	{
		if (mChunks[Index].IsLoaded())
		{
			const Vector3i Position = mChunkPositions[Index] * FChunk::CHUNK_SIZE;;
			Renderer.SetModelTransform(FTransform{ Position });
			mChunks[Index].Render(RenderMode);
		}
	}
}

void FChunkManager::Update()
{
	// Get the chunk that the camera is currently in.
	const Vector3f CameraPosition = FCamera::Main->Transform.GetWorldPosition();
	const Vector3i CameraChunk = CameraPosition / FChunk::CHUNK_SIZE;

	// Only update visibility list when that camera crosses a chunk boundary
	if (mLastCameraChunk != CameraChunk)
	{
		mLastCameraChunk = CameraChunk;
		UpdateVisibleList();
	}

	SwapChunkBuffers();
}

void FChunkManager::SwapChunkBuffers()
{	
	std::lock_guard<std::mutex> Lock(mBufferSwapMutex);
	while (!mBufferSwapQueue.empty())
	{
		const Vector3i ChunkPosition = mBufferSwapQueue.front();
		mBufferSwapQueue.pop_front();

		const uint32_t Index = ChunkIndex(ChunkPosition);

		mChunks[Index].SwapMeshBuffer(*mPhysicsSystem);

		mChunkPositions[Index] = ChunkPosition;
	}
}

void FChunkManager::SetBlock(Vector3i Position, FBlock::BlockType BlockType)
{
	const Vector3i ChunkPosition = Position / FChunk::CHUNK_SIZE;
	Position = Vector3i{ Position.x % FChunk::CHUNK_SIZE, Position.y % FChunk::CHUNK_SIZE, Position.z % FChunk::CHUNK_SIZE };

	int32_t ChunkNumber = ChunkIndex(ChunkPosition);
	mChunks[ChunkNumber].SetBlock(Position, BlockType);

	std::lock_guard<std::mutex> Lock(mRebuildListMutex);
	if (std::find(mRebuildList.begin(), mRebuildList.end(), ChunkNumber) == mRebuildList.end())
		mRebuildList.push_back(ChunkNumber);
}

FBlock::BlockType FChunkManager::GetBlock(Vector3i Position) const
{
	const Vector3i ChunkPosition = Position / FChunk::CHUNK_SIZE;
	Position = Vector3i{ Position.x % FChunk::CHUNK_SIZE, Position.y % FChunk::CHUNK_SIZE, Position.z % FChunk::CHUNK_SIZE };

	int32_t ChunkNumber = ChunkIndex(ChunkPosition);
	return mChunks[ChunkNumber].GetBlock(Position);
}

void FChunkManager::DestroyBlock(Vector3i Position)
{
	const Vector3i ChunkPosition = Position / FChunk::CHUNK_SIZE;
	Position = Vector3i{ Position.x % FChunk::CHUNK_SIZE, Position.y % FChunk::CHUNK_SIZE, Position.z % FChunk::CHUNK_SIZE };

	int32_t ChunkNumber = ChunkIndex(ChunkPosition);
	mChunks[ChunkNumber].DestroyBlock(Position);

	std::lock_guard<std::mutex> Lock(mRebuildListMutex);
	if (std::find(mRebuildList.begin(), mRebuildList.end(), ChunkNumber) == mRebuildList.end())
		mRebuildList.push_back(ChunkNumber);
}

void FChunkManager::SetPhysicsSystem(FPhysicsSystem& Physics)
{
	mPhysicsSystem = &Physics;
}

void FChunkManager::ChunkLoaderThreadLoop()
{
	while (!mMustShutdown)
	{
		UpdateRebuildList();
		UpdateLoadList();
	}

	UnloadAllChunks();
}

void FChunkManager::UpdateLoadList()
{
	uint32_t LoadsLeft = CHUNKS_TO_LOAD_PER_FRAME;

	std::unique_lock<std::mutex> LoadLock(mLoadListMutex);
	std::unique_lock<std::mutex> BufferSwapLock(mBufferSwapMutex, std::defer_lock);
	while (!mLoadList.empty() && LoadsLeft > 0)
	{
		// Buffer for all chunk data
		std::vector<uint8_t> ChunkData;

		if (mIsLoadListRefreshing)
			return;

		Vector3i ChunkPosition = mLoadList.front();
		mLoadList.pop();
		LoadLock.unlock();

		const uint32_t Index = ChunkIndex(ChunkPosition);

		///// Unload Chunk ////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////
		if (mChunks[Index].IsLoaded())
		{
			// Unload the chunk currently in this index
			mChunks[Index].Unload(ChunkData);

			// Get region position info for unloaded chunk
			const Vector3i UnloadChunkPosition = mChunkPositions[Index];

			// Write the data to file
			mFileSystem.WriteChunkData(UnloadChunkPosition, ChunkData);
			mFileSystem.RemoveRegionFileReference(UnloadChunkPosition);
		}

		///// Load Chunk /////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////
		
		// Get info for chunk data within its region
		mFileSystem.AddRegionFileReference(ChunkPosition);
		mFileSystem.GetChunkData(ChunkPosition, ChunkData);
	
		// Load and build the chunk
		Vector3i WorldPosition = ChunkPosition * FChunk::CHUNK_SIZE;
		mChunks[Index].Load(ChunkData, WorldPosition);

		BufferSwapLock.lock();
		auto& InSwapList = std::find(mBufferSwapQueue.begin(), mBufferSwapQueue.end(), ChunkPosition);
		if (InSwapList != mBufferSwapQueue.end())
			mBufferSwapQueue.erase(InSwapList);
		BufferSwapLock.unlock();

		mChunks[Index].RebuildMesh();

		BufferSwapLock.lock();
			mBufferSwapQueue.push_back(ChunkPosition);
		BufferSwapLock.unlock();

		LoadsLeft--;
		LoadLock.lock();
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
		if (ChunkPosition != Vector3i{ -1, -1, -1 })
		{

			// Check if its already in the swap list and remove if it is.
			BufferSwapLock.lock();
			auto& InSwapList = std::find(mBufferSwapQueue.begin(), mBufferSwapQueue.end(), ChunkPosition);
			if (InSwapList != mBufferSwapQueue.end())
				mBufferSwapQueue.erase(InSwapList);
			BufferSwapLock.unlock();

			mChunks[ChunkIndex].RebuildMesh();

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
	const int32_t ChunkBounds = 2 * mViewDistance + 1;

	// Clear previous load and visibile list when moving across chunks.
	mIsLoadListRefreshing.store(true);
	mLoadList = std::queue<Vector3i>();

	std::lock_guard<std::mutex> LoadLock(mLoadListMutex);

	// Add all chunks in the visible range to the visible list.
	// First add the xz plane that the camera is currently on.
	if (CameraChunkOffset.y < mWorldSize && CameraChunkOffset.y >= 0)
	{
		for (int32_t x = 0; x < ChunkBounds; x++)
		{
			const int32_t xPosition = CameraChunkOffset.x + x;
			if (xPosition >= mWorldSize || xPosition < 0)
				continue;

			for (int32_t z = 0; z < ChunkBounds; z++)
			{
				const int32_t zPosition = CameraChunkOffset.z + z;
				if (zPosition >= mWorldSize || zPosition < 0)
					continue;

				const Vector3i ChunkPosition{ xPosition, CameraChunkOffset.y, zPosition };
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
		for (int32_t y = -v; y < v + 1; y+= 2 * v)
		{
			const int32_t yPosition = CameraChunkOffset.y + y;

			if (yPosition >= mWorldSize || yPosition < 0)
				continue;

			for (int32_t x = 0; x < ChunkBounds; x++)
			{
				const int32_t xPosition = CameraChunkOffset.x + x;

				if (xPosition >= mWorldSize || xPosition < 0)
					continue;

				for (int32_t z = 0; z < ChunkBounds; z++)
				{
					const int32_t zPosition = CameraChunkOffset.z + z;

					if (zPosition >= mWorldSize || zPosition < 0)
						continue;

					const Vector3i ChunkPosition{ xPosition, yPosition, zPosition };
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

	mIsLoadListRefreshing.store(false);
}

void FChunkManager::UpdateRenderList()
{
	// Start with a fresh list
	mRenderList.clear();

	// The the current view frustum
	const FFrustum ViewFrustum = FCamera::Main->GetWorldViewFrustum();
	const int32_t ChunkHalfWidth = FChunk::CHUNK_SIZE / 2.0f;

	const Vector4i ChunkSizeVector{ FChunk::CHUNK_SIZE, FChunk::CHUNK_SIZE, FChunk::CHUNK_SIZE, 1 };
	const Vector4i HalfChunkVector{ ChunkHalfWidth, ChunkHalfWidth, ChunkHalfWidth, 0 };

	// Check each visible chunk against the frustum
	const uint32_t ListSize = ChunkCount();
	for (uint32_t i = 0; i < ListSize; i++)
	{
		const Vector4f ChunkCenter{ Vector4i{ mChunkPositions[i], 1 } * ChunkSizeVector + HalfChunkVector};

		if (!mChunks[i].IsEmpty() && ViewFrustum.IsUniformAABBVisible(ChunkCenter, FChunk::CHUNK_SIZE))
		{
			mRenderList.push_back(i);
		}
	}
}
