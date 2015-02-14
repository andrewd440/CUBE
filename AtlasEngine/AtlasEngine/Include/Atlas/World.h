#pragma once
#include "NonCopyable.h"
#include <memory>
#include "Atlas\SystemManager.h"
#include "Atlas\GameObjectManager.h"

namespace Atlas
{
	class FWorld : public NonCopyable
	{
	public:

		FWorld();
		FGameObjectManager& GetObjectManager() { return mObjectManager; }
		FSystemManager&     GetSystemManager() { return mSystemManager; }
		//FGroupManager&      GetGroupManager() { return mGroupManager; }

	private:
		FGameObjectManager mObjectManager;
		FSystemManager  mSystemManager;
		//FGroupManager      mGroupManager;
	};
}