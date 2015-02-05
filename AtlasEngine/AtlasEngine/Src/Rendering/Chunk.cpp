#include "..\..\Include\Rendering\Chunk.h"
#include "..\..\Include\Rendering\UniformBlockStandard.h"
#include "..\..\Include\Debugging\ConsoleOutput.h"
#include "Debugging\DebugText.h"
#include "Rendering\Screen.h"
#include "Rendering\ChunkManager.h"

FPoolAllocator<sizeof(FBlock) * FChunk::BLOCKS_PER_CHUNK, 10000> FChunk::ChunkAllocator(__alignof(FChunk));
FPoolAllocatorType<TMesh<FVoxelVertex>, 10000> FChunk::MeshAllocator(__alignof(TMesh<FVoxelVertex>));

namespace
{
	/**
	* Returns the index of a block in the mBlocks array based on 3D coordinates within the chunk.
	*/
	uint32_t BlockIndex(uint32_t X, uint32_t Y, uint32_t Z)
	{
		ASSERT((X * FChunk::CHUNK_SIZE * FChunk::CHUNK_SIZE + Y * FChunk::CHUNK_SIZE + Z) >= 0 && (X * FChunk::CHUNK_SIZE * FChunk::CHUNK_SIZE + Y * FChunk::CHUNK_SIZE + Z) < FChunk::BLOCKS_PER_CHUNK);
		return (X * FChunk::CHUNK_SIZE * FChunk::CHUNK_SIZE) + (Y * FChunk::CHUNK_SIZE) + Z;
	}

	/**
	* Returns the index of a block in the mBlocks array based on 3D coordinates within the chunk.
	*/
	uint32_t BlockIndex(Vector3i Position)
	{
		return BlockIndex((uint32_t)Position.x, (uint32_t)Position.y, (uint32_t)Position.z);
	}
}

FChunk::FChunk()
	: mBlocks(nullptr)
	, mMesh(nullptr)
	, mIsLoaded(false)
{
}

FChunk::FChunk(const FChunk& Other)
	: mBlocks(Other.mBlocks)
	, mMesh(Other.mMesh)
	, mIsLoaded(Other.mIsLoaded)
{
}

FChunk::~FChunk()
{
	if (mIsLoaded)
	{
		ChunkAllocator.Free(mBlocks);
		MeshAllocator.Free(mMesh);
	}
}

FChunk& FChunk::operator=(const FChunk& Other)
{
	// Free current data
	ChunkAllocator.Free(mBlocks);
	mMesh->~TMesh();
	MeshAllocator.Free(mMesh);

	// Copy Other data
	mBlocks = Other.mBlocks;
	mMesh = Other.mMesh;
	mIsLoaded = Other.mIsLoaded;

	return *this;
}

void FChunk::Load(const Vector3ui& LowerLeftPosition)
{
	ASSERT(!mIsLoaded);
	mIsLoaded = true;

	// In-place allocate mesh and allocate blocks
	mMesh = new (MeshAllocator.Allocate()) TMesh<FVoxelVertex>(GL_STATIC_DRAW);
	mBlocks = static_cast<FBlock*>(ChunkAllocator.Allocate());

	FOR(x, CHUNK_SIZE)
	{
		FOR(z, CHUNK_SIZE)
		{
			const uint32_t Height = (uint32_t)FChunkManager::GetInstance().GetNoiseHeight(LowerLeftPosition.x + x, LowerLeftPosition.z + z);
			uint32_t y = LowerLeftPosition.y;
			for (; y < Height && (y - LowerLeftPosition.y) < CHUNK_SIZE; y++)
			{
				uint32_t Index = BlockIndex(x, y - LowerLeftPosition.y, z);
				mBlocks[Index].SetActive(true);
				mBlocks[Index].SetType(FBlock::BlockType::Grass);
				if (y > ((FChunkManager::WORLD_SIZE * FBlock::BLOCK_SIZE) / 2 + 1925))
					mBlocks[Index].SetType(FBlock::BlockType::Snow);
			}
			for (; y < LowerLeftPosition.y + CHUNK_SIZE; y++)
			{
				uint32_t Index = BlockIndex(x, y - LowerLeftPosition.y, z);
				mBlocks[Index].SetActive(false);
				mBlocks[Index].SetType(FBlock::BlockType::Grass);
			}
		}
	}
}

