#include "Rendering\Camera.h"
#include "Math\PerspectiveMatrix.h"
#include "Rendering\Screen.h"
#include "Math\FMath.h"

FCamera* FCamera::Main = nullptr;

FCamera::FCamera()
	: mProjection()
	, mNearPlane(0)
	, mFarPlane(10)
	, mFieldOfView(85)
	, mAspectRatio(1)
	, mIsProjectionDirty(true)
{
	// If this is the first camera, assign it to main
	if (!Main)
		Main = this;
}


FCamera::~FCamera()
{
	// If this was the main camera, remove it
	if (Main == this)
		Main = nullptr;
}

FFrustum FCamera::GetWorldViewFrustum() const
{
	// From Mathematics for 3D Game Programming and Computer Graphics p.107
	// Cache common values for finding the frustum planes.
	const float FocalDistance = 1.0f / tan(FMath::ToRadians(mFieldOfView) / 2.0f);
	const float FocalSquared = FocalDistance * FocalDistance;
	const float Near = mNearPlane;
	const float Far = mFarPlane;
	const float AspectRatio = mAspectRatio;
	const float AspectSquared = AspectRatio * AspectRatio;

	const float LeftXNormal = FocalDistance / sqrt(FocalSquared + 1.0f);
	const float LeftZNormal = -1.0f / sqrt(FocalSquared + 1.0f);
	const float BottomYNormal = FocalDistance / sqrt(FocalSquared + AspectSquared);
	const float BottomZNormal = -AspectRatio / sqrt(FocalSquared + AspectSquared);

	using Plane = FFrustum::PlaneType;
	FFrustum Frustum;
	Frustum.SetPlane(Plane::Near, FPlane{ Vector3f{ 0, 0, -1 }, -Near });
	Frustum.SetPlane(Plane::Far, FPlane{ Vector3f{ 0, 0, 1 }, Far });
	Frustum.SetPlane(Plane::Left, FPlane{ Vector3f{ LeftXNormal, 0, LeftZNormal }, 0 });
	Frustum.SetPlane(Plane::Right, FPlane{ Vector3f{ -LeftXNormal, 0, LeftZNormal }, 0 });
	Frustum.SetPlane(Plane::Bottom, FPlane{ Vector3f{ 0, BottomYNormal, BottomZNormal }, 0 });
	Frustum.SetPlane(Plane::Top, FPlane{ Vector3f{ 0, -BottomYNormal, BottomZNormal }, 0 });

	// Transform all planes to world space.
	Frustum.TransformBy(Transform.LocalToWorldMatrix());
	return Frustum;
}

FMatrix4 FCamera::GetProjection()
{
	if (mIsProjectionDirty)
	{
		mProjection = FPerspectiveMatrix{ mAspectRatio, mFieldOfView / 2.0f, mNearPlane, mFarPlane };
		mIsProjectionDirty = false;
	}
	
	return mProjection;
}
