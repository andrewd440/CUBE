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

FFrustum FCamera::GetWorldViewFrustum() 
{
	if (mIsProjectionDirty)
	{
		mProjection = FPerspectiveMatrix{ mAspectRatio, mFieldOfView / 2.0f, mNearPlane, mFarPlane };
		mIsProjectionDirty = false;
	}


	// From Mathematics for 3D Game Programming and Computer Graphics p.107
	// We can also use the view matrix with the projection to take the frustum
	// into world coordinates.
	const FMatrix4 Mat = GetProjection() * Transform.WorldToLocalMatrix();
	const FPlane Left = FPlane{ Mat.GetRow(3) + Mat.GetRow(0) }.Normalize();
	const FPlane Right = FPlane{ Mat.GetRow(3) - Mat.GetRow(0) }.Normalize();
	const FPlane Bottom = FPlane{ Mat.GetRow(3) + Mat.GetRow(1) }.Normalize();
	const FPlane Top = FPlane{ Mat.GetRow(3) - Mat.GetRow(1) }.Normalize();
	const FPlane Near = FPlane{ Mat.GetRow(3) + Mat.GetRow(2) }.Normalize();
	const FPlane Far = FPlane{ Mat.GetRow(3) - Mat.GetRow(2) }.Normalize();

	using Plane = FFrustum::PlaneType;
	FFrustum Frustum;
	Frustum.SetPlane(Plane::Near, Near);
	Frustum.SetPlane(Plane::Far, Far );
	Frustum.SetPlane(Plane::Left, Left );
	Frustum.SetPlane(Plane::Right, Right );
	Frustum.SetPlane(Plane::Bottom, Bottom );
	Frustum.SetPlane(Plane::Top, Top );

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
