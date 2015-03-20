#include "..\..\Include\Rendering\WorldGenerator.h"
#include "noise\noise.h"
#include "Rendering\Chunk.h"
#include "FileIO\RegionFile.h"
#include "Math\FMath.h"
#include "SystemResources\SystemFile.h"
#include <algorithm>

FWorldGenerator::FWorldGenerator()
	: mTerrainLevels()
	, mLowerBounds(0, 0)
	, mUpperBounds(1, 1)
	, mWorldSizeInChunks(2)
	, mMaxHeight(1)
	, mMinHeight(0)
{

}

FWorldGenerator::~FWorldGenerator()
{

}

void FWorldGenerator::AddTerrainLevel(const int32_t StartingHeight, const FBlock::BlockType BlockType)
{
	mTerrainLevels.push_back(TerrainLevelRecord{ StartingHeight, BlockType });
}

void FWorldGenerator::SetWorldSizeInChunks(const int32_t NewWorldSize)
{
	ASSERT(((NewWorldSize >> 1) & NewWorldSize) == 0x0 && "World size must be a power of 2");
	mWorldSizeInChunks = NewWorldSize;
}

void FWorldGenerator::SetBounds(const Vector2f LowerBounds, const Vector2f UpperBounds)
{
	mLowerBounds = LowerBounds;
	mUpperBounds = UpperBounds;
}

void FWorldGenerator::Build(noise::module::Module& NoiseModule, const wchar_t* WorldName)
{
	utils::NoiseMap HeightMap;
	BuildHeightMap(NoiseModule, HeightMap);
	BuildWorldInfoFile(WorldName);

	// Sort terrain levels in reverse order to be used in world generation.
	std::sort(mTerrainLevels.begin(), mTerrainLevels.end(), [](const TerrainLevelRecord& Lhs, const TerrainLevelRecord& Rhs)
	{
		return Lhs.StartingHeight > Rhs.StartingHeight;
	});

	const int32_t NumRegions = (mWorldSizeInChunks / FRegionFile::RegionData::REGION_SIZE) + 1;

	for (int32_t y = 0; y < NumRegions; y++)
	{
		for (int32_t x = 0; x < NumRegions; x++)
		{
			for (int32_t z = 0; z < NumRegions; z++)
			{
				BuildRegion(WorldName, Vector3i{ x, y, z }, HeightMap);
			}
		}
	}

}

void FWorldGenerator::BuildHeightMap(const noise::module::Module& NoiseModule, utils::NoiseMap& HeightMapOut)
{
	utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule(NoiseModule);
	heightMapBuilder.SetDestNoiseMap(HeightMapOut);

	const int32_t WorldSize = mWorldSizeInChunks * FChunk::CHUNK_SIZE;
	heightMapBuilder.SetDestSize(WorldSize, WorldSize);
	heightMapBuilder.SetBounds(mLowerBounds.x, mUpperBounds.x, mLowerBounds.y, mUpperBounds.y);
	heightMapBuilder.Build();
}

void FWorldGenerator::BuildRegion(const wchar_t* WorldName, const Vector3i& RegionPosition, const utils::NoiseMap& HeightMapOut)
{
	const int32_t RegionSize = (int32_t)FRegionFile::RegionData::REGION_SIZE;
	FRegionFile Region;
	Region.Load(WorldName, RegionPosition);

	// Determine the bounds of this region
	Vector3i RegionBounds = Vector3i{ mWorldSizeInChunks, mWorldSizeInChunks, mWorldSizeInChunks } - (RegionPosition * RegionSize);

	for (int32_t i = 0; i < 3; i++)
	{
		if (RegionBounds[i] >= RegionSize)
			RegionBounds[i] = RegionSize;
		else
			RegionBounds[i] = RegionBounds[i] % RegionSize;
	}


	// Build each chunk within the region
	for (int32_t y = 0; y < RegionBounds.y; y++)
	{
		for (int32_t x = 0; x < RegionBounds.x; x++)
		{
			for (int32_t z = 0; z < RegionBounds.z; z++)
			{
				std::vector<uint8_t> ChunkDataBuffer;
				const Vector3i LocalChunkPosition{ x, y, z };
				const Vector3i WorldChunkPosition = (LocalChunkPosition + (RegionPosition * RegionSize)) * FChunk::CHUNK_SIZE;

				uint32_t DataSize = BuildChunk(WorldChunkPosition, HeightMapOut, ChunkDataBuffer);
				Region.WriteChunkData(LocalChunkPosition, ChunkDataBuffer.data(), DataSize);
			}
		}
	}
}

uint32_t FWorldGenerator::BuildChunk(const Vector3i& WorldPosition, const utils::NoiseMap& HeightMap, std::vector<uint8_t>& DataOut)
{
	uint32_t DataSize = 0;
	const float MinHeight = (float)mMinHeight;
	const float MaxHeight = (float)mMaxHeight;

	for (int32_t y = 0; y < FChunk::CHUNK_SIZE; y++)
	{
		const int32_t WorldY = y + WorldPosition.y;

		// Find the terrain at this level
		auto TerrainLevel = std::find_if(mTerrainLevels.begin(), mTerrainLevels.end(), [WorldY](const TerrainLevelRecord& Val)
		{
			return Val.StartingHeight <= WorldY;
		});

		const FBlock::BlockType BlockType = (TerrainLevel != mTerrainLevels.end()) ? TerrainLevel->BlockType : FBlock::Dirt;

		for (int32_t x = 0; x < FChunk::CHUNK_SIZE; x++)
		{
			const float* SlabValues = HeightMap.GetConstSlabPtr(WorldPosition.x + x);
			for (int32_t z = 0; z < FChunk::CHUNK_SIZE;)
			{
				const int32_t WorldZ = z + WorldPosition.z;

				float xzHeight = FMath::MapValue(SlabValues[WorldZ] + 1, -1.0f, 1.0f, MinHeight, MaxHeight);
				bool IsAir = xzHeight < WorldY;

				uint8_t Length = 1;
				xzHeight = FMath::MapValue(SlabValues[WorldZ + Length] + 1, -1.0f, 1.0f, MinHeight, MaxHeight);
				while ((IsAir == (xzHeight < WorldY)) && (z + (int32_t)Length < FChunk::CHUNK_SIZE))
				{
					Length++;
					xzHeight = FMath::MapValue(SlabValues[WorldZ + Length] + 1, -1.0f, 1.0f, MinHeight, MaxHeight);
				}

				DataOut.push_back(IsAir ? FBlock::None : BlockType);
				DataOut.push_back(Length);
				DataSize += 2;
				z += Length;
			}
		}
	}

	return DataSize;
}

void FWorldGenerator::BuildWorldInfoFile(const wchar_t* WorldName) const
{
	// Create the world directory
	IFileSystem& FileSystem = IFileSystem::GetInstance();

	std::wstring Filepath{ L"./Worlds/" };
	Filepath += WorldName;

	FileSystem.CreateFileDirectory(WorldName);
	Filepath += L"/WorldInfo.vgw";

	// Create the world info file
	auto InfoFile = FileSystem.OpenWritable(Filepath.c_str(), false, true);
	InfoFile->Write((uint8_t*)&mWorldSizeInChunks, 4);
}