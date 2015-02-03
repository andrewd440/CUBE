#pragma once

#include "Math\Transform.h"
#include "Math\Matrix4.h"

class FCamera
{
public:
	static FCamera* Main;

public:
	FTransform ViewTransform;

public:
	/**
	* Constructs a camera at world origin.
	*/
	FCamera();

	FCamera(const FCamera& Other) = default;

	~FCamera();

	FCamera& operator=(const FCamera& Other) = default;

	/**
	* Set the view projection used by this camera.
	*/
	void SetProjection(const FMatrix4& ViewProjection)
	{
		mProjection = ViewProjection;
	}

	/**
	* Get the view projection used by this camera.
	*/
	FMatrix4 GetProjection() const
	{
		return mProjection;
	}

private:
	FMatrix4 mProjection;
};

