#include "Rendering\ChunkManager.h"
#include "Input\ButtonEvent.h"
#include "Debugging\ConsoleOutput.h"
#include "Rendering\GLUtils.h"
#include "ResourceHolder.h"
#include "Rendering\Camera.h"
#include "Math\Frustum.h"
#include "Rendering\RenderSystem.h"

const int32_t FChunkManager::VISIBILITY_DISTANCE = 10;
const int32_t FChunkManager::CHUNKS_TO_LOAD_PER_FRAME = 1;

FChunkManager::FChunkManager()
	: mChunks()
	, mVisibleList()
	, mRenderList()
	, mLoadList()
	, mIsLoadedList()
	, mUnloadList()
	, mRebuildList()
	, mRegionFiles()
	, mLastCameraChunk()
	, mLastCameraPosition()
	, mLastCameraDirection()
	, mWorldSize(2)
	, mWorldName()
	, mPhysicsSystem(nullptr)
{
}

FChunkManager::~FChunkManager()
{
}

void FChunkManager::Shutdown()
{
	UnloadAllChunks();
}

void FChunkManager::LoadWorld(const wchar_t* WorldName)
{
	// Set world name and extract world size from file
	mWorldName = WorldName;

	IFileSystem& FileSystem = IFileSystem::GetInstance();
	FileSystem.SetToProgramDirectory();
	FileSystem.SetDirectory(L"Worlds");
	FileSystem.SetDirectory(WorldName);

	auto WorldInfoFile = FileSystem.OpenReadable(L"WorldInfo.vgw");
	WorldInfoFile->Read((uint8_t*)&mWorldSize, 4);
	ASSERT((mWorldSize & (mWorldSize - 1)) == 0x0 && "World size must be a power of two.");

	ResizeWorld();
}

void FChunkManager::ResizeWorld()
{
	UnloadAllChunks();
	mLoadList.clear();
	mRegionFiles.clear();
	mVisibleList.clear();
	mRenderList.clear();
	mChunks.resize(mWorldSize * mWorldSize * mWorldSize);

	for (auto& Chunk : mChunks)
	{
		Chunk.SetChunkManager(this);
	}
	PostWorldSetup();
}

void FChunkManager::UnloadAllChunks()
{
	for (const auto& Loaded : mIsLoadedList)
	{
		mUnloadList.push_back(Loaded);

		const Vector3i Position = IndexToChunkPosition(Loaded);
		AddRegionFileReference(Position.x, Position.y, Position.z);
	}
	UpdateUnloadList();
}

void FChunkManager::PostWorldSetup()
{
	// Store needed camera data.
	mLastCameraDirection = FCamera::Main->Transform.GetRotation() * -Vector3f::Forward;
	mLastCameraPosition = FCamera::Main->Transform.GetPosition();
	mLastCameraChunk = Vector3i((int32_t)mLastCameraPosition.x, (int32_t)mLastCameraPosition.y, (int32_t)mLastCameraPosition.z) / FChunk::CHUNK_SIZE;
	UpdateVisibleList();
}

void FChunkManager::Render(FRenderSystem& Renderer, const GLenum RenderMode)
{
	// Get camera data
	const Vector3f CameraDirection = FCamera::Main->Transform.GetRotation() * -Vector3f::Forward;
	const Vector3f CameraPosition = FCamera::Main->Transform.GetPosition();

	// Update the renderlist if the camera position or direction has changed.
	if (mLastCameraPosition != CameraPosition || mLastCameraDirection != CameraDirection)
	{
		mLastCameraPosition = CameraPosition;
		mLastCameraDirection = mLastCameraDirection;
		UpdateRenderList();
	}

	// Render everything in the renderlist
	for (const auto& Index : mRenderList)
	{
		if (mChunks[Index].IsLoaded())
		{
			const Vector3i IntPosition = IndexToWorldPosition(Index);
			const Vector3f Position((float)IntPosition.x, (float)IntPosition.y, (float)IntPosition.z);
			Renderer.SetModelTransform(FTransform{ Position });
			mChunks[Index].Render(RenderMode);
		}
	}
}

void FChunkManager::Update()
{
	UpdateLoadList();
	UpdateUnloadList();
	UpdateRebuildList();

	// Get the chunk that the camera is currently in.
	const Vector3f CameraPosition = FCamera::Main->Transform.GetPosition();
	const Vector3i CameraChunk =  Vector3i( (int32_t)CameraPosition.x,
											(int32_t)CameraPosition.y,
											(int32_t)CameraPosition.z) / FChunk::CHUNK_SIZE;

	// Only update visibility list when that camera crosses a chunk boundary
	if (mLastCameraChunk != CameraChunk)
	{
		mLastCameraChunk = CameraChunk;
		UpdateVisibleList();
	}
}

void FChunkManager::SetBlock(Vector3i Position, FBlock::BlockType BlockType)
{
	const Vector3i ChunkPosition = Position / FChunk::CHUNK_SIZE;
	Position = Vector3i{ Position.x % FChunk::CHUNK_SIZE, Position.y % FChunk::CHUNK_SIZE, Position.z % FChunk::CHUNK_SIZE };

	int32_t ChunkNumber = ChunkIndex(ChunkPosition);
	mChunks[ChunkNumber].SetBlock(Position, BlockType);

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

	mRebuildList.push_back(ChunkNumber);
}

