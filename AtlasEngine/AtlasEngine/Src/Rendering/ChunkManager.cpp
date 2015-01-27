#include "..\..\Include\Rendering\ChunkManager.h"
#include "..\..\Include\Input\ButtonEvent.h"
#include "..\..\Include\Debugging\ConsoleOutput.h"
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
	//FOR(i, WORLD_WIDTH * WORLD_WIDTH * WORLD_HEIGHT)
	//	mRenderList.push_back(i);
}


FChunkManager::~FChunkManager()
{
}


void FChunkManager::Setup()
{
	//for (std::size_t i = 0; i < mChunks.size(); i++)
	//{
	//	mChunks[i].Load();
	//	mChunks[i].BuildMesh();
	//}
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
	static const uint32_t HeightShift = CalcShift(WORLD_WIDTH * WORLD_WIDTH);

	for (const auto& Index : mRenderList)
	{
		const uint32_t X = (Index >> WidthShift & (WORLD_WIDTH - 1)) * FChunk::CHUNK_SIZE;
		const uint32_t Y = (Index >> HeightShift & ((WORLD_WIDTH<<1) - 1)) * FChunk::CHUNK_SIZE;
		const uint32_t Z = (Index & (WORLD_WIDTH - 1)) * FChunk::CHUNK_SIZE;
		mChunks[Index].Render(Vector3f((float)X, (float)Y, (float)Z));
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
	while (!mUnloadList.empty())
	{
		FDebug::PrintF("Unloaded");
		mChunks[mUnloadList.front()].Unload();
		mUnloadList.pop();
	}
}

void FChunkManager::UpdateLoadList()
{
	while (!mLoadList.empty())
	{
		mChunks[mLoadList.front()].Load();
		mChunks[mLoadList.front()].BuildMesh();
		mRenderList.push_back(mLoadList.front());
		mLoadList.pop();
	}
}

extern Vector3f Eye;
void FChunkManager::UpdateVisibleList()
{
	Vector3i CameraChunk = Vector3i(Eye.x, Eye.y, Eye.z) / 16;
	CameraChunk.x -= 3; CameraChunk.z -= 3;
	FOR(y, 1)
	{
		FOR(x, 6)
		{
			FOR(z, 6)
			{
				if (!mChunks[ChunkIndex(CameraChunk.x + x, y, CameraChunk.z + z)].IsLoaded())
					mLoadList.push(ChunkIndex(CameraChunk.x + x, y, CameraChunk.z + z));
			}
		}
	}

	CameraChunk.x -= 1; CameraChunk.z -= 1;
	for (int y = 0; y < 1; y++)
	{
		for (int x = 0; x < 8; x+=7)
		{
			for (int z = 0; z < 8; z++)
			{
				if (mChunks[ChunkIndex(CameraChunk.x + x, y, CameraChunk.z + z)].IsLoaded())
				{
					mUnloadList.push(ChunkIndex(CameraChunk.x + x, y, CameraChunk.z + z));
					auto& Remove = std::find(mRenderList.begin(), mRenderList.end(), ChunkIndex(CameraChunk.x + x, y, CameraChunk.z + z));
					if (Remove != mRenderList.end())
						mRenderList.erase(Remove);

				}
			}
		}
	}

	for (int y = 0; y < 1; y++)
	{
		for (int z = 0; z < 8; z += 7)
		{
			for (int x = 1; x < 7; x++)
			{
				if (mChunks[ChunkIndex(CameraChunk.x + x, y, CameraChunk.z + z)].IsLoaded())
				{
					mUnloadList.push(ChunkIndex(CameraChunk.x + x, y, CameraChunk.z + z));
					auto& Remove = std::find(mRenderList.begin(), mRenderList.end(), ChunkIndex(CameraChunk.x + x, y, CameraChunk.z + z));
					if (Remove != mRenderList.end())
						mRenderList.erase(Remove);

				}
			}
		}
	}
}

void FChunkManager::UpdateRenderList()
{

}