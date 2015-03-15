#include "Physics\PhysicsSystem.h"
#include "STime.h"
#include "Components\RigidBody.h"
#include "Components\Collider.h"
#include "Debugging\DebugDraw.h"
#include "SFML\Window\Keyboard.hpp"

FPhysicsSystem::FPhysicsSystem(Atlas::FWorld& World)
	: ISystem(World)
	, mCollisionConfig()
	, mCollisionDispatcher(&mCollisionConfig)
	, mBroadPhase()
	, mConstraintSolver()
	, mDynamicsWorld(&mCollisionDispatcher, &mBroadPhase, &mConstraintSolver, &mCollisionConfig)
{
	mDynamicsWorld.setGravity(btVector3{ 0, -10, 0 });
	mDynamicsWorld.setDebugDrawer(FDebug::Draw::GetInstancePtr());

	AddSubSystem<FRigidBodySystem>(mDynamicsWorld);
	AddSubSystem<FColliderSystem>(mDynamicsWorld);
}


FPhysicsSystem::~FPhysicsSystem()
{
}


void FPhysicsSystem::Update()
{
	mDynamicsWorld.stepSimulation(STime::GetDeltaTime());

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::U))
		RenderCollisionObjects();
}

void FPhysicsSystem::RenderCollisionObjects()
{
	mDynamicsWorld.debugDrawWorld();
}

void FPhysicsSystem::AddCollider(btCollisionObject& CollisionObject)
{
	mDynamicsWorld.addCollisionObject(&CollisionObject);
}

void FPhysicsSystem::RemoveCollider(btCollisionObject& CollisionObject)
{
	mDynamicsWorld.removeCollisionObject(&CollisionObject);
}

void FPhysicsSystem::AddRigidBody(btRigidBody& RigidBody)
{
	mDynamicsWorld.addRigidBody(&RigidBody);
}

void FPhysicsSystem::RemoveRigidBody(btRigidBody& RigidBody)
{
	mDynamicsWorld.removeRigidBody(&RigidBody);
}

void FPhysicsSystem::CheckInterest(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdatedComponent)
{
	// Only delegate to subsystems
	for (auto& SubSystem : GetSubSystems())
		SubSystem->CheckInterest(GameObject, UpdatedComponent);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////// Rigidbody System //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

FRigidBodySystem::FRigidBodySystem(Atlas::FWorld& World, btDynamicsWorld& DynamicsWorld)
	: ISystem(World)
	, mDynamicsWorld(DynamicsWorld)
{
	AddComponentType<Atlas::EComponent::RigidBody>();
}

FRigidBodySystem::~FRigidBodySystem()
{

}

void FRigidBodySystem::OnGameObjectAdd(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent)
{
    FRigidBody* RigidBody = static_cast<FRigidBody*>(&UpdateComponent);
	mDynamicsWorld.addRigidBody(&RigidBody->Body);
}

void FRigidBodySystem::OnGameObjectRemove(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent)
{
	FRigidBody* RigidBody = static_cast<FRigidBody*>(&UpdateComponent);
	mDynamicsWorld.removeRigidBody(&RigidBody->Body);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////// Collider System ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

FColliderSystem::FColliderSystem(Atlas::FWorld& World, btDynamicsWorld& DynamicsWorld)
	: ISystem(World)
	, mDynamicsWorld(DynamicsWorld)
{
	AddComponentType<Atlas::EComponent::Collider>();
}

FColliderSystem::~FColliderSystem()
{

}

void FColliderSystem::OnGameObjectAdd(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent)
{
	FCollider* Collider = static_cast<FCollider*>(&UpdateComponent);
	mDynamicsWorld.addCollisionObject(&Collider->CollisionObject);
}

void FColliderSystem::OnGameObjectRemove(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent)
{
	FCollider* Collider = static_cast<FCollider*>(&UpdateComponent);
	mDynamicsWorld.removeCollisionObject(&Collider->CollisionObject);
}