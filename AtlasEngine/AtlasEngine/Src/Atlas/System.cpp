#include "Atlas/System.h"

namespace Atlas
{
	ISystem::ISystem(FWorld& World)
		: mWorld(World)
		, mTypeBitMask()
		, mSystemBitMask()
		, mGameObjectIDs()
	{
		////////////////////////////////////////////////////////////////////////////
		////// Call addComponentType() in derived classes //////////////////////////
		////////////////////////////////////////////////////////////////////////////
	}

	void ISystem::CheckInterest(FGameObject& GameObject)
	{
		// Check if GameObject is already in our system and if this system is
		// interested in processing it
		bool Contains = (GameObject.GetSystemBitMask() & mSystemBitMask) == mSystemBitMask;
		bool Interest = (GameObject.GetComponentBitMask() & mTypeBitMask) == mTypeBitMask;

		// It is not in the system, but we are interested
		if (!Contains && Interest && mTypeBitMask.any())
		{
			mGameObjectIDs.push_back(&GameObject);
			GameObject.SetSystemBit(mSystemBitMask);
			OnGameObjectAdd(GameObject);
		}
		// It is in the system, but we are not interested
		else if (Contains && !Interest && mTypeBitMask.any())
		{
			OnGameObjectRemove(GameObject);
			RemoveObject(GameObject);
		}

		for (auto& SubSystem : mSubSystems)
			SubSystem->CheckInterest(GameObject);
	}

	void ISystem::RemoveObject(FGameObject& GameObject)
	{
		GameObject.RemoveSystemBit(mSystemBitMask);
		mGameObjectIDs.erase(std::find(mGameObjectIDs.begin(), mGameObjectIDs.end(), &GameObject));
	}

	void ISystem::OnGameObjectAdd(FGameObject& GameObject)
	{

	}

	void ISystem::OnGameObjectRemove(FGameObject& GameObject)
	{

	}
}