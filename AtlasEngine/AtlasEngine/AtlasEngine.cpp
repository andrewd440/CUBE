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

class Single : public TSingleton < Single >
{
public:
	Single()
		: TSingleton<Single>()
	{
	
	}

};

//Single* Single::Instance = nullptr;

int main()
{

	clock_t t1, t2;
	t1 = clock();

	FPoolAllocator<16, 100> VecPool;
	int* thisthat = new int;
	//FPoolAllocatorType<Vector4f, 100> VecPool;
	Vector4f* Vecs[100];
	for (int i = 0; i < 100; i++)
	{
		std::cout << VecPool.FreeSlotsLeft() << std::endl;
		Vecs[i] = (Vector4f*)VecPool.Allocate();
		*Vecs[i] = Vector4f(i, i, i);
	}

	FMatrix4* Mat = (FMatrix4*) FMemory::AllocateAligned(sizeof(FMatrix4), 16);
	FMemory::FreeAligned(Mat);
	VecPool.Free(Vecs[20]);
	VecPool.Free(Vecs[21]);

	for (int i = 0; i < 100; i++)
	{
		if (i != 20 && i != 21)
		{
			std::cout << Vecs[i]->ToString() << std::endl;
			VecPool.Free(Vecs[i]);
		}
	}

	delete thisthat;

	t2 = clock();
	std::cout << ((float)t2 - (float)t1) / CLOCKS_PER_SEC << std::endl;


	return 0;
}

