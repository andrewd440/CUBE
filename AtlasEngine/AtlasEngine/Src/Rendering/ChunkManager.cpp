#include "Rendering\ChunkManager.h"
#include "Input\ButtonEvent.h"
#include "Debugging\ConsoleOutput.h"
#include "Rendering\GLUtils.h"
#include "ResourceHolder.h"

namespace
{	
	/**
	* Converts a 3D chunk position into an index into the chunks array.
	*/
	uint32_t ChunkIndex(uint32_t X, uint32_t Y, uint32_t Z)
	{
		return Y * FChunkManager::WORLD_SIZE * FChunkManager::WORLD_SIZE + X * FChunkManager::WORLD_SIZE + Z;
	}

	/**
	* Converts a 3D chunk position into an index into the chunks array.
	*/
	uint32_t ChunkIndex(Vector3i Position)
	{
		return ChunkIndex(Position.x, Position.y, Position.z);
	}

	/**
	* Determines the amount of bit-shift require to
	* divide by a power of 2.
	*/
	uint32_t CalcShift(uint32_t Value)
	{
		uint32_t Shift = 0;
		while (Value > 1)
		{
			Value = Value >> 1;
			Shift++;
		}

		return Shift;
	}

	/**
	* Convertes a chunk index into a world space 3D position.
	*/
	Vector3ui IndexToPosition(uint32_t Index)
	{
		// World diminsions will be powers of two, so chunk locations can be 
		// calculated by shifting indexes and bitwise operations to avoid division.
		static const uint32_t WidthShift = CalcShift(FChunkManager::WORLD_SIZE);
		static const uint32_t HeightShift = CalcShift(FChunkManager::WORLD_SIZE * FChunkManager::WORLD_SIZE);

		const uint32_t X = (Index >> WidthShift & (FChunkManager::WORLD_SIZE - 1)) * FChunk::CHUNK_SIZE;
		const uint32_t Y = (Index >> HeightShift & ((FChunkManager::WORLD_SIZE << 1) - 1)) * FChunk::CHUNK_SIZE;
		const uint32_t Z = (Index & (FChunkManager::WORLD_SIZE - 1)) * FChunk::CHUNK_SIZE;

		return Vector3ui(X, Y, Z);
	}
}

const uint32_t FChunkManager::WORLD_SIZE = 256;
const uint32_t FChunkManager::VISIBILITY_DISTANCE = 10;
const uint32_t FChunkManager::CHUNKS_TO_LOAD_PER_FRAME = 10;

extern Vector3f gEyePosition;

FChunkManager::FChunkManager()
	: mChunks(WORLD_SIZE * WORLD_SIZE * WORLD_SIZE)
	, mVisibleList()
	, mRenderList()
	, mLoadList()
	, mIsLoadedList()
	, mUnloadList()
	, mRebuildList()
	, mShader()
	, mLastCameraChunk(TVector3<uint32_t>((uint32_t)gEyePosition.x, (uint32_t)gEyePosition.y, (uint32_t)gEyePosition.z) / FChunk::CHUNK_SIZE)
	, mNoiseMap()
{
	ASSERT((WORLD_SIZE & (WORLD_SIZE - 1)) == 0x0 && "World width must be a power of two.");

	auto& ShaderHolder = FShaderHolder::GetInstance();
	mShader.AttachShader(ShaderHolder.Get("BlinnVertex"));
	mShader.AttachShader(ShaderHolder.Get("BlinnFragment"));
	mShader.LinkProgram();

	// SEtup noise map
	noise::module::Perlin Perlin;
	Perlin.SetOctaveCount(1);
	Perlin.SetPersistence(.2);
	noise::utils::NoiseMapBuilderPlane MapBuilder;
	MapBuilder.SetSourceModule(Perlin);
	MapBuilder.SetDestNoiseMap(mNoiseMap);
	MapBuilder.SetDestSize(FChunkManager::WORLD_SIZE * FChunk::CHUNK_SIZE, FChunkManager::WORLD_SIZE * FChunk::CHUNK_SIZE);
	MapBuilder.SetBounds(0.0, FChunkManager::WORLD_SIZE, 0.0, FChunkManager::WORLD_SIZE);
	MapBuilder.Build();

}


