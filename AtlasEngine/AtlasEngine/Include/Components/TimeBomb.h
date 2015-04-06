#pragma once
#include "Atlas\Behavior.h"

class CTimeBomb : public Atlas::FBehavior
{
public:
	CTimeBomb();
	~CTimeBomb();

	void Update() override;

private:
	float mLifetime;
	float mTimer;
	uint32_t mRadius;
	bool mHasExploded;
};

