#pragma once
#include "Bitsize.h"

#include <unordered_map>
#include <typeindex>

namespace Atlas
{
	/**
	* Used to distribute a unique bit identifier to each System created.
	*/
	class SSystemBitManager
	{
	public:
		/**
		* Retrieves the bitmask for a System
		* @param type - the System of interest
		* @return A bitset containing the System bit
		*/
		static std::bitset<BITSIZE> GetBitMaskFor(const std::type_index& type);

		template <typename T>
		/**
		* Retrieves the bitmask for a System
		* @param type - pointer to a System
		* @return A bitset containing the System bit
		*/
		static std::bitset<BITSIZE> GetBitMaskFor(const T* type)
		{
			return GetBitMaskFor(typeid(*type));
		}


	private:
		SSystemBitManager() = delete; 		//Not meant for instantiation

		static std::bitset<BITSIZE>                                         mNextBit;
		static std::unordered_map<std::type_index, std::bitset<BITSIZE>>    mSystemBitMap;
	};
}