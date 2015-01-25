#include "..\..\Include\Rendering\Chunk.h"
#include "..\..\Include\Rendering\UniformBlockStandard.h"
#include "..\..\Include\Debugging\ConsoleOutput.h"

FPoolAllocator<sizeof(FBlock) * FChunk::BLOCKS_PER_CHUNK, 500> FChunk::ChunkAllocator(__alignof(FChunk));

namespace
{
	uint32_t BlockIndex(uint32_t X, uint32_t Y, uint32_t Z)
	{
		return X * FChunk::CHUNK_SIZE * FChunk::CHUNK_SIZE + Y * FChunk::CHUNK_SIZE + Z;
	}
}

FChunk::FChunk()
	: mMesh()
	, mIsLoaded(false)
	, mBlocks(nullptr)
{
}

FChunk::~FChunk()
{
	if (mIsLoaded)
		ChunkAllocator.Free(mBlocks);
}

void FChunk::Load()
{
	ASSERT(!mIsLoaded);
	mIsLoaded = true;
	mBlocks = static_cast<FBlock*>(ChunkAllocator.Allocate());
	//FDebug::PrintF("Chunks Used: %d \tChunks Remaining: %d", ChunkAllocator.Size(), ChunkAllocator.Capacity() - ChunkAllocator.Size());

	FOR(x, CHUNK_SIZE)
	{
		FOR(y, CHUNK_SIZE)
		{
			FOR(z, CHUNK_SIZE)
			{
				mBlocks[BlockIndex(x, y, z)].SetActive(true);
			}
		}
	}
}

void FChunk::Unload()
{
	ASSERT(mIsLoaded);
	mIsLoaded = false;
	ChunkAllocator.Free(mBlocks);
	mMesh.Deactivate();
}

bool FChunk::IsLoaded() const
{
	return mIsLoaded;
}

extern FUniformBlockStandard* UniformBuffer;
void FChunk::Render(const Vector3f& WorldPosition)
{
	UniformBuffer->SetData(128, WorldPosition);
	UniformBuffer->SendBuffer();
	mMesh.Render();
}

void FChunk::BuildMesh()
{
	// Reset the mesh
	mMesh.Deactivate();
	FOR(x, CHUNK_SIZE)
	{
		FOR(y, CHUNK_SIZE)
		{
			FOR(z, CHUNK_SIZE)
			{
				if (!mBlocks[BlockIndex(x, y, z)].IsActive())
					continue;

				CreateCube(Vector3f((float)x, (float)y, (float)z));
			}
		}
	}
	mMesh.Activate();
}

