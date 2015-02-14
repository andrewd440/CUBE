#include "Atlas\GameObject.h"

namespace Atlas
{
	FGameObject::FGameObject(FGameObjectManager& GOManager)
		: mComponentBits()
		, mSystemBits()
		, mGOManager(GOManager)
		, mID(0)
		, mIsActive(true)
		, mToBeDestroyed(false)
	{
		for (uint32_t i = 0; i < EComponent::Count; i++)
			mComponents[i] = NULL_COMPONENT;
	}
}