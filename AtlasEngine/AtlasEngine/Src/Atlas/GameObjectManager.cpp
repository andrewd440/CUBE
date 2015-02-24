#include "Atlas\GameObjectManager.h"
#include "Atlas\World.h"
#include "Atlas\ComponentTypes.h"
#include "Atlas\GameObject.h"
#include "Atlas\SystemManager.h"

namespace Atlas
{
	FGameObjectManager::FGameObjectManager(FWorld& World)
		: mSystemManager(World.GetSystemManager())
		, mGameObjects()
		, mSystemComponents()
		, mDestroyQueue()
	{
		mGameObjects.Init<FGameObject>(DEFAULT_CONTAINER_SIZE);
	}

	void FGameObjectManager::Update()
	{
		// remove destroyed GOs
		while(!mDestroyQueue.empty())
		{
			DestroyGameObjectHelp(*mDestroyQueue.front());
			mDestroyQueue.pop();
		}
	}

	FGameObject& FGameObjectManager::CreateGameObject()
	{
		// Get the index for the new object
		uint32_t Index = mGameObjects.Allocate();

		// In-place allocate it with this memory
		new (&(mGameObjects.At<FGameObject>(Index))) FGameObject(*this);

		// Set the ID with the index
		FGameObject& NewObject = mGameObjects.At<FGameObject>(Index);
		NewObject.SetID(Index);

		return NewObject;
	}

	void FGameObjectManager::DestroyGameObject(FGameObject& GameObject)
	{
		mDestroyQueue.push(&GameObject);
	}

	void FGameObjectManager::DestroyGameObjectHelp(FGameObject& GameObject)
	{
		// Deactivate entity and reset properties
		const uint32_t ID = GameObject.mID;
		RemoveAllComponentsFor(ID);
		mGameObjects.Free(ID);
	}

	FGameObject& FGameObjectManager::GetGameObject(const uint32_t GameObjectID)
	{
		ASSERT(GameObjectID < mGameObjects.Capacity());
		return mGameObjects.At<FGameObject>(GameObjectID);
	}

	FRawGappedArray& FGameObjectManager::GetComponentsOfType(const EComponent::Type Type)
	{
		return mSystemComponents[Type];
	}

	void FGameObjectManager::RemoveComponent(FGameObject& GameObject, EComponent::Type Type)
	{
		GameObject.RemoveComponentBit(SComponentHandleManager::GetBitMask(Type));
		mSystemComponents[Type].Free(GameObject.GetID());
		GameObject.mComponents[Type] = FGameObject::NULL_COMPONENT;

		// Notify component systems
		mSystemManager.CheckInterest(GameObject);
	}

	std::vector<IComponent*> FGameObjectManager::GetAllComponentsFor(const uint32_t ID)
	{
		std::vector<IComponent*> Components(EComponent::Count);
		const FGameObject& Object = mGameObjects.At<FGameObject>(ID);

		for (uint32_t i = 0; i < EComponent::Count; i++)
		{
			const uint32_t ComponentIndex = Object.mComponents[i];
			if (ComponentIndex != FGameObject::NULL_COMPONENT)
				Components[i] = &mSystemComponents[i].At<IComponent>(ComponentIndex);
		}

		return Components;
	}

	void FGameObjectManager::RemoveAllComponentsFor(const uint32_t ID)
	{
		FGameObject& Object = mGameObjects.At<FGameObject>(ID);

		for (uint32_t i = 0; i < EComponent::Count; i++)
		{
			uint32_t ComponentIndex = Object.mComponents[i];
			if (ComponentIndex != FGameObject::NULL_COMPONENT)
			{
				Object.mComponents[i] = FGameObject::NULL_COMPONENT;
				mSystemComponents[i].Free(ComponentIndex);
			}
		}

		// Clear all component bits
		Object.mComponentBits.reset();
		mSystemManager.CheckInterest(Object);
	}
}