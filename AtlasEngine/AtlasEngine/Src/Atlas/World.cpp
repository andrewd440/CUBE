#include "Atlas\World.h"


namespace Atlas
{
	FWorld::FWorld()
		: mObjectManager(*this)
		, mSystemManager(*this)
		//, mGroupManager(new FGroupManager(*this))
	{
	}
}