FChunkManager::~FChunkManager()
{
}

float FChunkManager::GetNoiseHeight(uint32_t x, uint32_t z)
{
	static const int32_t MidHeight = (FChunkManager::WORLD_SIZE * FChunk::CHUNK_SIZE) / 2;
	static const int32_t HeightVariation = 10;
	const int32_t IndexOffset = x + (z * FChunkManager::WORLD_SIZE * FChunk::CHUNK_SIZE);
	return MidHeight + *(mNoiseMap.GetConstSlabPtr() + IndexOffset) * HeightVariation;
}

void FChunkManager::Setup()
{
	// Load all chunks surrounding the camera's starting position.
	TVector3<uint32_t> CameraChunk = mLastCameraChunk;
	CameraChunk -= VISIBILITY_DISTANCE;

	const uint32_t DoubleVisibility = VISIBILITY_DISTANCE * 2;

	// Add all chunks in the visible range to the visible list.
	for (int32_t Y = VISIBILITY_DISTANCE; Y >= 0; Y--)
	{
		for (int32_t X = 0; X <= DoubleVisibility; X++)
		{
			for (int32_t Z = 0; Z <= DoubleVisibility; Z++)
			{
				const uint32_t VisibleChunkIndex = ChunkIndex(CameraChunk.x + X, CameraChunk.y + Y, CameraChunk.z + Z);
				mVisibleList.push_back(VisibleChunkIndex);
				mLoadList.push_back(VisibleChunkIndex);
			}
		}
	}

	for (int32_t Y = VISIBILITY_DISTANCE + 1; Y < DoubleVisibility; Y++)
	{
		for (int32_t X = 0; X <= DoubleVisibility; X++)
		{
			for (int32_t Z = 0; Z <= DoubleVisibility; Z++)
			{
				const uint32_t VisibleChunkIndex = ChunkIndex(CameraChunk.x + X, CameraChunk.y + Y, CameraChunk.z + Z);
				mVisibleList.push_back(VisibleChunkIndex);
				mLoadList.push_back(VisibleChunkIndex);
			}
		}
	}
}

void FChunkManager::Render()
{
	mShader.Use();

	for (const auto& Index : mRenderList)
	{
		auto Fposition = IndexToPosition(Index);
		const Vector3f Position((float)Fposition.x, (float)Fposition.y, (float)Fposition.z);
		mChunks[Index].Render(Position);
	}
}

void FChunkManager::Update()
{
	UpdateVisibleList();
	UpdateLoadList();
	UpdateUnloadList();
}

void FChunkManager::UpdateUnloadList()
{
	for (auto Itr = mUnloadList.begin(); Itr != mUnloadList.end(); Itr++)
	{
		// Unload the chunk
		const uint32_t Index = *Itr;
		mChunks[Index].Unload();

		// If it is in the loaded list, remove it
		auto Remove = std::find(mIsLoadedList.begin(), mIsLoadedList.end(), Index);
		if (Remove != mIsLoadedList.end())
			mIsLoadedList.erase(Remove);

		// If it is in the render list, remove it
		Remove = std::find(mRenderList.begin(), mRenderList.end(), Index);
		if (Remove != mRenderList.end())
			mRenderList.erase(Remove);
	}

	mUnloadList.clear();
}