void FChunk::Unload()
{
	ASSERT(mIsLoaded);

	mIsLoaded = false;

	// DON'T forget the run the mesh destructor.
	MeshAllocator.Free(mMesh);
	ChunkAllocator.Free(mBlocks);
}

bool FChunk::IsLoaded() const
{
	return mIsLoaded;
}

extern FUniformBlockStandard* UniformBuffer;
void FChunk::Render(const Vector3f& WorldPosition)
{
	ASSERT(mIsLoaded);
	UniformBuffer->SetData(128, WorldPosition);
	UniformBuffer->SendBuffer();
	mMesh->Render();
}

void FChunk::RebuildMesh()
{
	// Make sure mesh data is cleared
	mMesh->ClearData();
	GreedyMesh();
}

void FChunk::GreedyMesh()
{
	// Greedy mesh algorithm by Mikola Lysenko from http://0fps.net/2012/06/30/meshing-in-a-minecraft-game/
	// Java implementation from https://github.com/roboleary/GreedyMesh/blob/master/src/mygame/Main.java

	// Vertex and index data to be sent to the mesh
	std::vector<FVoxelVertex> Vertices;
	std::vector<uint32_t> Indices;

	// Variables to be used by the algorithm
	int32_t i, j, k, Length, Width, Height, u, v, n, Side = 0;
	int32_t x[3], q[3], du[3], dv[3];

	// This mask will contain matching voxel faces as we
	// loop through each direction in the chunk
	FBlock Mask[CHUNK_SIZE * CHUNK_SIZE];

	// Start with a for loop the will flip face direction once we iterate through
	// the chunk in one direction.
	for (bool BackFace = true, b = false; b != BackFace; BackFace = BackFace && b, b = !b)
	{

		// Iterate through each dimension of the chunk
		for (int32_t d = 0; d < 3; d++)
		{
			// Get the other 2 axes
			u = (d + 1) % 3;
			v = (d + 2) % 3;

			x[0] = 0;
			x[1] = 0;
			x[2] = 0;

			// Used to check against covering voxels
			q[0] = 0;
			q[1] = 0;
			q[2] = 0;
			q[d] = 1;

			if (d == 0)
			{ 
				Side = BackFace ? WEST : EAST; 
			}
			else if (d == 1) 
			{ 
				Side = BackFace ? BOTTOM : TOP; 
			}
			else if (d == 2) 
			{ 
				Side = BackFace ? SOUTH : NORTH; 
			}

			// Move through the dimension from front to back
			for (x[d] = -1; x[d] < (int32_t)CHUNK_SIZE;)
			{
				// Compute mask
				n = 0;

				for (x[v] = 0; x[v] < (int32_t)CHUNK_SIZE; x[v]++)
				{
					for (x[u] = 0; x[u] < (int32_t)CHUNK_SIZE; x[u]++)
					{
						// Check covering voxel
						FBlock Voxel1 = (x[d] >= 0) ? mBlocks[BlockIndex(x[0], x[1], x[2])] : FBlock{};
						FBlock Voxel2 = (x[d] < (int32_t)CHUNK_SIZE - 1) ? mBlocks[BlockIndex(x[0] + q[0], x[1] + q[1], x[2] + q[2])] : FBlock{};

						// If both voxels are active and the same type, mark the mask with an inactive block, if not
						// choose the appropriate voxel to mark
						Mask[n++] = ((Voxel1.IsActive() && Voxel2.IsActive()) && (Voxel1.GetType() == Voxel2.GetType())) ? FBlock{} : (BackFace ? Voxel2 : Voxel1);
					}
				}

				x[d]++;

				// Generate the mesh for the mask
				n = 0;

				for (j = 0; j < (int32_t)CHUNK_SIZE; j++)
				{
					for (i = 0; i < (int32_t)CHUNK_SIZE;)
					{
						if (Mask[n].IsActive())
						{
							// Compute the width
							for (Width = 0; i + Width < (int32_t)CHUNK_SIZE && Mask[n + Width] == Mask[n]; Width++){}

							// Compute Height
							bool Done = false;

							for (Height = 1; j + Height < (int32_t)CHUNK_SIZE; Height++)
							{
								for (k = 0; k < Width; k++)
								{
									if (!Mask[n + k + Height*(int32_t)CHUNK_SIZE].IsActive() || Mask[n + k + Height*(int32_t)CHUNK_SIZE].GetType() != Mask[n].GetType())
									{
										Done = true;
										break;
									}
								}

								if (Done) 
									break;
							}


							// Add the quad
							x[u] = i;
							x[v] = j;

							du[0] = 0;
							du[1] = 0;
							du[2] = 0;
							du[u] = Width;

							dv[0] = 0;
							dv[1] = 0;
							dv[2] = 0;
							dv[v] = Height;

							AddQuad(Vector3f((float)x[0], (float)x[1], (float)x[2]),
								Vector3f((float)x[0] + (float)du[0], (float)x[1] + (float)du[1], (float)x[2] + (float)du[2]),
								Vector3f((float)x[0] + (float)du[0] + (float)dv[0], (float)x[1] + (float)du[1] + (float)dv[1], (float)x[2] + (float)du[2] + (float)dv[2]),
								Vector3f((float)x[0] + (float)dv[0], (float)x[1] + (float)dv[1], (float)x[2] + (float)dv[2]),
								BackFace,
								Side,
								Mask[n].GetType(),
								Vertices,
								Indices);


							// Zero the mask
							for (Length = 0; Length < Height; Length++)
							{
								for (k = 0; k < Width; k++)
								{
									Mask[n + k + Length * (int32_t)CHUNK_SIZE] = FBlock{};
								}
							}

							// Increment counters
							i += Width;
							n += Width;
						}
						else
						{
							i++;
							n++;
						}
					}
				}
			}
		}
	}

	// Send all data directly to the gpu
	mMesh->MapAndActivate(Vertices.data(), Vertices.size(), Indices.data(), Indices.size());
}

