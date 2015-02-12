#include "Atlas\GroupManager.h"

#include <cassert>
#include <iostream>

namespace Atlas
{
	GroupManager::GroupManager()
		: mGroupsByName()
		, mFGameObjectGroups()
	{

	}

	void GroupManager::addToGroup(const std::string& name, const FGameObject& entity)
	{
		const auto& ID = entity.GetID();

		mGroupsByName[name].push_back(ID);

		// make sure vector is large enough
		if (mFGameObjectGroups.size() <= ID)
			mFGameObjectGroups.resize(ID * 2 + 1);

		mFGameObjectGroups[ID].push_back(name);
	}

	std::vector<FGameObject::ID> GroupManager::getGroup(const std::string& name)
	{
		return mGroupsByName[name];
	}

	std::vector<std::string> GroupManager::getFGameObjectGroups(const FGameObject& entity) const
	{
		const auto& ID = entity.GetID();

		assert(mFGameObjectGroups.size() > ID);
		return mFGameObjectGroups[ID];
	}

	void GroupManager::removeFromGroup(const std::string& name, const FGameObject& entity, bool fullErase)
	{
		const auto& ID = entity.GetID();

		auto positionItr = std::find(mGroupsByName[name].begin(), mGroupsByName[name].end(), ID);
		if (positionItr != mGroupsByName[name].end())
			mGroupsByName[name].erase(positionItr);

		// if the entity is not being removed from all groups, lookup and remove it from entityGroups
		if (!fullErase)
		{
			auto entityItr = std::find(mFGameObjectGroups[ID].begin(), mFGameObjectGroups[ID].end(), name);
			if (entityItr != mFGameObjectGroups[ID].end())
				mFGameObjectGroups[ID].erase(entityItr);
		}
	}

	void GroupManager::removeFromAllGroups(const FGameObject& entity)
	{
		const auto& ID = entity.GetID();

		for (const auto& name : mFGameObjectGroups[ID])
		{
			removeFromGroup(name, entity, true);
		}

		mFGameObjectGroups[ID].clear();
	}

	void GroupManager::toString()
	{
		using namespace std;

		for (auto itr = mGroupsByName.begin(); itr != mGroupsByName.end(); itr++)
		{
			cout << itr->first << ": ";
			for (const auto& ID : itr->second)
				cout << ID << ", ";
			cout << endl;
		}
	}
}