void FChunkManager::UpdateLoadList()
{
	uint32_t LoadsLeft = CHUNKS_TO_LOAD_PER_FRAME;

	for (auto Itr = mLoadList.begin(); Itr != mLoadList.end() && LoadsLeft > 0; Itr++)
	{
		const uint32_t Index = *Itr;

		// Load and build the chunk
		mChunks[Index].Load(IndexToPosition(Index));
		mChunks[Index].RebuildMesh();

		// Add it to the loaded and render lists
		mIsLoadedList.push_back(Index);
		mRenderList.push_back(Index);

		LoadsLeft--;
	}

	// Erase loaded chunks
	mLoadList.erase(mLoadList.begin(), mLoadList.begin() + (CHUNKS_TO_LOAD_PER_FRAME - LoadsLeft));
}

void FChunkManager::UpdateVisibleList()
{
	// Chunk coordinate of the camera
	const TVector3<uint32_t> CameraChunk = TVector3<uint32_t>(	(uint32_t)gEyePosition.x, 
																(uint32_t)gEyePosition.y, 
																(uint32_t)gEyePosition.z) / FChunk::CHUNK_SIZE;
	
	// Only update visibility list when we move to another chunk
	if (CameraChunk == mLastCameraChunk)
		return;
	
	// Offset the camera chunk position so the loop centers the camera
	const TVector3<uint32_t> CameraChunkOffset = CameraChunk - VISIBILITY_DISTANCE;

	// Get the total range of visible area.
	const uint32_t DoubleVisibility = VISIBILITY_DISTANCE * 2;

	// Clear previous load and visibile list when moving across chunks.
	mLoadList.clear();
	mVisibleList.clear();

	// Add all chunks in the visible range to the visible list.
	for (int32_t Y = VISIBILITY_DISTANCE; Y >= 0; Y--)
	{
		for (int32_t X = 0; X <= DoubleVisibility; X++)
		{
			for (int32_t Z = 0; Z <= DoubleVisibility; Z++)
			{
				const uint32_t VisibleChunkIndex = ChunkIndex(CameraChunkOffset.x + X, CameraChunkOffset.y + Y, CameraChunkOffset.z + Z);
				mVisibleList.push_back(VisibleChunkIndex);

				// If this visible chunk is not loaded, load it.
				if (!mChunks[VisibleChunkIndex].IsLoaded())
					mLoadList.push_back(VisibleChunkIndex);
			}
		}
	}

	for (int32_t Y = VISIBILITY_DISTANCE + 1; Y < DoubleVisibility; Y++)
	{
		for (int32_t X = 0; X <= DoubleVisibility; X++)
		{
			for (int32_t Z = 0; Z <= DoubleVisibility; Z++)
			{
				const uint32_t VisibleChunkIndex = ChunkIndex(CameraChunkOffset.x + X, CameraChunkOffset.y + Y, CameraChunkOffset.z + Z);
				mVisibleList.push_back(VisibleChunkIndex);

				// If this visible chunk is not loaded, load it.
				if (!mChunks[VisibleChunkIndex].IsLoaded())
					mLoadList.push_back(VisibleChunkIndex);
			}
		}
	}

	// Add all loaded chunks that are no longer visible to the unload list.
	for (auto Itr = mIsLoadedList.begin(); Itr != mIsLoadedList.end(); Itr++)
	{
		// Get the chunk coordinates of this loaded chunk.
		Vector3ui Position = IndexToPosition(*Itr) / FChunk::CHUNK_SIZE;
		
		// Unload if it is loaded and outside of visible area.
		if (mChunks[*Itr].IsLoaded())
		{
			if (Position.x < CameraChunkOffset.x || Position.x >(CameraChunkOffset.x + DoubleVisibility) ||
				Position.y < CameraChunkOffset.y || Position.y >(CameraChunkOffset.y + DoubleVisibility) ||
				Position.z < CameraChunkOffset.z || Position.z >(CameraChunkOffset.z + DoubleVisibility))
			{
				mUnloadList.push_back(*Itr);
			}
		}
	}

	mLastCameraChunk = CameraChunk;
}

void FChunkManager::UpdateRenderList()
{

}
