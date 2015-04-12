#pragma once
#include <map>
#include <cstdint>
#include <memory>

#include "Singleton.h"

template <typename Resource>
/**
* Singleton class for loading resources from files.
*/
class TResourceHolder
{
public:
	TResourceHolder() = delete;
	~TResourceHolder() = delete;

	/**
	* Loads a resource and stores it with
	* a specified name.
	*/
	static void Load(const char* Name);

	/**
	* Loads a resource from a file and stores it with
	* a specified name.
	*/
	static void Load(const char* Name, const wchar_t* Filename);

	template <typename FirstParam>
	/**
	* Loads a resource from a file and stores it with
	* a specified name.
	* @param Param - Parameter for constructing the resource type.
	*/
	static void Load(const char* Name, const wchar_t* Filename, const FirstParam& Param);

	/**
	* Deletes a resource stored in this holder.
	*/
	static void Unload(const char* Name);

	/**
	* Retrieves a resource by name.
	*/
	static Resource& Get(const char* Name);

private:
	static std::map<uint32_t, std::unique_ptr<Resource>> mResourceMap;
};

class FShader;
using SShaderHolder = TResourceHolder<FShader>;

class FShaderProgram;
using SShaderProgramHolder = TResourceHolder<FShaderProgram>;

#include "ResourceHolder.inl"