// AtlasEngine.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <xmmintrin.h>
#include "Include\Math\FMath.h"
#include <string>

#include <ctime>

#include "Include\Math\SSEMath.h"
#include "Include\Math\Random.h"

int main()
{

	clock_t t1, t2;
	t1 = clock();

	FRandom::Seed(123);
	uint32_t Rand = FRandom::GenRand();

	t2 = clock();
	std::cout << ((float)t2 - (float)t1) / CLOCKS_PER_SEC << std::endl;


	return 0;
}

