#pragma once
#include "Bitsize.h"
#include "ComponentTypes.h"

#include <memory>
#include <cstdint>

namespace Atlas
{
	/**
	* Handle used for Components.
	* Distributes a unique ID and Bitset for Components.
	* SComponentHandleManager uses this class to assign
	* Identifiers for each property component type.
	*/
	class FComponentHandle
	{
	public:
		// Default copy semantics
		FComponentHandle(const FComponentHandle& Other) = default;
		FComponentHandle& operator=(const FComponentHandle& Other) = default;

		/**
		* Retrieves the unique ID for the identifier
		*/
		EComponent::Type GetID() const;

		/**
		* Retrieves the unique bitset for the identifier
		*/
		std::bitset<BITSIZE> GetBitMask() const;

	private:
		friend class SComponentHandleManager;	// Give full access to the Manager

		FComponentHandle(const EComponent::Type Type);	// Only to be created by the manager

	private:
		// Next available component bit mask
		static std::bitset<BITSIZE> NextBit;

		EComponent::Type mID;
		std::bitset<BITSIZE> mBit;
	};
}