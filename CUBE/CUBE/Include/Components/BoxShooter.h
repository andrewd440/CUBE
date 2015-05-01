#pragma once
#include "Atlas\Behavior.h"

class CBoxShooter : public Atlas::FBehavior
{
public:
	CBoxShooter();
	~CBoxShooter();

	void Update() override;
};