void FChunkManager::SetPhysicsSystem(FPhysicsSystem& Physics)
{
	mPhysicsSystem = &Physics;
}

void FChunkManager::UpdateUnloadList()
{
	for (auto Itr = mUnloadList.begin(); Itr != mUnloadList.end(); Itr++)
	{
		// Unload the chunk
		const int32_t Index = *Itr;
		
		// Buffer for all chunk unloading data.
		std::vector<uint8_t> ChunkDataBuffer;
		mChunks[Index].Unload(ChunkDataBuffer, *mPhysicsSystem);

		// Get region position info
		const Vector3i ChunkPosition = IndexToChunkPosition(Index);
		const Vector3i RegionPosition = FRegionFile::ChunkToRegionPosition(ChunkPosition);
		const Vector3i LocalRegionPosition = FRegionFile::LocalRegionPosition(ChunkPosition);

		// Write the data to file
		mRegionFiles[RegionPosition].File.WriteChunkData(LocalRegionPosition, ChunkDataBuffer.data(), ChunkDataBuffer.size());

		// If it is in the loaded list, remove it
		auto Remove = std::find(mIsLoadedList.begin(), mIsLoadedList.end(), Index);
		if (Remove != mIsLoadedList.end())
			mIsLoadedList.erase(Remove);

		// If it is in the render list, remove it
		Remove = std::find(mRenderList.begin(), mRenderList.end(), Index);
		if (Remove != mRenderList.end())
			mRenderList.erase(Remove);

		RemoveRegionFileReference(IndexToChunkPosition(Index));
	}

	mUnloadList.clear();
}

void FChunkManager::UpdateLoadList()
{
	uint32_t LoadsLeft = CHUNKS_TO_LOAD_PER_FRAME;

	// Buffer for all chunk data
	std::vector<uint8_t> ChunkData;

	for (auto Itr = mLoadList.begin(); Itr != mLoadList.end() && LoadsLeft > 0; Itr++)
	{
		const int32_t Index = *Itr;

		// Get region position info
		Vector3i ChunkPosition = IndexToChunkPosition(Index);
		const Vector3i RegionPosition = FRegionFile::ChunkToRegionPosition(ChunkPosition);
		const Vector3i LocalRegionPosition = FRegionFile::LocalRegionPosition(ChunkPosition);
		
		// Get info for chunk data within its region
		uint32_t ChunkDataSize, ChunkSectorOffset;
		FRegionFile& RegionFile = mRegionFiles[RegionPosition].File;
		RegionFile.GetChunkDataInfo(LocalRegionPosition, ChunkDataSize, ChunkSectorOffset);
		
		// Obtain info for chunk
		ChunkData.resize(ChunkDataSize);
		RegionFile.GetChunkData(ChunkSectorOffset, ChunkData.data(), ChunkDataSize);

		// Load and build the chunk
		ChunkPosition *= FChunk::CHUNK_SIZE;
		const Vector3f FPosition{ (float)ChunkPosition.x, (float)ChunkPosition.y, (float)ChunkPosition.z };
		mChunks[Index].Load(ChunkData, *mPhysicsSystem, FPosition);
		mChunks[Index].RebuildMesh();

		// Add it to the loaded lists
		mIsLoadedList.push_back(Index);
		RemoveRegionFileReference(IndexToChunkPosition(Index));

		LoadsLeft--;
	}

	// Erase loaded chunks
	mLoadList.erase(mLoadList.begin(), mLoadList.begin() + (CHUNKS_TO_LOAD_PER_FRAME - LoadsLeft));
}

void FChunkManager::UpdateRebuildList()
{
	for (const auto& Chunk : mRebuildList)
	{
		mChunks[Chunk].RebuildMesh();
	}

	mRebuildList.clear();
}

