#include "Atlas\GameObjectManager.h"

#include <iostream>


namespace
{
	const int POOLSIZE = 100;
}

namespace Atlas
{
	FGameObjectManager::FGameObjectManager()
		: mActiveFGameObjectCount(0)
		, mNextAvaibleID(0)
		, mActiveEntities(POOLSIZE)
		, mDeadEntities()
		, mComponentsByType(BITSIZE)
		, mFGameObjectComponents()
	{
	}

	void FGameObjectManager::update()
	{
		// remove inactive entities
		for (auto& entity : mActiveEntities)
			if (entity && !entity->isActive())
				removeFGameObject(*entity);
	}

	FGameObject& FGameObjectManager::CreateGameObject()
	{
		std::unique_ptr<FGameObject> e;

		// If a dead entity is pooled, set it to active and it will be the created entity
		if (!mDeadEntities.empty())
		{
			e = std::move(mDeadEntities.top());
			mDeadEntities.pop();
		}
		// if not, allocate a new entity, move it into our vector and increament ID
		else
		{
			e.reset(new FGameObject(mNextAvaibleID++));
		}

		e->setActive(true);
		mActiveFGameObjectCount++;

		FGameObject::ID id = e->GetID();

		// make sure the id fits the container
		if (mActiveEntities.capacity() <= id)
		{
			mActiveEntities.resize(id * 2 + 1);
		}

		mActiveEntities[id] = std::move(e);

		return *mActiveEntities[id];

	}

	void FGameObjectManager::removeFGameObject(FGameObject& entity)
	{
		// Only remove inactive entities
		assert(!entity.isActive());

		const auto& id = entity.GetID();

		mWorld.getGroupManager().removeFromAllGroups(entity);

		// Deactivate entity and reset properties
		removeAllComponentsFor(entity);
		mActiveEntities[id]->reset();

		// Move entity to dead entity pool and set active element to null
		mDeadEntities.push(std::move(mActiveEntities[id]));

		mActiveEntities[id] = nullptr;

		mActiveFGameObjectCount--;
	}

	FGameObject& FGameObjectManager::getFGameObject(const FGameObject::ID& id) const
	{
		assert(id < mActiveEntities.size());
		return *mActiveEntities[id];
	}

	void FGameObjectManager::addComponent(FGameObject& entity, IComponent::Ptr component)
	{
		const FComponentHandle& identifier = SComponentHandleManager::getType(typeid(*component));
		const FGameObject::ID& eID = entity.GetID();

		auto& typeComponents = mComponentsByType[identifier.GetID()];

		// make sure the id fits the container
		if (typeComponents.capacity() <= eID)
		{
			typeComponents.resize(eID * 2 + 1);
		}

		// check if entity already has component type
		if (typeComponents[eID] != nullptr)
		{
			removeComponent(entity, identifier);
		}

		entity.addComponentBit(identifier.GetBitMask());
		typeComponents[eID] = std::move(component);

		// Check all systems for interest in new component
		mWorld.getSystemManager().checkInterest(entity);
	}

	void FGameObjectManager::removeComponent(FGameObject& entity, const FComponentHandle& id)
	{
		// Check if specific component vector not out of range
		assert(mComponentsByType[id.GetID()].capacity() > entity.GetID());

		// Check if entity has component first
		assert(mComponentsByType[id.GetID()][entity.GetID()] != nullptr);

		entity.removeComponentBit(id.GetBitMask());
		mComponentsByType[id.GetID()][entity.GetID()] = nullptr;

		// Check all systems to see if still interested in entity
		mWorld.getSystemManager().checkInterest(entity);
	}

	std::vector<IComponent*> FGameObjectManager::getAllComponentsFor(const FGameObject& entity)
	{
		mFGameObjectComponents.clear();

		const auto& entityBits = entity.getComponentBits();

		for (int i = 0; i < entityBits.size(); i++)
		{
			if (entityBits[i])
				mFGameObjectComponents.push_back(mComponentsByType[i][entity.GetID()].get());
		}

		return mFGameObjectComponents;
	}

	void FGameObjectManager::removeAllComponentsFor(FGameObject& entity)
	{
		const auto& id = entity.GetID();

		for (int i = 0; i < mComponentsByType.size(); i++)
		{
			// check if vector is out of range and if there is a component for the entity present
			if (mComponentsByType[i].size() > id && mComponentsByType[i][id] != nullptr)
			{
				// get FComponentHandle for this component
				const auto& type = SComponentHandleManager::getType(typeid(*mComponentsByType[i][id]));

				entity.removeComponentBit(type.GetBitMask());
				mComponentsByType[i][id] = nullptr;
			}
		}

		// Check all systems to remove entity from them
		mWorld.getSystemManager().checkInterest(entity);
	}

	void FGameObjectManager::toString()
	{
		using namespace std;

		cout << "-------------------------------------------------"
			<< "\n|\t\tFGameObjectManager\t\t\t|"
			<< "\n-------------------------------------------------"
			<< endl;

		cout << "\t\tActive Entities\t\t\t"
			<< "\n-------------------------------------------------"
			<< endl;

		for (const auto& entity : mActiveEntities)
			if (entity != nullptr)
				entity->toString();

		cout << "\t\tInactive Entities: " << mDeadEntities.size() << "\t\t"
			<< "\n-------------------------------------------------"
			<< endl << endl;
	}
}