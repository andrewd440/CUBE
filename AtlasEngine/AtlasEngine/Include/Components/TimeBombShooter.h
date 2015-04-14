#pragma once
#include "Atlas\Behavior.h"

class CTimeBombShooter : public Atlas::FBehavior
{
public:
	CTimeBombShooter();
	~CTimeBombShooter();

	void OnStart() override;
	void Update() override;

	void OnRedSet(Vector3i Position, FBlockTypes::BlockID ID);

private:
	void ShootBomb();
};

