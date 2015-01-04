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

template <uint32_t N>
struct Fact
{
	static const uint64_t Value = N * Fact<N-1>::Value;
};

template <>
struct Fact < 0 >
{
	static const uint64_t Value = 1;
};

int main()
{
	clock_t t1, t2;
	t1 = clock();

	FMatrix4 Mat1;
	Mat1.Rotate(Vector3f(5, 100, 10));

	FMatrix4 Mat2;
	Mat2.SetOrigin(Vector3f(5, 0, 1));
	Mat2.Rotate(Vector3f(50, 5, 10));

	FMatrix4 Res1;

	//for (int i = 0; i < 1000000; i++)
	//{
	//	//Res1 = Mat2 * Mat1;
	//	SSEMath::MultMatrixMatrix(Mat2, Mat1, Res1);
	//	Mat2.Rotate(Vector3f(i, 0, 0));
	//}
	t2 = clock();
	std::cout << ((float)t2 - (float)t1) / CLOCKS_PER_SEC << std::endl;


	return 0;
}

