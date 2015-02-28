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

	uint32_t BlockIndex(Vector3ui Position)
	{
		return BlockIndex(Position.x, Position.y, Position.z);
	}
}

FChunk::FChunk()
	: mBlocks(nullptr)
	, mMesh(nullptr)
	, mChunkManager(nullptr)
	, mIsLoaded(false)
	, mIsEmpty(true)
{
}

FChunk::FChunk(const FChunk& Other)
	: mBlocks(Other.mBlocks)
	, mMesh(Other.mMesh)
	, mChunkManager(Other.mChunkManager)
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
	mChunkManager = Other.mChunkManager;

	return *this;
}

void FChunk::SetChunkManager(FChunkManager* NewManager)
{
	mChunkManager = NewManager;
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
				mBlocks[Index].Type = FBlock::Grass;

				if (y > (8 + (FChunkManager::WORLD_SIZE * CHUNK_SIZE * FBlock::BLOCK_SIZE) / 2))
					mBlocks[Index].Type = FBlock::Snow;
			}
			for (; y < LowerLeftPosition.y + CHUNK_SIZE; y++)
			{
				uint32_t Index = BlockIndex(x, y - LowerLeftPosition.y, z);
				mBlocks[Index].Type = FBlock::None;
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

void FChunk::Render(const GLenum RenderMode)
{
	ASSERT(mIsLoaded);
	mMesh->Render(RenderMode);
}

void FChunk::RebuildMesh()
{
	// Make sure mesh data is cleared
	mMesh->ClearData();
	GreedyMesh();
}

void FChunk::SetBlock(Vector3ui Position, FBlock::BlockType BlockType)
{
	mBlocks[BlockIndex(Position)].Type = BlockType;
}

FBlock::BlockType FChunk::GetBlock(Vector3ui Position) const
{
	if (mBlocks)
		return mBlocks[BlockIndex(Position)].Type;

	return FBlock::None;
}

void FChunk::DestroyBlock(Vector3ui Position)
{
	mBlocks[BlockIndex(Position)].Type = FBlock::None;
}

void FChunk::GreedyMesh()
{
	// Greedy mesh algorithm by Mikola Lysenko from http://0fps.net/2012/06/30/meshing-in-a-minecraft-game/
	// Java implementation from https://github.com/roboleary/GreedyMesh/blob/master/src/mygame/Main.java

	// Cache position of this chunk to check neiboring chunks in ambient occlusion tests
	//const Vector3ui ChunkPosition = mChunkManager->GetChunkPosition(this);

	// Vertex and index data to be sent to the mesh
	std::vector<FVoxelVertex> Vertices;
	std::vector<uint32_t> Indices;

	// Variables to be used by the algorithm
	int32_t i, j, k, Length, Width, Height, u, v, n, Side = 0;
	int32_t x[3], q[3], du[3], dv[3];

	// This mask will contain matching voxel faces as we
	// loop through each direction in the chunk
	MaskInfo Mask[CHUNK_SIZE * CHUNK_SIZE];

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
						FBlock Voxel1 = (x[d] >= 0) ? mBlocks[BlockIndex(x[0], x[1], x[2])] : FBlock{ FBlock::None };
						FBlock Voxel2 = (x[d] < (int32_t)CHUNK_SIZE - 1) ? mBlocks[BlockIndex(x[0] + q[0], x[1] + q[1], x[2] + q[2])] : FBlock{ FBlock::None };

						// If both voxels are active and the same type, mark the mask with an inactive block, if not
						// choose the appropriate voxel to mark
						if (Voxel1 == Voxel2)
							Mask[n++].Block = FBlock{ FBlock::None };
						else
						{
							int32_t DepthOffset = 0;
							Vector3i BlockPosition;
							
							// Set the correct block in the mask and get the direction we need to check for surrounding
							// blocks for ambient occlusion calculations
							if (BackFace)
							{
								Mask[n].Block = Voxel2;
								DepthOffset = -1;
								BlockPosition = Vector3i{ x[0] + q[0], x[1] + q[1], x[2] + q[2] };
							}
							else
							{
								Mask[n].Block = Voxel1;
								DepthOffset = 1;
								BlockPosition = Vector3i{ x[0], x[1], x[2] };
							}

							// Get status of surrounding blocks for ambient occlusion
							//  ---------
							//  | 0 1 2 | // Surrounding blocks a closer than C
							//  | 3 C 4 | // C = Current
							//  | 5 6 7 |
							//  ---------
							bool Sides[8] = { false, false, false, false, false, false, false, false };

							CheckAOSides(Sides, u, v, d, BlockPosition, DepthOffset);
	
							Mask[n].AOFactors = ComputeBlockFaceAO(Sides);

							n++;
						}
					}
				}

				x[d]++;

				// Generate the mesh for the mask
				n = 0;

				for (j = 0; j < (int32_t)CHUNK_SIZE; j++)
				{
					for (i = 0; i < (int32_t)CHUNK_SIZE;)
					{
						if (Mask[n].Block.Type != FBlock::None)
						{
							// Compute the width
							for (Width = 1; i + Width < (int32_t)CHUNK_SIZE && IsMeshable(Mask[n + Width], Mask[n]); Width++){}

							// Compute Height
							bool Done = false;

							for (Height = 1; j + Height < (int32_t)CHUNK_SIZE; Height++)
							{
								for (k = 0; k < Width; k++)
								{
									if (!IsMeshable(Mask[n + k + Height*(int32_t)CHUNK_SIZE], Mask[n]))
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

							mIsEmpty = false;
							AddQuad(Vector3f((float)x[0], (float)x[1], (float)x[2]),
								Vector3f((float)x[0] + (float)du[0], (float)x[1] + (float)du[1], (float)x[2] + (float)du[2]),
								Vector3f((float)x[0] + (float)du[0] + (float)dv[0], (float)x[1] + (float)du[1] + (float)dv[1], (float)x[2] + (float)du[2] + (float)dv[2]),
								Vector3f((float)x[0] + (float)dv[0], (float)x[1] + (float)dv[1], (float)x[2] + (float)dv[2]),
								BackFace,
								Side,
								Mask[n],
								Vertices,
								Indices);


							// Zero the mask
							for (Length = 0; Length < Height; Length++)
							{
								for (k = 0; k < Width; k++)
								{
									Mask[n + k + Length * (int32_t)CHUNK_SIZE].Block.Type = FBlock::None;
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

void FChunk::CheckAOSides(bool Sides[8], int32_t u, int32_t v, int32_t d, Vector3i BlockPosition, int32_t DepthOffset) const
{
	bool HasTop = BlockPosition[v] < (CHUNK_SIZE - 1);
	bool HasBottom = BlockPosition[v] > 0;
	bool HasLeft = BlockPosition[u] > 0;
	bool HasRight = BlockPosition[u] < (CHUNK_SIZE - 1);
	bool HasFront = ((BlockPosition[d] + DepthOffset) <= (CHUNK_SIZE - 1)) && ((BlockPosition[d] + DepthOffset) >= 0);

	//   4       3       2        1      0
	// Front    Left   Right   Bottom   Top
	uint8_t SideMask = (HasFront << 4) + (HasLeft << 3) + (HasRight << 2) + (HasBottom << 1) + HasTop;

	Vector3i Temp = BlockPosition;
	Temp[d] += DepthOffset;

	// If surrounding blocks are all in this chunk
	if (SideMask == 0x1F)
	{
		Temp[u] -= 1; Temp[v] += 1;
		Sides[0] = mBlocks[BlockIndex(Temp)].Type != FBlock::None;

		Temp[u] += 1;
		Sides[1] = mBlocks[BlockIndex(Temp)].Type != FBlock::None;

		Temp[u] += 1;
		Sides[2] = mBlocks[BlockIndex(Temp)].Type != FBlock::None;

		Temp[v] -= 1;
		Sides[4] = mBlocks[BlockIndex(Temp)].Type != FBlock::None;

		Temp[u] -= 2;
		Sides[3] = mBlocks[BlockIndex(Temp)].Type != FBlock::None;

		Temp[v] -= 1;
		Sides[5] = mBlocks[BlockIndex(Temp)].Type != FBlock::None;

		Temp[u] += 1;
		Sides[6] = mBlocks[BlockIndex(Temp)].Type != FBlock::None;

		Temp[u] += 1;
		Sides[7] = mBlocks[BlockIndex(Temp)].Type != FBlock::None;
	}
}

bool FChunk::IsMeshable(const MaskInfo& Lhs, const MaskInfo& Rhs) const
{
	if (Lhs.Block != Rhs.Block || Lhs.Block.Type == FBlock::None || Rhs.Block.Type == FBlock::None)
		return false;

	int32_t BaseAO = Lhs.AOFactors.x;
	for (size_t i = 0; i < 4; i++)
	{
		if (Lhs.AOFactors[i] != BaseAO || Rhs.AOFactors[i] != BaseAO)
			return false;
	}

	return true;
}

Vector4i FChunk::ComputeBlockFaceAO(bool Sides[8]) const
{
	//  ---------
	//  | 0 1 2 | // Surrounding blocks a closer than C
	//  | 3 C 4 | // C = Current
	//  | 5 6 7 |
	//  ---------

	Vector4i FaceAO{0,0,0,0};
	FaceAO.x = ComputeBlockVertexAO(Sides[3], Sides[1], Sides[0]); // Top left vertex
	FaceAO.y = ComputeBlockVertexAO(Sides[3], Sides[6], Sides[5]); // Bottom left vertex
	FaceAO.z = ComputeBlockVertexAO(Sides[4], Sides[6], Sides[7]); // Bottom right vertex
	FaceAO.w = ComputeBlockVertexAO(Sides[1], Sides[4], Sides[2]); // Top right vertex
	return FaceAO;
}

int32_t FChunk::ComputeBlockVertexAO(bool Side1, bool Side2, bool Corner) const
{
	if (Side1 && Side2)
		return 0;

	return 3 - (Side1 + Side2 + Corner);
}

void FChunk::AddQuad(	const Vector3f BottomLeft,
						const Vector3f BottomRight,
						const Vector3f TopRight,
						const Vector3f TopLeft,
						bool IsBackface,
						uint32_t Side,
						const MaskInfo& FaceInfo,
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
	static const float AOFactors[4] = { 0.4f, 0.65f, 0.85f, 1.0f };
	const Vector4f ComputedAOFactors{ AOFactors[FaceInfo.AOFactors.x], AOFactors[FaceInfo.AOFactors.y], AOFactors[FaceInfo.AOFactors.z], AOFactors[FaceInfo.AOFactors.w] };
	uint32_t BaseIndex = VerticesOut.size();

	// We need to rearrange the order that the quad is constructed based on AO factors for each vertex. This prevents anisotropy.
	if (ComputedAOFactors.w + ComputedAOFactors.y > ComputedAOFactors.x + ComputedAOFactors.z)
	{
		VerticesOut.insert(VerticesOut.end(), { { Vector4f{ BottomLeft, ComputedAOFactors.y }, Normal, FBlock::Colors[FaceInfo.Block.Type] },
												{ Vector4f{ BottomRight, ComputedAOFactors.z }, Normal, FBlock::Colors[FaceInfo.Block.Type] },
												{ Vector4f{ TopRight, ComputedAOFactors.w }, Normal, FBlock::Colors[FaceInfo.Block.Type] },
												{ Vector4f{ TopLeft, ComputedAOFactors.x }, Normal, FBlock::Colors[FaceInfo.Block.Type] } });
	}
	else
	{
		VerticesOut.insert(VerticesOut.end(), { { Vector4f{ TopLeft, ComputedAOFactors.x }, Normal, FBlock::Colors[FaceInfo.Block.Type] },
												{ Vector4f{ BottomLeft, ComputedAOFactors.y }, Normal, FBlock::Colors[FaceInfo.Block.Type] },
												{ Vector4f{ BottomRight, ComputedAOFactors.z }, Normal, FBlock::Colors[FaceInfo.Block.Type] },
												{ Vector4f{ TopRight, ComputedAOFactors.w }, Normal, FBlock::Colors[FaceInfo.Block.Type] } });
	}

	// Add adjusted indices.
	if (!IsBackface)
	{
		IndicesOut.insert(IndicesOut.end(), {	0 + BaseIndex, 
												1 + BaseIndex, 
												2 + BaseIndex, 
												2 + BaseIndex, 
												3 + BaseIndex, 
												0 + BaseIndex });

	}
	else
	{
		IndicesOut.insert(IndicesOut.end(), {	0 + BaseIndex, 
												3 + BaseIndex, 
												2 + BaseIndex, 
												0 + BaseIndex, 
												2 + BaseIndex, 
												1 + BaseIndex });
	}

}