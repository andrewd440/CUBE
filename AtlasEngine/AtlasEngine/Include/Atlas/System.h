#pragma once
#include <memory>
#include <vector>
#include <bitset>

#include "Bitsize.h"
#include "FGameObject.h"
#include "ComponentHandleManager.h"

namespace Atlas
{
	/**
	* Base class for all Systems
	*/
	class System : public NonCopyable
	{
	public:
		/** Standard pointer when referring to a System */
		typedef std::unique_ptr<System> Ptr;

		/**
		* Creates a new System.
		* @param world - the World object that owns this System
		*/
		System();
		virtual ~System();

		/**
		* Proccesses all Entities relevant to this System.
		* @param dt - update time
		*/
		virtual void update(float dt) = 0;

		/**
		* Checks to see if the System is interested in an FGameObject based on which
		* Components it owns. This is called everytime an FGameObject removes or adds
		* a IComponent. If the System is no longer interested in an FGameObject that
		* it already contains, the FGameObject will be removed from the System.
		* @param FGameObject - the FGameObject to be checked
		*/
		virtual void checkInterest(FGameObject& FGameObject);

		/**
		* Retrieves the system type bits that are assigned to this System.
		* @return Bitset assigned to this System
		*/
		std::bitset<BITSIZE> getSystemBits() const;

		/**
		* Prints Entities in the System, as well as this System's TypeBits and SystemBits
		*/
		void toString();

	protected:
		template <typename T>
		/**
		* Adds a IComponent type for the System to process.
		*
		* Usage Example: (in HealthSystem constructor)
		* ~~~~~~~~~~~~~~~~~~~~~
		* addComponentType<HealthComponent>();
		* ~~~~~~~~~~~~~~~~~~~~~
		*/
		void addComponentType()
		{
			mTypeBits |= SComponentHandleManager::getBits<T>();
		}

		/**
		* Retrieves the IComponent bit types that the System will process.
		* @return Bitset of IComponent types
		*/
		std::bitset<BITSIZE> getTypeBits() const;

		/**
		* Retrieves the FGameObject::IDs of interested Entities
		* @return Container of FGameObject::IDs
		*/
		const std::vector<FGameObject::ID>& getEntities() const;

		/**
		* Retrieves the World object for this System
		*/
		World& getWorld() const;

	private:
		friend class SystemManager;  // Give full access to SystemManager

		/**
		* Removes an FGameObject from the System.
		* @params FGameObject - the FGameObject to be removed
		*/
		virtual void remove(FGameObject& FGameObject);

		/**
		* Assigns a bitset for the System
		* @params Bit - the bit to be assigned
		*/
		void setSystemBits(const std::bitset<BITSIZE>& Bit);

	private:
		World&                          mWorld;
		std::bitset<BITSIZE>            mTypeBits;
		std::bitset<BITSIZE>            mSystemBits;
		std::vector<FGameObject::ID>         mEntities;
	};
}