#include "Physics\PhysicsSystem.h"
#include "STime.h"
#include "Components\RigidBody.h"
#include "Components\Collider.h"

FPhysicsSystem::FPhysicsSystem(Atlas::FWorld& World)
	: ISystem(World)
	, mCollisionConfig()
	, mCollisionDispatcher(&mCollisionConfig)
	, mBroadPhase()
	, mConstraintSolver()
	, mDynamicsWorld(&mCollisionDispatcher, &mBroadPhase, &mConstraintSolver, &mCollisionConfig)
{
	mDynamicsWorld.setGravity(btVector3{ 0, -10, 0 });
	AddSubSystem<FRigidBodySystem>(mDynamicsWorld);
	AddSubSystem<FColliderSystem>(mDynamicsWorld);
}


FPhysicsSystem::~FPhysicsSystem()
{
}


void FPhysicsSystem::Update()
{
	mDynamicsWorld.stepSimulation(STime::GetDeltaTime());
}

void FPhysicsSystem::CheckInterest(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdatedComponent)
{
	// Only delegate to subsystems
	for (auto& SubSystem : GetSubSystems())
		SubSystem->CheckInterest(GameObject, UpdatedComponent);
}


void FPhysicsSystem::RemoveObject(Atlas::FGameObject& GameObject)
{
	// Not holding object IDs, so do nothing.
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