#pragma once
#include "Atlas\Behavior.h"

class CTimeBombShooter : public Atlas::FBehavior
{
public:
	CTimeBombShooter();
	~CTimeBombShooter();

	void OnStart() override;
	void Update() override;

private:
	void ShootBomb();
};

