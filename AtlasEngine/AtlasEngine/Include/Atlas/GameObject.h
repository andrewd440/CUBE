#pragma once
#include "Component.h"
#include "NonCopyable.h"
#include "Bitsize.h"
#include "ComponentHandleManager.h"

#include <vector>
#include <memory>

namespace Atlas
{
	/**
	* Used to represent any game object. 
	* Acts as a grouping/container for Components.
	*/
	class FGameObject
	{
	public:
		/** ID type for an FGameObject */
		using ID = uint32_t;

		template <typename Type>
		/**
		* Retrieves a specified component from the GameObject.
		* @return IComponent casted to it's derived type
		*/
		Type* GetComponent() const;

		template <typename T>
		/**
		* Adds a component to the GameObject.
		* @return A reference to the added component.
		*/
		T& AddComponent();

		template <typename T>
		/**
		* Removes a specified IComponent from the FGameObject.
		*/
		void RemoveComponent();

		/**
		* Retrieves all Components that are attached to the GameObject.
		* @return All Components owned by the FGameObject
		*/
		std::vector<IComponent*> GetAllComponents() const;

		/**
		* Removes all component current attached to this GameObject.
		*/
		void RemoveAllComponents();

		/**
		* Retrieves the ID assigned to the FGameObject
		* @return The ID
		*/
		ID GetID() const;

		/**
		* Checks if the FGameObject is active
		* @return True if FGameObject is active
		*/
		bool IsActive() const;

		/**
		* Sets the active state of the FGameObject. Inactive entity components are not
		* processed.
		* @param Flag - Status of the GameObject
		*/
		void SetActive(bool Flag);

		/**
		* Sets this GameObject to be destroyed. Objects are not instantly destroyed,
		* but destroy on the next GO manager update.
		*/
		void Destroy();

	private:
		friend class FGameObjectManager;
		friend class FSystemManager;

		FGameObject(const FGameObject::ID ID);	// Only the GameObject Manager creates Entities

		/**
		* Sets the ID for the GameObject.
		* This is not a GUID, it is just an index in the GameObject Managers' container.
		* @param ID - ID to be assigned
		*/
		void SetID(const FGameObject::ID ID);

		/**
		* Adds a component bit to the GameObject.
		* Used by the GameObject Manager when a component is added to a GameObject.
		* @param Bit - The bit to be added
		*/
		void AddComponentBit(const std::bitset<BITSIZE>& Bit);

		/**
		* Removes a component bit from the GameObject.
		* Used by the GameObject Manager when a component is removed from a GameObject.
		* @param Bit - Bit to be removed
		*/
		void RemoveComponentBit(const std::bitset<BITSIZE>& Bit);

		/**
		* Retrieves the component bits in use by this GameObject.
		*/
		std::bitset<BITSIZE> GetComponentBitMask() const;

		/**
		* Sets a System bit to be assigned to the GameObject
		* @param Bit - System bit to be added
		*/
		void SetSystemBit(const std::bitset<BITSIZE>& Bit);

		/**
		* Removes a system bit from the GameObject
		* @param Bit - bit to be removed
		*/
		void RemoveSystemBit(const std::bitset<BITSIZE>& Bit);

		/**
		* Retrieves the system bits assigned to this GameObject
		* @return Set of system bits
		*/
		std::bitset<BITSIZE> GetSystemBitMask() const;

	private:
		std::bitset<BITSIZE>            mComponentBits; // Property component types currently attached.
		std::bitset<BITSIZE>            mSystemBits;    // Systems currently registered in.
		FGameObjectManager&             mGOManager;
		uint32_t                        mComponents[EComponent::Count]; // Handles for common property components.
		ID                              mID;            // Non-unique id for this GO.
		bool                            mIsActive;      // If not active, this GO's components will not be processed.
		bool                            mToBeDestroyed; // If set, this GO will be destroy on the next GO manager update.
	};
}

#include "GameObject.inl"