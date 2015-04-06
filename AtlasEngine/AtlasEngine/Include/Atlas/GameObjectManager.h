#pragma once
#include <queue>

#include "ComponentTypes.h"
#include "Containers\RawGappedArray.h"

class FChunkManager;

namespace Atlas
{
	class FGameObject;
	class IComponent;
	class FWorld;
	class FSystemManager;

	/**
	* Manages all gameobjects and system components within the a world.
	* Entities and Components will be directly or indirectly retrieved
	* and added through the GameObject Manager.
	*/
	class FGameObjectManager
	{
	public:
		/**
		* Creates a new GameObject Manager.
		*/
		FGameObjectManager(FWorld& World);

		FGameObjectManager(const FGameObjectManager& Other) = delete;
		FGameObjectManager& operator=(const FGameObjectManager& Other) = delete;

		/**
		* Initializes data for the start of the 
		* simulation.
		*/
		void Start();

		/**
		* Updates the GameObject manager.
		*/
		void Update();

		/**
		* Creates an empty GameObject
		* @returns Reference to the new GameObject
		*/
		FGameObject& CreateGameObject();

		/**
		* Retreives an GameObject by ID
		* @param ID - The GameObject's ID
		* @returns Reference to the GameObject
		*/
		FGameObject& GetGameObject(const uint32_t GameObjectID);

		template <EComponent::Type Type>
		/**
		* Registers a component type to this manager. This must be a component
		* type that is a part of the EComponent::Type enum and must have a declared
		* set of ComponentTraits.
		* @tparam Type - The type of component to register.
		*/
		void RegisterComponentType();

		template <EComponent::Type Type>
		/**
		* Attaches a system component to a GameObject.
		* @tparam Type - The type of component to add.
		* @param GameObject - The GameObject to add the component to.
		* @return A reference to the added component
		*/
		typename ComponentTraits::Object<Type>::Type& AddComponent(FGameObject& GameObject);

		/**
		* Removes a system component from a GameObject. If the game object doesn't not have the
		* specified component, this function will do nothing.
		* @tparam Type - The type of component to remove.
		* @param GameObject - The GameObject to remove the component from.
		*/
		void RemoveComponent(FGameObject& GameObject, EComponent::Type Type);

		template <EComponent::Type Type>
		/**
		* Retrieve a component type by it's index handle into the
		* GameObject Manager's container for that specific component
		* type.
		* @param IndexHandle - The index into the component container.
		* @return A reference to the component.
		*/
		typename ComponentTraits::Object<Type>::Type& GetComponent(const uint32_t IndexHandle);
		
		template <EComponent::Type Type>
		/**
		* Retrieve a specific system component from a GameObject
		* @param ID - The targeted FGameObject's ID
		* @return A reference to the derived type IComponent
		*/
		typename ComponentTraits::Object<Type>::Type& GetComponentFor(const uint32_t ID);

		/**
		* Retrieve all components of a specific type.
		* @param Type - The targeted component type
		* @return A reference to the container of that component type held by this object.
		*/
		FRawGappedArray& GetComponentsOfType(const EComponent::Type Type);

		/**
		* Retrieves all the components attached to a GameObject.
		* @param ID - The ID of the targeted GameObject
		* @return Vector of component pointers from the GameObject
		*/
		std::vector<IComponent*> GetAllComponentsFor(const uint32_t ID);

		/**
		* Removes and destroys all the Components attached to a FGameObject.
		* @param ID - The ID of the targeted GameObject
		*/
		void RemoveAllComponentsFor(const uint32_t ID);

		uint32_t GetGameObjectCount() const { return mGameObjects.Size(); }

		/**
		* Sets a gameobject to be destroyed.
		* @param GameObject - The targeted GameObject
		*/
		void DestroyGameObject(FGameObject& GameObject);

		void SetChunkManager(FChunkManager* ChunkManager) { mChunkManager = ChunkManager; }

	private:
		/**
		* Resets a GameObject and moves it from the active GameObject container to the dead GameObject pool
		* @param GameObject - The targeted GameObject
		*/
		void DestroyGameObjectHelp(FGameObject& GameObject);

		void* AllocateComponentForObject(const EComponent::Type Type, FGameObject& GameObject);

		void UpdateComponentSystems(FGameObject& GameObject, IComponent& UpdatedComponent);

	private:
		static const uint32_t DEFAULT_CONTAINER_SIZE = 500;

	private:
		// The world this manager represents
		FSystemManager& mSystemManager;

		FChunkManager* mChunkManager;

		// Holds all gameobjects
		FRawGappedArray mGameObjects;

		// Holds all system based components.
		FRawGappedArray mSystemComponents[EComponent::Type::Count];

		// List of gameobjects set to be destroyed
		std::queue<FGameObject*> mDestroyQueue;
	};
}

#include "GameObject.inl"