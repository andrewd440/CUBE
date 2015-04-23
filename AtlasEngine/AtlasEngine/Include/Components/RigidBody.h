#pragma once
#include "Atlas\Component.h"
#include "BulletPhysics\btBulletCollisionCommon.h"
#include "BulletPhysics\btBulletDynamicsCommon.h"
#include "Math\Vector3.h"
#include "Atlas\ComponentTypes.h"
#include "Common.h"

/**
* Collision/Physics component for gameobjects that
* are dynamic (moves within the scene).
*/
WIN_ALIGN(16)
struct FRigidBody : public Atlas::IComponent
{
	FRigidBody(btMotionState* MotionState = nullptr)
		: CapsuleCollider(.5f, 2)
		, Body(1.0f, MotionState, &CapsuleCollider)
	{
		btVector3 Inertia;
		CapsuleCollider.calculateLocalInertia(1.0f, Inertia);
		Inertia = btVector3{ 1, 1, 1 } / Inertia;
		Body.setInvInertiaDiagLocal(Inertia);
	}

	btCapsuleShape  CapsuleCollider;
	btRigidBody   Body;
};

template <>
struct Atlas::ComponentTraits::Object<Atlas::EComponent::RigidBody>
{
	using Type = FRigidBody;
};