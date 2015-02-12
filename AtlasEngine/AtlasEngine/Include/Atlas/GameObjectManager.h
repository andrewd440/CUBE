#pragma once
#include <stack>
#include <vector>
#include <memory>
#include <cassert>

#include "GameObject.h"
#include "ComponentHandle.h"
#include "Memory\GapArrayAllocator.h"

namespace Atlas
{
	/**
	* Manages all Entities and Components within the a world.
	* Entities and Components will be directly or indirectly retrieved
	* and added through the GameObject Manager.
	*/
	class FGameObjectManager : public NonCopyable
	{
	public:
		/**
		* Creates a new FGameObjectManager.
		* @param world - the World object that owns this FGameObjectManager
		*/
		FGameObjectManager();

		/**
		* Removes all entities that have been set to inactive
		*/
		void Update();

		/**
		* Creates an empty FGameObject
		* @returns Reference to the new FGameObject
		*/
		FGameObject& CreateGameObject();

		/**
		* Retreives an active FGameObject
		* @param ID - The GameObject's ID
		* @returns Reference to the GameObject
		*/
		FGameObject& GetGameObject(const FGameObject::ID GameObjectID) const;

		template <EComponent::Type Type>
		/**
		* Attaches a property component to a GameObject.
		* @param GameObject - The GameObject to add the component to.
		* @param ComponentType - The type of component to add.
		* @return A reference to the added component
		*/
		EComponent_Type<Type>::Type& AddComponent(FGameObject& GameObject);

		template <typename T>
		/**
		* Removes a property component from a GameObject. If the game object doesn't not have the
		* specified component, this function will do nothing.
		* @param GameObject - The GameObject to add the component to.
		* @param ComponentType - The type of component to remove.
		*/
		void RemoveComponent(FGameObject& GameObject);

		template <typename T>
		/**
		* Retieve a component type by it's index handle into the
		* GameObject Manager's container for that specific component
		* type.
		* @param IndexHandle - The index into the component container.
		* @return A reference to the component.
		*/
		T& GetComponent(const uint32_t IndexHandle);

		template <typename T>
		/**
		* Retrieve a specific property component from an GameObject
		* @param ID - The targeted FGameObject's ID
		* @return Pointer to the derived type IComponent
		*/
		T& GetComponentFor(const FGameObject::ID ID) const;

		/**
		* Retrieves all the components attached to a GameObject.
		* @param ID - The ID of the targeted GameObject
		* @return Vector of component pointers from the GameObject
		*/
		std::vector<IComponent*> GetAllComponentsFor(const FGameObject::ID ID);

		/**
		* Removes and destroys all the Components attached to a FGameObject.
		* @param ID - The ID of the targeted GameObject
		*/
		void RemoveAllComponentsFor(const FGameObject::ID ID);

	private:
		/**
		* Resets a GameObject and moves it from the active GameObject container to the dead GameObject pool
		* @param GameObject - The targeted GameObject
		*/
		void DestroyGameObject(FGameObject& GameObject);

	private:
		unsigned int                mActiveFGameObjectCount;
		FGameObject::ID                  mNextAvaibleID;

		/** Holds all entities with the ID being used as an index */
		std::vector<FGameObject::Ptr>   mActiveEntities;

		/** Used as a pool for entity reuse */
		std::stack<FGameObject::Ptr>    mDeadEntities;

		/**
		* Holds all Components sorted by its parent FGameObject and type.
		* Rows are sorted by ComponentID
		* Columns are sorted by ID
		*/
		FGapArrayAllocator mComponentsByType[EComponent::Type::Count];

		/** Temp container used when retrieving components for a single FGameObject */
		std::vector<IComponent*>      mFGameObjectComponents;
	};
}