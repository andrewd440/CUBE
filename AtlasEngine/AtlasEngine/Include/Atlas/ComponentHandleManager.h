#pragma once
#include "Bitsize.h"

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <cstdint>

#include "Component.h"
#include "ComponentHandle.h"

namespace Atlas
{

	/**
	* Used to assign a unique ID and Bit to a System when
	* added to the SystemManager.
	*/
	class SComponentHandleManager
	{
	public:

		/**
		* Retrieves an handle for a property component type.
		* If an handle has not been assigned to the component type, one is created and assigned.
		*
		* @param index - type_index for the property component type
		*/
		static const FComponentHandle& GetHandle(const EComponent::Type Type);

		/**
		* Retrieves the bit mask assigned to a property component type.
		*/
		static std::bitset<BITSIZE> GetBitMask(const EComponent::Type Type)
		{
			return GetHandle(Type).GetBitMask();
		}

	private:
		SComponentHandleManager() = default;	// Not meant for instantiation

		static std::unordered_map<EComponent::Type, FComponentHandle> ComponentMap;	// Map of Components-to-ComponentHandles
	};
}