void FChunkManager::UpdateVisibleList()
{
	// Offset the camera chunk position so the loop centers the camera
	Vector3i CameraChunkOffset = mLastCameraChunk - Vector3i{ VISIBILITY_DISTANCE, 0, VISIBILITY_DISTANCE };

	// Get the total range of visible area.
	const int32_t DoubleVisibility = VISIBILITY_DISTANCE * 2;

	// Clear previous load and visibile list when moving across chunks.
	mLoadList.clear();
	mVisibleList.clear();
	mRegionFiles.clear();

	// Add all chunks in the visible range to the visible list.
	// First add the xz plane that the camera is currently on.
	if (CameraChunkOffset.y < mWorldSize && CameraChunkOffset.y >= 0)
	{
		for (int32_t x = 0; x < DoubleVisibility; x++)
		{
			const int32_t xPosition = CameraChunkOffset.x + x;
			if (xPosition >= mWorldSize || xPosition < 0)
				continue;

			for (int32_t z = 0; z < DoubleVisibility; z++)
			{
				const int32_t zPosition = CameraChunkOffset.z + z;
				if (zPosition >= mWorldSize || zPosition < 0)
					continue;

				const int32_t VisibleChunkIndex = ChunkIndex(xPosition, CameraChunkOffset.y, zPosition);
				mVisibleList.push_back(VisibleChunkIndex);

				// If this visible chunk is not loaded, load it.
				if (!mChunks[VisibleChunkIndex].IsLoaded())
				{
					mLoadList.push_back(VisibleChunkIndex);
					AddRegionFileReference(xPosition, CameraChunkOffset.y, zPosition);
				}		
			}
		}
	}

	// Now add the world by alternating the xz planes from below to above the camera's chunk
	for (int32_t v = 1; v <= (int32_t)VISIBILITY_DISTANCE; v++)
	{
		for (int32_t y = -v; y < v + 1; y += 2 * v)
		{
			const int32_t yPosition = CameraChunkOffset.y + y;

			if (yPosition >= mWorldSize || yPosition < 0)
				continue;

			for (int32_t x = 0; x < DoubleVisibility; x++)
			{
				const int32_t xPosition = CameraChunkOffset.x + x;

				if (xPosition >= mWorldSize || xPosition < 0)
					continue;

				for (int32_t z = 0; z < DoubleVisibility; z++)
				{
					const int32_t zPosition = CameraChunkOffset.z + z;

					if (zPosition >= mWorldSize || zPosition < 0)
						continue;

					const int32_t VisibleChunkIndex = ChunkIndex(xPosition, yPosition, zPosition);
					mVisibleList.push_back(VisibleChunkIndex);

					// If this visible chunk is not loaded, load it.
					if (!mChunks[VisibleChunkIndex].IsLoaded())
					{
						mLoadList.push_back(VisibleChunkIndex);
						AddRegionFileReference(xPosition, yPosition, zPosition);
					}					
				}
			}
		}
	}

	// Add all loaded chunks that are no longer visible to the unload list.
	CameraChunkOffset.y -= VISIBILITY_DISTANCE;
	for (auto Itr = mIsLoadedList.begin(); Itr != mIsLoadedList.end(); Itr++)
	{
		// Get the chunk coordinates of this loaded chunk.
		Vector3i Position = IndexToWorldPosition(*Itr) / FChunk::CHUNK_SIZE;
		
		// Unload if it is loaded and outside of visible area.
		if (mChunks[*Itr].IsLoaded())
		{
			if (Position.x < CameraChunkOffset.x || Position.x >(CameraChunkOffset.x + DoubleVisibility) ||
				Position.y < CameraChunkOffset.y || Position.y >(CameraChunkOffset.y + DoubleVisibility) ||
				Position.z < CameraChunkOffset.z || Position.z >(CameraChunkOffset.z + DoubleVisibility))
			{
				mUnloadList.push_back(*Itr);
				AddRegionFileReference(Position.x, Position.y, Position.z);
			}
		}
	}
}

void FChunkManager::AddRegionFileReference(int32_t X, int32_t Y, int32_t Z)
{
	Vector3i RegionName = FRegionFile::ChunkToRegionPosition(X, Y, Z);

	// Increment if the file is loaded
	if (mRegionFiles.find(RegionName) != mRegionFiles.end())
	{
		mRegionFiles[RegionName].ReferenceCount++;
	}
	else
	{
		// Add the file if not loaded
		RegionFileRecord& Record = mRegionFiles[RegionName];
		Record.ReferenceCount = 1;
		Record.File.Load(L"GenWorld", RegionName);
	}
}

void FChunkManager::RemoveRegionFileReference(const Vector3i& ChunkPosition)
{
	Vector3i RegionName = FRegionFile::ChunkToRegionPosition(ChunkPosition);
	ASSERT(mRegionFiles.find(RegionName) != mRegionFiles.end() && "Shouldn't be removing a record that is not there.");

	// Decrement reference count and erase if 0
	RegionFileRecord& Record = mRegionFiles[RegionName];
	Record.ReferenceCount--;

	if (Record.ReferenceCount <= 0)
	{
		mRegionFiles.erase(RegionName);
	}
}

void FChunkManager::UpdateRenderList()
{
	// Start with a fresh list
	mRenderList.clear();

	// The the current view frustum
	const FFrustum ViewFrustum = FCamera::Main->GetWorldViewFrustum();
	const float ChunkHalfWidth = FChunk::CHUNK_SIZE / 2.0f;

	// Check each visible chunk against the frustum
	for (const auto& Chunk : mVisibleList)
	{
		const Vector3i UnsignedChunkCenter = IndexToWorldPosition(Chunk);
		const Vector3f Center = Vector3f{ (float)UnsignedChunkCenter.x + ChunkHalfWidth, 
										  (float)UnsignedChunkCenter.y + ChunkHalfWidth, 
										  (float)UnsignedChunkCenter.z + ChunkHalfWidth };

		if (ViewFrustum.IsUniformAABBVisible(Center, FChunk::CHUNK_SIZE) && !mChunks[Chunk].IsEmpty())
		{
			mRenderList.push_back(Chunk);
		}
	}
}
