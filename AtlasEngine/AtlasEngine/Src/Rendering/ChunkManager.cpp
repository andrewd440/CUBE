#include "..\..\Include\Rendering\ChunkManager.h"
#include "..\..\Include\Input\ButtonEvent.h"

namespace
{
	uint32_t ChunkIndex(uint32_t X, uint32_t Y, uint32_t Z)
	{
		return Y * FChunkManager::WORLD_HEIGHT * FChunkManager::WORLD_HEIGHT + X * FChunkManager::WORLD_WIDTH + Z;
	}
}

FChunkManager::FChunkManager()
	: mChunks(WORLD_WIDTH * WORLD_WIDTH * WORLD_HEIGHT)
{
	ASSERT((WORLD_WIDTH & (WORLD_WIDTH - 1)) == 0x0 && "World width must be a power of two.");
	FOR(i, WORLD_WIDTH * WORLD_WIDTH * WORLD_HEIGHT)
		mRenderList.push_back(i);
}


FChunkManager::~FChunkManager()
{
}


void FChunkManager::Setup()
{
	for (std::size_t i = 0; i < mChunks.size(); i++)
	{
		mChunks[i].Load();
		mChunks[i].BuildMesh();
	}
}

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

void FChunkManager::Render()
{
	// World diminsions will be powers of two, so chunk locations can be 
	// calculated by shifting indexes and bitwise operations to avoid division.
	static const uint32_t WidthShift = CalcShift(WORLD_WIDTH);

	for (const auto& Index : mRenderList)
	{
		const uint32_t X = (Index >> WidthShift & (WORLD_WIDTH - 1)) * FChunk::CHUNK_SIZE;
		const uint32_t Y = (Index >> WORLD_WIDTH) * FChunk::CHUNK_SIZE;
		const uint32_t Z = (Index & (WORLD_WIDTH - 1)) * FChunk::CHUNK_SIZE;
		mChunks[Index].Render(Vector3f(X, Y, Z));
	}
}