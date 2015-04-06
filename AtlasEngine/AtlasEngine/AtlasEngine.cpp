// AtlasEngine.cpp : Defines the entry point for the console application.
//

#include "VoxiGineRoot.h"

//#include "FileIO\RegionFile.h"
//#include <vector>

//#include <iostream>
#include "noise/noise.h"
#include "Rendering\WorldGenerator.h"


int main()
{
	FVoxiGineRoot root;
	root.Start();

	//IFileSystem* FileSys = new FFileSystem;
	//FileSys->SetToProgramDirectory();

	//module::RidgedMulti mountainTerrain;

	//module::Billow baseFlatTerrain;
	//baseFlatTerrain.SetFrequency(2.0);

	//module::ScaleBias flatTerrain;
	//flatTerrain.SetSourceModule(0, baseFlatTerrain);
	//flatTerrain.SetScale(0.125);
	//flatTerrain.SetBias(-0.75);

	//module::Perlin terrainType;
	//terrainType.SetFrequency(0.5);
	//terrainType.SetPersistence(0.25);

	//module::Select terrainSelector;
	//terrainSelector.SetSourceModule(0, flatTerrain);
	//terrainSelector.SetSourceModule(1, mountainTerrain);
	//terrainSelector.SetControlModule(terrainType);
	//terrainSelector.SetBounds(0.0, 1000.0);
	//terrainSelector.SetEdgeFalloff(0.125);

	//module::Turbulence finalTerrain;
	//finalTerrain.SetSourceModule(0, terrainSelector);
	//finalTerrain.SetFrequency(4.0);
	//finalTerrain.SetPower(0.125);

	//FWorldGenerator Generator;
	//Generator.SetBounds(Vector2f{ 1.0f, 1.0f }, Vector2f{ 10.0f, 10.0f });
	//Generator.SetWorldSizeInChunks(32);
	//Generator.SetMaxHeight(280);
	//Generator.SetMinHeight(180);

	//Generator.AddTerrainLevel(0, FBlock::Brick);
	//Generator.AddTerrainLevel(180, FBlock::Grass);
	//Generator.AddTerrainLevel(250, FBlock::Dirt);
	//Generator.AddTerrainLevel(270, FBlock::Snow);

	//Generator.Build(finalTerrain, L"PrettyWorld");

	//delete FileSys;

	return 0;
}

