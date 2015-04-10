#pragma once

#include "LibNoise\module\module.h"
#include "Math\Vector2.h"
#include "Math\Vector3.h"
#include "LibNoise\noiseutils.h"
#include "Rendering\Block.h"

#include <vector>

/**
* Used for generating world layouts from noise functions.
*/
class FWorldGenerator
{
public:
	/** Ctor */
	FWorldGenerator();

	/** Dtor */
	~FWorldGenerator();

	/**
	* Adds a terrain type at a specific staring height within the generated world.
	* This block type will be used for the bounds between it's starting height and the
	* next terrain level, if added.
	*/
	void AddTerrainLevel(const int32_t StartingHeight, const FBlock::Type Type);

	/**
	* Sets the minimum height, in world coordinates, that the world terrain will
	* reach.
	*/
	void SetMinHeight(const int32_t MinHeight) { mMinHeight = MinHeight; }

	/**
	* Sets the maximum height, in world coordinates, that the world terrain will
	* reach.
	*/
	void SetMaxHeight(const int32_t MaxHeight) { mMaxHeight = MaxHeight; }

	/**
	* Sets the cubic size of the world in chunks.
	*/
	void SetWorldSizeInChunks(const int32_t NewWorldSize);

	/**
	* Sets the bounds used in the noise function to generate
	* the world layout.
	*/
	void SetBounds(const Vector2f LowerBounds, const Vector2f UpperBounds);

	/**
	* Builds the world region files.
	* @param NoiseModule - The noise module used to build to world.
	* @param WorldName - The name of the world to build.
	*/
	void Build(noise::module::Module& NoiseModule, const wchar_t* WorldName);

private:
	/**
	* Builds a heightmap with the currently stored world info.
	* @param NoiseModule - The module to built the map with.
	* @param HeightMapOut - The map to use to build.
	*/
	void BuildHeightMap(const noise::module::Module& NoiseModule, utils::NoiseMap& HeightMapOut);

	/**
	* Builds a region file for a world from a given heightmap.
	* @param WorldName - The name of the world for this region.
	* @param RegionPosition - The position of the region within this world.
	* @param HeightMap - The heightmap to use to build this region.
	*/
	void BuildRegion(const wchar_t* WorldName, const Vector3i& RegionPosition, const utils::NoiseMap& HeightMap);

	/**
	* Builds RLE chunk data based of a heightmap.
	* @param WorldPosition - The world position of this chunk.
	* @param HeightMap - The heightmap to use to build this chunk.
	* @param DataOut - RLE data for this chunk will be placed here.
	* @return The size of the data added to DataOut.
	*/
	uint32_t BuildChunk(const Vector3i& WorldPosition, const utils::NoiseMap& HeightMap, std::vector<uint8_t>& DataOut);

	void BuildWorldInfoFile(const wchar_t* WorldName) const;


private:
	struct TerrainLevelRecord
	{
		int32_t StartingHeight;
		FBlock::Type BlockType;
	};

private:
	std::vector<TerrainLevelRecord> mTerrainLevels;
	Vector2f mLowerBounds;
	Vector2f mUpperBounds;
	int32_t mWorldSizeInChunks;
	int32_t mMaxHeight;
	int32_t mMinHeight;
};

