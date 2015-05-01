#include "Atlas\ComponentHandleManager.h"
#include "Atlas\ComponentHandle.h"

namespace Atlas
{
	const FComponentHandle& SComponentHandleManager::GetHandle(const EComponent::Type Type)
	{
		// If an identifier is not assigned, assign one
		auto Found = ComponentMap.find(Type);
		if (Found == ComponentMap.end())
		{
			ComponentMap.insert({ Type, FComponentHandle{ Type } });
		}

		return ComponentMap.at(Type);
	}

	std::unordered_map <EComponent::Type, FComponentHandle> SComponentHandleManager::ComponentMap;
}