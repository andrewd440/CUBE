#pragma once
#include <unordered_map>
#include <cstdint>
#include <memory>

#include "Singleton.h"

template <typename Resource>
/**
* Singleton class for loading resources from files.
*/
class TResourceHolder : public TSingleton<TResourceHolder<Resource>>
{
public:

	TResourceHolder() = default;
	~TResourceHolder() = default;

	/**
	* Loads a resource from a file and stores it with
	* a specified name.
	*/
	void Load(const char* Name, const wchar_t* Filename);

	template <typename FirstParam>
	/**
	* Loads a resource from a file and stores it with
	* a specified name.
	* @param Param - Parameter for constructing the resource type.
	*/
	void Load(const char* Name, const wchar_t* Filename, const FirstParam& Param);

	/**
	* Deletes a resource stored in this holder.
	*/
	void Unload(const char* Name);

	/**
	* Retrieves a resource by name.
	*/
	Resource& Get(const char* Name);

	/**
	* Retrieves a resource by name.
	*/
	const Resource& Get(const char* Name) const;

private:
	std::unordered_map<uint32_t, std::unique_ptr<Resource>> mResourceMap;
};

class FShader;
using FShaderHolder = TResourceHolder<FShader>;

#include "ResourceHolder.inl"