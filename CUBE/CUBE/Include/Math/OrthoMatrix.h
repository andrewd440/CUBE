#pragma once
#include "Matrix4.h"
class FOrthoMatrix : public FMatrix4
{
public:

	/**
	* Ctor
	* Construct a orthographic matrix.
	* @param Width of the view
	* @param Height of the view
	* @param Near - Distance to the near plane
	* @param Far - Distance to the far plane
	*/
	FOrthoMatrix(const float Width, const float Height, const float Near, const float Far);

	FOrthoMatrix(const float Right, const float Left, const float Top, const float Bottom, const float Near, const float Far);

	~FOrthoMatrix() = default;
};

inline FOrthoMatrix::FOrthoMatrix(const float Width, const float Height, const float Near, const float Far)
	: FMatrix4()
{
	M[0][0] = 1.0f / (Width / 2.0f); 
	M[1][1] = 1.0f / (Height / 2.0f);
	M[2][2] = -2.0f / (Far - Near);
	M[3][2] = -(Far + Near) / (Far - Near);
}

inline FOrthoMatrix::FOrthoMatrix(const float Left, const float Right, const float Top, const float Bottom, const float Near, const float Far)
	: FMatrix4()
{
	M[0][0] = 2.0f / (Right - Left);
	M[1][1] = 2.0f / (Top - Bottom);
	M[2][2] = -2.0f / (Far - Near);

	M[3][0] = -(Right + Left) / (Right - Left);
	M[3][1] = -(Top + Bottom) / (Top - Bottom);
	M[3][2] = -(Far + Near) / (Far - Near);
}