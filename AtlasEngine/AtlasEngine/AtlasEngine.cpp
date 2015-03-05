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
	FVoxiGineRoot Root;
	Root.Start();
	
	//IFileSystem* FileSys = new FFileSystem;
	//FileSys->SetToProgramDirectory();


	//noise::module::Perlin Module;
	//Module.SetOctaveCount(1);
	//Module.SetPersistence(.1f);

	//FWorldGenerator Generator;
	//Generator.SetBounds(Vector2f{ 1.0f, 1.0f }, Vector2f{ 10.0f, 10.0f });
	//Generator.SetWorldSizeInChunks(8);
	//Generator.SetMaxHeight(128);
	//Generator.SetMinHeight(110);

	//Generator.AddTerrainLevel(0, FBlock::Brick);
	//Generator.AddTerrainLevel(70, FBlock::Dirt);
	//Generator.AddTerrainLevel(119, FBlock::Grass);
	//Generator.AddTerrainLevel(130, FBlock::Snow);

	//Generator.Build(Module, L"GenWorld");

	//delete FileSys;

	return 0;
}

