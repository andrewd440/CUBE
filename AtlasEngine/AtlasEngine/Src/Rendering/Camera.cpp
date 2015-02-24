#include "Rendering\Camera.h"
#include "Math\PerspectiveMatrix.h"
#include "Rendering\Screen.h"
#include "Math\FMath.h"

FCamera* FCamera::Main = nullptr;

FCamera::FCamera()
	: mProjection()
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
	// We can also use the view matrix with the projection to take the frustum
	// into world coordinates.
	const FMatrix4 Mat = GetProjection() * Transform.WorldToLocalMatrix();

	FFrustum Frustum;
	GetFrustumCommon(Frustum, Mat);

	return Frustum;
}

FFrustum FCamera::GetViewFrustum()
{
	const FMatrix4 Mat = GetProjection();

	FFrustum Frustum;
	GetFrustumCommon(Frustum, Mat);

	return Frustum;
}

void FCamera::GetFrustumCommon(FFrustum& Frustum, const FMatrix4& Projection) const
{
	// From Mathematics for 3D Game Programming and Computer Graphics p.107
	const FPlane Left = FPlane{ Projection.GetRow(3) + Projection.GetRow(0) }.Normalize();
	const FPlane Right = FPlane{ Projection.GetRow(3) - Projection.GetRow(0) }.Normalize();
	const FPlane Bottom = FPlane{ Projection.GetRow(3) + Projection.GetRow(1) }.Normalize();
	const FPlane Top = FPlane{ Projection.GetRow(3) - Projection.GetRow(1) }.Normalize();
	const FPlane Near = FPlane{ Projection.GetRow(3) + Projection.GetRow(2) }.Normalize();
	const FPlane Far = FPlane{ Projection.GetRow(3) - Projection.GetRow(2) }.Normalize();

	using Plane = FFrustum::PlaneType;
	Frustum.SetPlane(Plane::Near, Near);
	Frustum.SetPlane(Plane::Far, Far);
	Frustum.SetPlane(Plane::Left, Left);
	Frustum.SetPlane(Plane::Right, Right);
	Frustum.SetPlane(Plane::Bottom, Bottom);
	Frustum.SetPlane(Plane::Top, Top);
}

FMatrix4 FCamera::GetProjection()
{
	return mProjection;
}

void FCamera::SetProjection(const FMatrix4& NewProjection)
{
	mProjection = NewProjection;
}