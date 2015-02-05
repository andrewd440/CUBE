#include "..\..\Include\Math\FMath.h"
#include "Math\Box.h"
#include "Math\Plane.h"

namespace FMath
{
	void UpdateBounds(Vector3f& CurrentMin, Vector3f& CurrentMax, const Vector3f& NewPoint)
	{
		for (int i = 0; i < 3; i++)
		{
			if (CurrentMax[i] < NewPoint[i])
			{
				CurrentMax[i] = NewPoint[i];
			}
			if (CurrentMin[i] > NewPoint[i])
			{
				CurrentMin[i] = NewPoint[i];
			}
		}
	}

	void ComputeBarycentric(const Vector3f& V0,
		const Vector3f& V1,
		const Vector3f& V2,
		const Vector3f& SurfacePoint,
		float BOut[3])
	{
		// compute counter clockwise edge vectors
		const Vector3f& e1 = V1 - V2;
		const Vector3f& e2 = V2 - V0;
		const Vector3f& e3 = V0 - V1;

		const Vector3f& Normal = Vector3f::Cross(e2, e1);

		// compute vectors from point to each vertex
		const Vector3f& d1 = SurfacePoint - V0;
		const Vector3f& d2 = SurfacePoint - V1;
		const Vector3f& d3 = SurfacePoint - V2;

		// compute denominator for each barycentric coordinate calculation
		const float& Denom = 1.0f / Vector3f::Dot(Vector3f::Cross(e2, e1), Normal);

		BOut[0] = Vector3f::Dot(Vector3f::Cross(e1, d3), Normal) * Denom;
		BOut[1] = Vector3f::Dot(Vector3f::Cross(e2, d1), Normal) * Denom;
		BOut[2] = Vector3f::Dot(Vector3f::Cross(e3, d2), Normal) * Denom;
	}

	
	IntersectionType Intersects(const FPlane& Plane, const FBox& Box)
	{
		// From Real-Time Collision Detection
		const Vector3f BoxCenter = Box.GetCenter();
		const Vector3f Extents = Box.Max - BoxCenter;

		// Compute projection interval radius of box
		float Radius = Extents.x * abs(Plane.Normal.x) + Extents.y * abs(Plane.Normal.y) + Extents.z * abs(Plane.Normal.z);
		
		// Compute distance of box center from plane
		float Distance = Vector3f::Dot(Plane.Normal, BoxCenter) - Plane.DistanceFromOrigin;

		if (abs(Distance) <= Radius)
		{
			return IntersectionType::Straddle;
		}
		else if (Distance >= 0)
		{
			return IntersectionType::Front;
		}
		else
		{
			return IntersectionType::Behind;
		}
	}
}