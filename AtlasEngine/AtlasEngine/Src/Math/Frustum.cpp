#include "Math\Frustum.h"
#include "Math\Vector4.h"
#include "Math\Sphere.h"
#include "Math\SystemMath.h"

bool FFrustum::IsUniformAABBVisible(const Vector3f& CenterPoint, const float BoxWidth) const
{
	// From Mathematics for 3D Game Programming and Computer Graphics
	const Vector4f BoxCenterVector{ CenterPoint, 1 };
	for (auto& Plane : mPlanes)
	{
		// Compute effective radius of the box against the plane
		const float EffectiveRadius = 0.5f *   (abs(Vector3f::Dot(Plane.Normal, Vector3f::Right * BoxWidth)) + 
												abs(Vector3f::Dot(Plane.Normal, Vector3f::Up * BoxWidth)) + 
												abs(Vector3f::Dot(Plane.Normal, Vector3f::Forward * BoxWidth)));
		const Vector4f PlaneVector{ Plane.Normal, Plane.DistanceFromOrigin };

		if (Vector4f::Dot4(BoxCenterVector, PlaneVector) <= -EffectiveRadius)
			return false;
	}

	return true;
}

bool FFrustum::IsSphereVisible(const FSphere& Sphere) const
{
	Vector4f SphereVector{ Sphere.Center, 1.0f };
	for (const FPlane& Plane : mPlanes)
	{
		float Dot;
		Dot4Product(&Plane.Normal.x, &SphereVector.x, Dot);
		if (Dot <= -Sphere.Radius)
			return false;
	}

	return true;
}

void FFrustum::TransformBy(const FMatrix4& Transform)
{
	for (int32_t i = 0; i < 6; i++)
	{
		mPlanes[i].TransformBy(Transform);
	}
}