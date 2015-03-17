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


	//noise::module::Perlin Module;
	//Module.SetOctaveCount(1);
	//Module.SetPersistence(.1f);

	//FWorldGenerator Generator;
	//Generator.SetBounds(Vector2f{ 1.0f, 1.0f }, Vector2f{ 10.0f, 10.0f });
	//Generator.SetWorldSizeInChunks(64);
	//Generator.SetMaxHeight(200);
	//Generator.SetMinHeight(188);

	//Generator.AddTerrainLevel(0, FBlock::Brick);
	//Generator.AddTerrainLevel(150, FBlock::Dirt);
	//Generator.AddTerrainLevel(192, FBlock::Grass);
	//Generator.AddTerrainLevel(201, FBlock::Snow);

	//Generator.Build(Module, L"LargeWorld");

	//delete FileSys;

	return 0;
}

