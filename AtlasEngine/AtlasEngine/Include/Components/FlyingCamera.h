#pragma once
#include "Atlas\Behavior.h"

class FCamera;

class CFlyingCamera : public Atlas::FBehavior
{
public:
	CFlyingCamera();
	~CFlyingCamera();

	void OnStart() override;
	void Update() override;

	void SpawnBox();

private:
	FCamera* mCamera;
};

