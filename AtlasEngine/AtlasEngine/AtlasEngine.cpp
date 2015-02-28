// AtlasEngine.cpp : Defines the entry point for the console application.
//

#include "VoxiGineRoot.h"
#include "SystemResources\SystemFile.h"
#include "FileIO\RegionFile.h"
#include <iostream>

int main()
{
	//FVoxiGineRoot Root;
	//Root.Start();
	
	//FRegionFile Region;
	//Region.Load();
	static uint8_t Padding[1000];

	std::wstring Name;
	Name.reserve(1000);
	IFileSystem* FileSys = new FFileSystem;

	FRegionFile Region;
	Region.Load(L"TestWorld", Vector3ui{ 1, 1, 1 });
	Region.WriteChunkData(Vector3ui{ 0, 0, 0 }, (uint8_t*)"This is the test data for this chunk in the region", 50);
	Region.WriteChunkData(Vector3ui{ 0, 0, 1 }, (uint8_t*)"This is the test data for this chunk in the region", 50);

	uint8_t RelData[5000];
	for (int32_t i = 0; i < 5000; i++)
	{
		RelData[i] = 1;
	}
	Region.WriteChunkData(Vector3ui{ 0, 0, 0 }, RelData, 5000);

	int8_t Data[50];
	uint32_t Sector, Size;
	Region.GetChunkDataInfo(Vector3ui{ 0, 0, 0 }, Size, Sector);
	Region.GetChunkData(Sector, (uint8_t*)RelData, Size);

	Region.GetChunkDataInfo(Vector3ui{ 0, 0, 1 }, Size, Sector);
	Region.GetChunkData(Sector, (uint8_t*)Data, Size);

	delete FileSys;
	return 0;
}