void FChunk::AddQuad(	const Vector3f BottomLeft,
						const Vector3f TopLeft,
						const Vector3f TopRight,
						const Vector3f BottomRight,
						bool IsBackface,
						uint32_t Side,
						const FBlock::BlockType BlockType,
						std::vector<FVoxelVertex>& VerticesOut,
						std::vector<uint32_t>& IndicesOut)
{
	Vector3f Normal;
	switch (Side)
	{
	case WEST:
		Normal = Vector3f(-1, 0, 0);
		break;
	case EAST:
		Normal = Vector3f(1, 0, 0);
		break;
	case NORTH:
		Normal = Vector3f(0, 0, 1);
		break;
	case SOUTH:
		Normal = Vector3f(0, 0, -1);
		break;
	case TOP:
		Normal = Vector3f(0, 1, 0);
		break;
	case BOTTOM:
		Normal = Vector3f(0, -1, 0);
		break;
	}
	
	// Get the index offset by checking the size of the vertex list.
	uint32_t BaseIndex = VerticesOut.size();
	VerticesOut.insert(VerticesOut.end(), { { BottomLeft,	Normal, FBlock::Colors[BlockType] },
											{ BottomRight,	Normal, FBlock::Colors[BlockType] },
											{ TopLeft,		Normal, FBlock::Colors[BlockType] },
											{ TopRight,		Normal, FBlock::Colors[BlockType] } });

	// Add adjusted indices.
	if (IsBackface)
	{
		IndicesOut.insert(IndicesOut.end(), {	2 + BaseIndex, 
												0 + BaseIndex, 
												1 + BaseIndex, 
												1 + BaseIndex, 
												3 + BaseIndex, 
												2 + BaseIndex });
	}
	else
	{
		IndicesOut.insert(IndicesOut.end(), {	2 + BaseIndex, 
												3 + BaseIndex, 
												1 + BaseIndex, 
												1 + BaseIndex, 
												0 + BaseIndex, 
												2 + BaseIndex });
	}

}