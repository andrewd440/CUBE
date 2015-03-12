#pragma once

#include "ComponentHandleManager.h"
#include "GameObjectManager.h"
#include "GameObject.h"
#include "SystemManager.h"
#include "Components\RigidBody.h"
#include "Components\Collider.h"

namespace Atlas
{
	template <EComponent::Type Type>
	inline void FGameObjectManager::RegisterComponentType()
	{
		using ComponentType = ComponentTraits::Object<Type>::Type;
		mSystemComponents[Type].Init<ComponentType>(DEFAULT_CONTAINER_SIZE);
	}

	template <EComponent::Type Type>
	inline typename ComponentTraits::Object<Type>::Type& FGameObjectManager::AddComponent(FGameObject& GameObject)
	{
		using ComponentType = ComponentTraits::Object<Type>::Type;
		ComponentType& Component = *reinterpret_cast<ComponentType*>(AllocateComponentForObject(Type, GameObject));

		UpdateComponentSystems(GameObject, *reinterpret_cast<IComponent*>(&Component));

		return Component;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////// RigidBody and Collider specializations ////////////////////////////////////////////////////////////////////////
	template <>
	inline typename ComponentTraits::Object<EComponent::RigidBody>::Type& FGameObjectManager::AddComponent<EComponent::RigidBody>(FGameObject& GameObject)
	{
		const EComponent::Type Type = EComponent::RigidBody;
		using ComponentType = ComponentTraits::Object<Type>::Type;

		// Allocate then construct the component
		ComponentType* Component = reinterpret_cast<ComponentType*>(AllocateComponentForObject(Type, GameObject));
		new (Component) ComponentType(static_cast<btMotionState*>(&GameObject));

		UpdateComponentSystems(GameObject, *reinterpret_cast<IComponent*>(Component));

		return *Component;
	}

	template <>
	inline typename ComponentTraits::Object<EComponent::Collider>::Type& FGameObjectManager::AddComponent<EComponent::Collider>(FGameObject& GameObject)
	{
		const EComponent::Type Type = EComponent::Collider;
		using ComponentType = ComponentTraits::Object<Type>::Type;

		// Allocate then construct the component
		ComponentType* Component = reinterpret_cast<ComponentType*>(AllocateComponentForObject(Type, GameObject));
		new (Component)ComponentType(static_cast<btMotionState*>(&GameObject));

		UpdateComponentSystems(GameObject, *reinterpret_cast<IComponent*>(Component));

		return *Component;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	template <EComponent::Type Type>
	inline typename ComponentTraits::Object<Type>::Type& FGameObjectManager::GetComponent(const uint32_t IndexHandle)
	{
		using ComponentType = ComponentTraits::Object<Type>::Type;

		return mSystemComponents[Type].At<ComponentType>(IndexHandle);
	}

	template <EComponent::Type Type>
	inline typename ComponentTraits::Object<Type>::Type& FGameObjectManager::GetComponentFor(const FGameObject::ID ID)
	{
		using ComponentType = ComponentTraits::Object<Type>::Type;
		const uint32_t ComponentIndex = mGameObjects.At<FGameObject>(ID).mComponents[Type];

		ASSERT(ComponentIndex != FGameObject::NULL_COMPONENT && "Trying to retrieve a null component.");
		return mSystemComponents[Type].At<ComponentType>(ComponentIndex);
	}

	template <EComponent::Type Type>
	inline typename ComponentTraits::Object<Type>::Type& FGameObject::GetComponent()
	{
		ASSERT(mComponents[Type] != NULL_COMPONENT);
		return mGOManager.GetComponent<Type>(mComponents[Type]);
	}

	template <EComponent::Type Type>
	inline typename ComponentTraits::Object<Type>::Type& FGameObject::AddComponent()
	{
		return mGOManager.AddComponent<Type>(*this); 
	}

	inline void FGameObject::RemoveComponent(const EComponent::Type Type)
	{
		mGOManager.RemoveComponent(*this, Type);
	}

	inline std::vector<IComponent*> FGameObject::GetAllComponents()
	{
		return mGOManager.GetAllComponentsFor(mID);
	}

	inline void FGameObject::RemoveAllComponents()
	{
		mGOManager.RemoveAllComponentsFor(mID);
	}

	inline FGameObject::ID FGameObject::GetID() const
	{
		return mID;
	}

	inline void FGameObject::SetID(const FGameObject::ID ID)
	{
		mID = ID;
	}

	inline bool FGameObject::IsActive() const
	{
		return mIsActive;
	}

	inline void FGameObject::SetActive(bool Flag)
	{
		mIsActive = Flag;
	}

	inline void FGameObject::Destroy()
	{
		mGOManager.DestroyGameObject(*this);
	}

	inline std::bitset<BITSIZE> FGameObject::GetComponentBitMask() const
	{
		return mComponentBits;
	}

	inline void FGameObject::AddComponentBit(const std::bitset<BITSIZE>& bit)
	{
		mComponentBits |= bit;
	}

	inline void FGameObject::RemoveComponentBit(const std::bitset<BITSIZE>& bit)
	{
		mComponentBits &= ~bit;
	}

	inline void FGameObject::SetSystemBit(const std::bitset<BITSIZE>& bit)
	{
		mSystemBits |= bit;
	}

	inline void FGameObject::RemoveSystemBit(const std::bitset<BITSIZE>& bit)
	{
		mSystemBits &= ~bit;
	}

	inline std::bitset<BITSIZE> FGameObject::GetSystemBitMask() const
	{
		return mSystemBits;
	}
}