void FChunk::CreateCube(const Vector3f& Position)
{
	// Positions
	const Vector3f P[8] =
	{
		Position + Vector3f(-FBlock::BLOCK_SIZE, -FBlock::BLOCK_SIZE, FBlock::BLOCK_SIZE),	// Bottom - Left -	Front
		Position + Vector3f(-FBlock::BLOCK_SIZE, FBlock::BLOCK_SIZE, FBlock::BLOCK_SIZE),	// Top -	Left -	Front
		Position + Vector3f(FBlock::BLOCK_SIZE, FBlock::BLOCK_SIZE, FBlock::BLOCK_SIZE),	// Top -	Right - Front
		Position + Vector3f(FBlock::BLOCK_SIZE, -FBlock::BLOCK_SIZE, FBlock::BLOCK_SIZE),	// Bottom -	Right - Front
		Position + Vector3f(-FBlock::BLOCK_SIZE, -FBlock::BLOCK_SIZE, -FBlock::BLOCK_SIZE),	// Bottom - Left -	Back
		Position + Vector3f(-FBlock::BLOCK_SIZE, FBlock::BLOCK_SIZE, -FBlock::BLOCK_SIZE),	// Top -	Left -	Back
		Position + Vector3f(FBlock::BLOCK_SIZE, FBlock::BLOCK_SIZE, -FBlock::BLOCK_SIZE),	// Top -	Right - Back
		Position + Vector3f(FBlock::BLOCK_SIZE, -FBlock::BLOCK_SIZE, -FBlock::BLOCK_SIZE)	// Bottom - Right - Back
	};

	// Normals
	static const Vector3f N[6] =
	{
		Vector3f(0, 0, 1),	// Front
		Vector3f(0, 1, 0),  // Top
		Vector3f(1, 0, 0),	// Right
		Vector3f(0, -1, 0),	// Bottom
		Vector3f(-1, 0, 0),	// Left
		Vector3f(0, 0, -1)	// Back
	};

	// Colors
	static const Vector3f C[6] =
	{
		Vector3f(0.1f, 0.55f, 0.2f),	// Front
		Vector3f(0.1f, 0.55f, 0.2f),	// Top
		Vector3f(0.1f, 0.55f, 0.2f),	// Right
		Vector3f(0.1f, 0.55f, 0.2f),	// Bottom
		Vector3f(0.1f, 0.55f, 0.2f),	// Left
		Vector3f(0.1f, 0.55f, 0.2f),	// Back
	};

	FVoxelVertex Vertices[24];
	// Front
	Vertices[0].Position = P[0];
	Vertices[1].Position = P[1];
	Vertices[2].Position = P[2];
	Vertices[3].Position = P[3];
	Vertices[0].Normal = Vertices[1].Normal = Vertices[2].Normal = Vertices[3].Normal = N[0];
	Vertices[0].Color = Vertices[1].Color = Vertices[2].Color = Vertices[3].Color = C[0];

	// Back
	Vertices[4].Position = P[4];
	Vertices[5].Position = P[5];
	Vertices[6].Position = P[6];
	Vertices[7].Position = P[7];
	Vertices[4].Normal = Vertices[5].Normal = Vertices[6].Normal = Vertices[7].Normal = N[5];
	Vertices[4].Color = Vertices[5].Color = Vertices[6].Color = Vertices[7].Color = C[5];

	// Left
	Vertices[8].Position = P[0];
	Vertices[9].Position = P[1];
	Vertices[10].Position = P[4];
	Vertices[11].Position = P[5];
	Vertices[8].Normal = Vertices[9].Normal = Vertices[10].Normal = Vertices[11].Normal = N[4];
	Vertices[8].Color = Vertices[9].Color = Vertices[10].Color = Vertices[11].Color = C[4];

	// Right
	Vertices[12].Position = P[2];
	Vertices[13].Position = P[3];
	Vertices[14].Position = P[6];
	Vertices[15].Position = P[7];
	Vertices[12].Normal = Vertices[13].Normal = Vertices[14].Normal = Vertices[15].Normal = N[2];
	Vertices[12].Color = Vertices[13].Color = Vertices[14].Color = Vertices[15].Color = C[2];

	// Top
	Vertices[16].Position = P[1];
	Vertices[17].Position = P[2];
	Vertices[18].Position = P[5];
	Vertices[19].Position = P[6];
	Vertices[16].Normal = Vertices[17].Normal = Vertices[18].Normal = Vertices[19].Normal = N[1];
	Vertices[16].Color = Vertices[17].Color = Vertices[18].Color = Vertices[19].Color = C[1];

	// Bottom
	Vertices[20].Position = P[0];
	Vertices[21].Position = P[3];
	Vertices[22].Position = P[4];
	Vertices[23].Position = P[7];
	Vertices[20].Normal = Vertices[21].Normal = Vertices[22].Normal = Vertices[23].Normal = N[3];
	Vertices[20].Color = Vertices[21].Color = Vertices[22].Color = Vertices[23].Color = C[3];


	static const uint32_t BaseIndices[36] =
	{
		1, 0, 3,	// Front
		1, 3, 2,
		18, 17, 19, // Top
		18, 16, 17,
		13, 15, 14, // Right
		12, 13, 14,
		20, 22, 21, // Bottom
		22, 23, 21,
		8, 9, 10,	// Left
		10, 9, 11,
		7, 4, 5,	// Back
		7, 5, 6,
	};

	uint32_t BaseIndex = mMesh.AddVertex(Vertices, 24);

	// Calculate indices with offset
	uint32_t VertexIndices[36];
	FOR(i, 36)
		VertexIndices[i] = BaseIndex + BaseIndices[i];

	mMesh.AddTriangle(VertexIndices, 12);
}