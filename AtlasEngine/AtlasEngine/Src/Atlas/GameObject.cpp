#include "Atlas\GameObject.h"
#include "Atlas\GameObjectManager.h"
#include "Atlas\Utilities.h"
#include "Atlas\ComponentHandleManager.h"

#include <iostream>
namespace Atlas
{
	FGameObject::FGameObject(ID id)
		: mGOManager()
		, mID(id)
		, mIsActive(false)
		, mComponentBits()
		, mSystemBits()
	{
	}

	std::vector<IComponent*> FGameObject::getAllComponents() const
	{
		return mFGameObjectManager.getAllComponentsFor(*this);
	}

	void FGameObject::addComponent(IComponent::Ptr component)
	{
		mFGameObjectManager.addComponent(*this, std::move(component));
	}

	void FGameObject::setActive(bool flag)
	{
		mIsActive = flag;
	}

	bool FGameObject::isActive() const
	{
		return mIsActive;
	}

	void FGameObject::setID(ID id)
	{
		mID = id;
	}

	FGameObject::ID FGameObject::GetID() const
	{
		return mID;
	}

	void FGameObject::reset()
	{
		mComponentBits = 0;
		mSystemBits = 0;
	}


	std::bitset<BITSIZE> FGameObject::getComponentBits() const
	{
		return mComponentBits;
	}

	void FGameObject::addComponentBit(const std::bitset<BITSIZE>& bit)
	{
		mComponentBits |= bit;
	}

	void FGameObject::removeComponentBit(const std::bitset<BITSIZE>& bit)
	{
		mComponentBits &= ~bit;
	}

	void FGameObject::setSystemBit(const std::bitset<BITSIZE>& bit)
	{
		mSystemBits |= bit;
	}

	void FGameObject::removeSystemBit(const std::bitset<BITSIZE>& bit)
	{
		mSystemBits &= ~bit;
	}

	std::bitset<BITSIZE> FGameObject::getSystemBits() const
	{
		return mSystemBits;
	}

	void FGameObject::toString() const
	{
		using namespace std;
		const auto& components = getAllComponents();

		cout << "ID: " << mID
			<< "\nComponentBits: " << mComponentBits.to_string()
			<< "\nComponents: ";

		for (const auto& c : components)
		{
			cout << typeidStringFormator(c) << ", ";
		}

		cout << "\nActive: " << mIsActive << endl << endl;
	}
}