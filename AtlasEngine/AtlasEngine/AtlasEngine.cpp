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
#include "Include\StringID.h"
#include "Include\Math\SSEMath.h"
#include "Include\Windows\WindowsFile.h"
#include "Include\Clock.h"
#include "Include\FTime.h"

int main()
{
	Vector4f Vec1(4, 3, 2, 1), Res;
	FMatrix4 Mat;
	Mat.Rotate(Vector3f(2, 3, 4));

	FTime::SetDeltaTime(1.0f / 30.0f);

	for (int i = 0; i < 1000; i++)
	{
		uint64_t T1 = FClock::ReadClockTimer();
		SSEMath::MultVectorMatrix(Mat, Vec1, Res);
		uint64_t T2 = FClock::ReadClockTimer();

		FTime::SetDeltaTime(FClock::CyclesToSeconds(T2 - T1));
	}

	std::cout << Res.x << std::endl;
	//std::cout << T2 - T1 << std::endl;

	return 0;
}

