// AtlasEngine.cpp : Defines the entry point for the console application.
//
#include <string>
#include <iostream>
#include <cstdint>

#include "Include\Math\Matrix4.h"

__declspec(align(16))
struct Test
{
	float M[4][4];
};

int main()
{
	Vector4f v1(1,1,0,1);
	v1.Normalize3();
	Vector4f v2(2,2,0,1);

	std::cout << v1.ProjectedOnVector(v2).ToString() << std::endl;

	return 0;
}

