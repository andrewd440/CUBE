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
		: BoxCollider(btVector3{0.5f, 0.5f, 0.5f})
		, Body(1.0f, MotionState, &BoxCollider)
	{
		btVector3 Inertia;
		BoxCollider.calculateLocalInertia(1.0f, Inertia);
		Inertia = btVector3{ 1, 1, 1 } / Inertia;
		Body.setInvInertiaDiagLocal(Inertia);
	}

	btBoxShape    BoxCollider;
	btRigidBody   Body;
};

template <>
struct Atlas::ComponentTraits::Object<Atlas::EComponent::RigidBody>
{
	using Type = FRigidBody;
};