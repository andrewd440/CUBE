#pragma once
#include "Atlas\Component.h"
#include "btBulletCollisionCommon.h"
#include "Atlas\ComponentTypes.h"
#include "Common.h"

/**
* Collision component for gameobjects that
* are static (does not move).
*/
WIN_ALIGN(16)
struct FCollider : public Atlas::IComponent
{
	FCollider(const btMotionState* StartState = nullptr)
		: BoxCollider(btVector3{})
		, CollisionObject()
	{
		CollisionObject.setCollisionShape(&BoxCollider);

		btTransform WorldTransform;
		StartState->getWorldTransform(WorldTransform);
		CollisionObject.setWorldTransform(WorldTransform);
	}

	btBoxShape        BoxCollider;
	btCollisionObject CollisionObject;
};

template <>
struct Atlas::ComponentTraits::Object<Atlas::EComponent::Collider>
{
	using Type = FCollider;
};