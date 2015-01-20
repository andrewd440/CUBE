#pragma once

#include "Misc\Assertions.h"

template <typename T>
/**
* Base class for all singleton objects
*/
class TSingleton
{
public:

	TSingleton()
	{
		ASSERT(!Instance && "Only one instance is allowed.");
		Instance = static_cast<T*>(this);
	}

	virtual ~TSingleton()
	{
		Instance = nullptr;
	}

	static T& GetInstance()
	{
		ASSERT(Instance != nullptr);
		return *Instance;
	}

	static T* GetInstancePtr()
	{
		ASSERT(Instance != nullptr);
		return Instance;
	}

private:
	// Disable copy ctor and assignment
	TSingleton(const TSingleton<T>& Other) = delete;
	TSingleton<T>& operator=(const TSingleton<T>& Other) = delete;

protected:
	/** Derived classes must initialize this */
	static T* Instance;
};

template <typename T>
T* TSingleton<T>::Instance = nullptr;
