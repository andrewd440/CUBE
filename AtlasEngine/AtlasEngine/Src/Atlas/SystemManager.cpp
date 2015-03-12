#include "Atlas\SystemManager.h"
#include "Atlas\System.h"

namespace Atlas
{
	FSystemManager::FSystemManager(FWorld& World)
		: mWorld(World)
		, mSystems()
	{
	}

	FSystemManager::~FSystemManager()
	{

	}

	void FSystemManager::CheckInterest(FGameObject& GameObject, IComponent& UpdateComponent)
	{
		// Delegate the all current systems
		for (auto& System : mSystems)
			System->CheckInterest(GameObject, UpdateComponent);
	}
}