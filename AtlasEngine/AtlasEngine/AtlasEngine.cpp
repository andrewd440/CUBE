// AtlasEngine.cpp : Defines the entry point for the console application.
//

#include "VoxiGineRoot.h"

//#include "FileIO\RegionFile.h"
//#include <vector>

int main()
{
	FVoxiGineRoot Root;
	Root.Start();
	
	//IFileSystem* FileSys = new FFileSystem;
	//FileSys->SetToProgramDirectory();

	//FRegionFile Region;
	//Region.Load(L"TestWorld", Vector3i{ 0, 0, 0 });

	//std::vector<uint8_t> RegionData;
	//for (int32_t y = 0; y < 32; y++)
	//{
	//	for (int32_t x = 0; x < 32; x++)
	//	{

	//		RegionData.insert(RegionData.end(), { (y > 10) ? (uint8_t)5 : (uint8_t)0, 32 });
	//	}
	//}

	//Region.WriteChunkData(Vector3i{ 0, 0, 0 }, RegionData.data(), RegionData.size());
	//Region.WriteChunkData(Vector3i{ 0, 0, 1 }, RegionData.data(), RegionData.size());
	//Region.WriteChunkData(Vector3i{ 0, 1, 0 }, RegionData.data(), RegionData.size());
	//Region.WriteChunkData(Vector3i{ 0, 1, 1 }, RegionData.data(), RegionData.size());
	//Region.WriteChunkData(Vector3i{ 1, 0, 0 }, RegionData.data(), RegionData.size());
	//Region.WriteChunkData(Vector3i{ 1, 0, 1 }, RegionData.data(), RegionData.size());
	//Region.WriteChunkData(Vector3i{ 1, 1, 0 }, RegionData.data(), RegionData.size());
	//Region.WriteChunkData(Vector3i{ 1, 1, 1 }, RegionData.data(), RegionData.size());

	return 0;
}

