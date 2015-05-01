#pragma once
#include "Atlas\Behavior.h"

#include <bitset>

class CBlockPlacer : public Atlas::FBehavior
{
public:
	CBlockPlacer();
	~CBlockPlacer();

	void Update() override;

	void ChangeType();

private:
	uint32_t mActiveType;
	float    mPlacerRange;
	bool     mShowBox;
};

