#include "Math\Box.h"
#include "Math\Matrix4.h"

void FBox::TransformAABB(const FMatrix4& Transform)
{
	// From 3D Math Primer for Graphics and Game Development p. 310
	// Start at current center
	Vector3f NewMin, NewMax;
	NewMin = NewMax = Transform.GetOrigin();

	// Test each matrix element and compute new AABB
	if (Transform.M[0][0] > 0.0f)
	{
		NewMin.x += Transform.M[0][0] * Min.x; NewMax.x += Transform.M[0][0] * Max.x;
	}
	else
	{
		NewMin.x += Transform.M[0][0] * Max.x; NewMax.x += Transform.M[0][0] * Min.x;
	}

	if (Transform.M[0][1] > 0.0f)
	{
		NewMin.y += Transform.M[0][1] * Min.x; NewMax.y += Transform.M[0][1] * Max.x;
	}
	else
	{
		NewMin.y += Transform.M[0][1] * Max.x; NewMax.y += Transform.M[0][1] * Min.x;
	}

	if (Transform.M[0][2] > 0.0f)
	{
		NewMin.z += Transform.M[0][2] * Min.x; NewMax.z += Transform.M[0][2] * Max.x;
	}
	else
	{
		NewMin.z += Transform.M[0][2] * Max.x; NewMax.z += Transform.M[0][2] * Min.x;
	}

	if (Transform.M[1][0] > 0.0f)
	{
		NewMin.x += Transform.M[1][0] * Min.y; NewMax.x += Transform.M[1][0] * Max.y;
	}
	else
	{
		NewMin.x += Transform.M[1][0] * Max.y; NewMax.x += Transform.M[1][0] * Min.y;
	}

	if (Transform.M[1][1] > 0.0f)
	{
		NewMin.y += Transform.M[1][1] * Min.y; NewMax.y += Transform.M[1][1] * Max.y;
	}
	else
	{
		NewMin.y += Transform.M[1][1] * Max.y; NewMax.y += Transform.M[1][1] * Min.y;
	}

	if (Transform.M[1][2] > 0.0f)
	{
		NewMin.z += Transform.M[1][2] * Min.y; NewMax.z += Transform.M[1][2] * Max.y;
	}
	else
	{
		NewMin.z += Transform.M[1][2] * Max.y; NewMax.z += Transform.M[1][2] * Min.y;
	}

	if (Transform.M[2][0] > 0.0f)
	{
		NewMin.x += Transform.M[2][0] * Min.z; NewMax.x += Transform.M[2][0] * Max.z;
	}
	else
	{
		NewMin.x += Transform.M[2][0] * Max.z; NewMax.x += Transform.M[2][0] * Min.z;
	}

	if (Transform.M[2][1] > 0.0f)
	{
		NewMin.y += Transform.M[2][1] * Min.z; NewMax.y += Transform.M[2][1] * Max.z;
	}
	else
	{
		NewMin.y += Transform.M[2][1] * Max.z; NewMax.y += Transform.M[2][1] * Min.z;
	}

	if (Transform.M[2][2] > 0.0f)
	{
		NewMin.z += Transform.M[2][2] * Min.z; NewMax.z += Transform.M[2][2] * Max.z;
	}
	else
	{
		NewMin.z += Transform.M[2][2] * Max.z; NewMax.z += Transform.M[2][2] * Min.z;
	}

	Max = NewMax;
	Min = NewMin;
}