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
	* Get the view projection used by this camera.
	*/
	void SetProjection(const FMatrix4& NewProjection);

	/**
	* Get the view projection used by this camera.
	*/
	FMatrix4 GetProjection();

	/**
	* Get the view frustum of the camera in world space.
	*/
	FFrustum GetWorldViewFrustum();

	/**
	* Get the view frustum of the camera in view space.
	*/
	FFrustum GetViewFrustum();

private:
	void GetFrustumCommon(FFrustum& Frustum, const FMatrix4& Projection) const;

private:
	FMatrix4 mProjection;
};