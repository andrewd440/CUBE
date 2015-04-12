#include "Components\TimeBomb.h"
#include "STime.h"
#include "Atlas\GameObject.h"


CTimeBomb::CTimeBomb()
	: FBehavior()
	, mLifetime(10.0f)
	, mTimer(0.0f)
	, mRadius(7)
	, mHasExploded(false)
{
}


CTimeBomb::~CTimeBomb()
{
}


void CTimeBomb::Update()
{
	mTimer += STime::GetDeltaTime();

	if (!mHasExploded && mTimer >= mLifetime)
	{
		mHasExploded = true;
		const Vector3i MinPoint = GetGameObject()->Transform.GetWorldPosition() - mRadius;
		const uint32_t Bounds = 2 * mRadius + 1;
		for (uint32_t y = 0; y < Bounds; y++)
		{
			for (uint32_t x = 0; x < Bounds; x++)
			{
				for (uint32_t z = 0; z < Bounds; z++)
				{
					DestroyBlock(MinPoint + Vector3i{ x, y, z });
				}
			}
		}

		DestroyGameObject();
	}
}