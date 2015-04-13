#include "Components\TimeBomb.h"
#include "STime.h"
#include "Atlas\GameObject.h"


CTimeBomb::CTimeBomb()
	: FBehavior()
	, mLifetime(3.0f)
	, mTimer(0.0f)
	, mRadius(10)
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
		const int32_t Bounds = 2 * mRadius + 1;

		for (int32_t y = 0; y < Bounds; y++)
		{
			for (int32_t x = 0; x < Bounds; x++)
			{
				for (int32_t z = 0; z < Bounds; z++)
				{
					if (sqrt((float)(x - Bounds / 2)*(x - Bounds / 2) + (y - Bounds / 2)*(y - Bounds / 2) + (z - Bounds / 2)*(z - Bounds / 2)) <= Bounds / 2)
					{
						DestroyBlock(MinPoint + Vector3i{ x, y, z });
					}
				}
			}
		}

		DestroyGameObject();
	}
}