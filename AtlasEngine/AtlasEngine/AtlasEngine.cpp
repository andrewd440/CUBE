// AtlasEngine.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "Include\AtlasRoot.h"
#include "Include\Math\Matrix4.h"

int main()
{
	//FAtlasRoot Root;
	//Root.Start();

	FMatrix4 Mat1{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,1,1,1 };
	FMatrix4 Mat2{ 2, 4, 1, 1, 1, 1, 1, 1, 1, 1, 6, 8,2,2,2,2 };
	Mat2 = Mat1 * Mat2;
	return 0;
}

