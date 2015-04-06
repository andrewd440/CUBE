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

	void FGameObjectManager::Start()
	{
		for (auto Itr = mGameObjects.Begin<FGameObject>(); Itr != mGameObjects.End<FGameObject>(); Itr++)
		{
			Itr->OnStart();
		}
	}

	void FGameObjectManager::Update()
	{
		// remove destroyed GOs
		while(!mDestroyQueue.empty())
		{
			DestroyGameObjectHelp(*mDestroyQueue.front());
			mDestroyQueue.pop();
		}

		for (auto Itr = mGameObjects.Begin<FGameObject>(); Itr != mGameObjects.End<FGameObject>(); Itr++)
		{
			Itr->Update();
		}
	}

	FGameObject& FGameObjectManager::CreateGameObject()
	{
		// Get the index for the new object
		uint32_t Index = mGameObjects.Allocate();

		// In-place allocate it with this memory
		new (&(mGameObjects.At<FGameObject>(Index))) FGameObject(*this, *mChunkManager);

		// Set the ID with the index
		FGameObject& NewObject = mGameObjects.At<FGameObject>(Index);
		NewObject.SetID(Index);

		std::cout << mGameObjects.Size() << std::endl;

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

		mGameObjects.At<FGameObject>(ID).~FGameObject();
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

		const uint32_t ComponentIndex = GameObject.mComponents[Type];
		IComponent& RemovedComponent = mSystemComponents[Type].At<IComponent>(ComponentIndex);

		// Notify component systems
		mSystemManager.CheckInterest(GameObject, RemovedComponent);

		// Free the component
		mSystemComponents[Type].Free(ComponentIndex);

		GameObject.mComponents[Type] = FGameObject::NULL_COMPONENT;
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
				RemoveComponent(Object, Atlas::EComponent::Type(i));
			}
		}
	}

	void* FGameObjectManager::AllocateComponentForObject(const EComponent::Type Type, FGameObject& GameObject)
	{
		// Check if the object already has this component
		ASSERT(GameObject.mComponents[Type] == FGameObject::NULL_COMPONENT && "Trying to add duplicate component to gameobject.");

		GameObject.AddComponentBit(SComponentHandleManager::GetBitMask(Type));
		uint32_t ComponentIndex = mSystemComponents[Type].Allocate();
		GameObject.mComponents[Type] = ComponentIndex;

		// Notify component systems and return component
		void* Component = mSystemComponents[Type][ComponentIndex];

		return Component;
	}

	void FGameObjectManager::UpdateComponentSystems(FGameObject& GameObject, IComponent& UpdatedComponent)
	{
		mSystemManager.CheckInterest(GameObject, UpdatedComponent);
	}
}