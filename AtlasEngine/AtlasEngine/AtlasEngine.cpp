// AtlasEngine.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <xmmintrin.h>
#include "Include\Math\FMath.h"
#include <string>

#include <ctime>
#include <memory>
#include "Include\Math\SSEMath.h"
#include "Include\Math\Random.h"

#include "Include\Singleton.h"
#include "Include\Memory\PoolAllocator.h"
#include <vector>
#include "Include\Memory\StackAllocator.h"
#include "Include\Misc\StringUtil.h"

int main()
{
	clock_t t1, t2;
	t1 = clock();

	FMatrix4 Mat1;
	Mat1.Rotate(Vector3f(5, 100, 10));

	FMatrix4 Mat2;
	Mat2.SetOrigin(Vector3f(5, 0, 1));
	Mat2.Rotate(Vector3f(50, 5, 10));
	
	Vector4f Vec(1, 2, 3, 4);

	//uint8_t Char = 0x6F;

	FMatrix4 Res1;

	//Res1 = Mat1 * Mat2;

	//std::cout << FString::HashCRC32("This") << std::endl;
	//std::cout << FString::HashCRC32("Tis") << std::endl;
	//std::cout << FString::HashCRC32("GoAndSees") << std::endl;
	//std::cout << FString::HashCRC32("Beast") << std::endl;
	Vector4f Res;
	for (int i = 0; i < 1000000; i++)
	{
		//Res = Mat1.TransformVector(Vec);
		//SSEMath::MultVectorMatrix(Mat1, Vec, Res);
		//Mat1.Rotate(Vector3f(i, 0, 0));
		//Res1 = Mat2 * Mat1;
		SSEMath::MultMatrixMatrix(Mat2, Mat1, Res1);
		Mat2.Rotate(Vector3f(i, 0, 0));
	}

	std::cout << Res[0] << std::endl;
	t2 = clock();
	std::cout << ((float)t2 - (float)t1) / CLOCKS_PER_SEC << std::endl;


	return 0;
}

