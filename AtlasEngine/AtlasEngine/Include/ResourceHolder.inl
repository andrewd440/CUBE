#pragma once
#include "Misc\StringUtil.h"
#include "Misc\Assertions.h"

template <typename Resource>
inline void TResourceHolder<Resource>::Load(const char* Name)
{
	std::unique_ptr<Resource> ResourcePtr(new Resource);
	uint32_t GUID = FString::HashCRC32(Name);
	auto Insert = mResourceMap.insert({ GUID, std::move(ResourcePtr) });
	ASSERT(Insert.second && "Duplicate keys in resource map.");
}

template <typename Resource>
inline void TResourceHolder<Resource>::Load(const char* Name, const wchar_t* Filename)
{
	std::unique_ptr<Resource> ResourcePtr(new Resource(Filename));
	uint32_t GUID = FString::HashCRC32(Name);
	auto Insert = mResourceMap.insert({ GUID, std::move(ResourcePtr) });
	ASSERT(Insert.second && "Duplicate keys in resource map.");
}

template <typename Resource>
template <typename FirstParam>
inline void TResourceHolder<Resource>::Load(const char* Name, const wchar_t* Filename, const FirstParam& Param)
{
	std::unique_ptr<Resource> ResourcePtr(new Resource(Filename, Param));
	uint32_t GUID = FString::HashCRC32(Name);
	auto Insert = mResourceMap.insert({ GUID, std::move(ResourcePtr) });
	ASSERT(Insert.second && "Duplicate keys in resource map.");
}

template <typename Resource>
inline void TResourceHolder<Resource>::Unload(const char* Name)
{
	auto NumErased = mResourceMap.erase(FString::HashCRC32(Name));
	ASSERT(NumErased == 1 && "Tried to unload a non-existent resource.");
}

template <typename Resource>
inline Resource& TResourceHolder<Resource>::Get(const char* Name)
{
	auto Found = mResourceMap.find(FString::HashCRC32(Name));
	ASSERT(Found != mResourceMap.end() && "Resource not in resource map.");
	return *Found->second;
}

template <typename Resource>
std::unordered_map<uint32_t, std::unique_ptr<Resource>> TResourceHolder<Resource>::mResourceMap;