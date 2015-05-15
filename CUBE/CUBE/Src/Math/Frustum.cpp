#include "Math\Frustum.h"
#include "Math\Vector4.h"
#include "Math\Sphere.h"
#include "Math\SystemMath.h"

bool FFrustum::IsUniformAABBVisible(const Vector4f& CenterPoint, const float BoxWidth) const
{

	// From Mathematics for 3D Game Programming and Computer Graphics
	const Vector4f BoxRight = Vector4f{ BoxWidth, 0, 0, 0.0f };
	const Vector4f BoxUp = Vector4f{ 0, BoxWidth, 0, 0.0f };
	const Vector4f BoxForward = Vector4f{ 0, 0, -BoxWidth, 0.0f };

	for (auto& Plane : mPlanes)
	{
		// Compute effective radius of the box against the plane
		const float EffectiveRadius = 0.5f *   (abs(Vector4f::Dot4(Plane.NormalwDistance, BoxRight)) +
												abs(Vector4f::Dot4(Plane.NormalwDistance, BoxUp)) +
												abs(Vector4f::Dot4(Plane.NormalwDistance, BoxForward)));

		const float CenterDotNormal = Vector4f::Dot4(CenterPoint, Plane.NormalwDistance);
		if (CenterDotNormal <= -EffectiveRadius)
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
		Dot4Product(&Plane.NormalwDistance.x, &SphereVector.x, &Dot);
		if (Dot <= -Sphere.Radius)
			return false;
	}

	return true;
}

FFrustum::Intersection FFrustum::TestAABB(const Vector4f& CenterPoint, const float BoxWidth) const
{

}

void FFrustum::TransformBy(const FMatrix4& Transform)
{
	for (int32_t i = 0; i < 6; i++)
	{
		mPlanes[i].TransformBy(Transform);
	}
}