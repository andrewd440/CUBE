#pragma once
#include "..\Common.h"
#include "Matrix4.h"

/**
* An OpenGL perspective matrix.
*/
WIN_ALIGN(16)
class FPerspectiveMatrix : public FMatrix4
{
public:
	/**
	* Ctor
	* Construct a perspective matrix.
	* @param Width of the view 
	* @param Height of the view
	* @param HalfFOV - Half of the camera horizontal FOV
	* @param Near - Distance to the near plane
	* @param Far - Distance to the far plane
	*/
	FPerspectiveMatrix(const float Width, const float Height, const float HalfFOV, const float Near, const float Far);

	/**
	* Ctor
	* Construct a perspective matrix.
	* @param Aspect - The aspect ratio of the view.
	* @param HalfFOV - Half of the camera horizontal FOV
	* @param Near - Distance to the near plane
	* @param Far - Distance to the far plane
	*/
	FPerspectiveMatrix(const float Aspect, const float HalfFOV, const float Near, const float Far);

	~FPerspectiveMatrix() = default;

	/**
	* Copy conversion with standard matrix class.
	*/
	FPerspectiveMatrix& operator=(const FMatrix4& Other);
};

inline FPerspectiveMatrix::FPerspectiveMatrix(const float Width, const float Height, const float HalfFOV, const float Near, const float Far)
	: FPerspectiveMatrix(Width/Height, HalfFOV, Near, Far)
{
}

inline FPerspectiveMatrix::FPerspectiveMatrix(const float Aspect, const float HalfFOV, const float Near, const float Far)
{
	const float TanHalfFOV = tan(FMath::ToRadians(HalfFOV));
	*this = FMatrix4{
		1 / (Aspect * TanHalfFOV), 0, 0, 0,
		0, 1 / TanHalfFOV, 0, 0,
		0, 0, -(Far + Near) / (Far - Near), -(2 * Far * Near) / (Far - Near),
		0, 0, -1, 0
	};
}


inline FPerspectiveMatrix& FPerspectiveMatrix::operator=(const FMatrix4& Other)
{
	for (int col = 0; col < 4; col++)
	{
		for (int row = 0; row < 4; row++)
		{
			M[col][row] = Other.M[col][row];
		}
	}

	return *this;
}