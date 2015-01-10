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

#include "Include\FileIO\WindowsFile.h"

int main()
{
	clock_t t1, t2;
	t1 = clock();

	t2 = clock();
	std::cout << ((float)t2 - (float)t1) / CLOCKS_PER_SEC << std::endl;

	return 0;
}

