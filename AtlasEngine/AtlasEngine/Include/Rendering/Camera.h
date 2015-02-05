#pragma once

#include "Math\Transform.h"
#include "Math\Matrix4.h"
#include "Math\Frustum.h"

/**
* The camera that a scene is view by.
*/
class FCamera
{
public:
	// Current rendering camera
	static FCamera* Main;

public:
	FTransform Transform;

public:
	/**
	* Constructs a camera at world origin.
	* If this is the only camera instantiated, it will be assign
	* to FCamera::Main.
	*/
	FCamera();

	FCamera(const FCamera& Other) = default;

	~FCamera();

	FCamera& operator=(const FCamera& Other) = default;

	/**
	* Set the distance to the near plane.
	*/
	void SetNearPlane(const float NearPlaneDistance);

	/**
	* Get the distance to the near plane.
	*/
	float GetNearPlane() const;

	/**
	* Set the distance to the far plane.
	*/
	void SetFarPlane(const float FarPlaneDistance);

	/**
	* Set the distance to the far plane.
	*/
	float GetFarPlane() const;

	/**
	* Set the horizontal field of view of the camera.
	*/
	void SetFieldOfView(const float NewFOV);

	/**
	* Get the horizontal field of view of the camera.
	*/
	float GetFieldOfView() const;

	/**
	* Set the aspect ratio of the camera.
	*/
	void SetAspectRatio(const float NewAspect);

	/**
	* Get the aspect ratio of the camera.
	*/
	float GetAspectRatio() const;

	/**
	* Get the view projection used by this camera.
	*/
	FMatrix4 GetProjection();

	/**
	* Get the view frustum of the camera in world space.
	*/
	FFrustum GetWorldViewFrustum() const;

private:
	FMatrix4 mProjection;
	float mNearPlane;
	float mFarPlane;
	float mFieldOfView;
	float mAspectRatio;
	bool mIsProjectionDirty; // Dirty if the projection matrix has not been updated with new plane, FOV, or aspect values.
};

inline void FCamera::SetNearPlane(const float NearPlaneDistance)
{
	mNearPlane = NearPlaneDistance;
	mIsProjectionDirty = true;
}

inline float FCamera::GetNearPlane() const
{
	return mNearPlane;
}

inline void FCamera::SetFarPlane(const float FarPlaneDistance)
{
	mFarPlane = FarPlaneDistance;
	mIsProjectionDirty = true;
}

inline float FCamera::GetFarPlane() const
{
	return mFarPlane;
}

inline void FCamera::SetFieldOfView(const float NewFOV)
{
	mFieldOfView = NewFOV;
	mIsProjectionDirty = true;
}

inline float FCamera::GetFieldOfView() const
{
	return mFieldOfView;
}

inline void FCamera::SetAspectRatio(const float NewAspect)
{
	mAspectRatio = NewAspect;
}

inline float FCamera::GetAspectRatio() const
{
	return mAspectRatio;
}