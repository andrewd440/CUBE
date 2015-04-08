#include "Physics\PhysicsSystem.h"
#include "STime.h"
#include "Components\RigidBody.h"
#include "Components\Collider.h"
#include "Debugging\DebugDraw.h"
#include "SFML\Window\Keyboard.hpp"

FPhysicsSystem::FPhysicsSystem(Atlas::FWorld& World)
	: ISystem(World)
	, mRigidBodyMutex()
	, mRigidBodyQueue()
	, mColliderMutex()
	, mColliderQueue()
	, mCollisionConfig()
	, mCollisionDispatcher(&mCollisionConfig)
	, mBroadPhase()
	, mConstraintSolver()
	, mDynamicsWorld(&mCollisionDispatcher, &mBroadPhase, &mConstraintSolver, &mCollisionConfig)
{
	mDynamicsWorld.setGravity(btVector3{ 0, -10, 0 });
	mDynamicsWorld.setDebugDrawer(FDebug::Draw::GetInstancePtr());

	AddSubSystem<FRigidBodySystem>(*this);
	AddSubSystem<FColliderSystem>(*this);
}


FPhysicsSystem::~FPhysicsSystem()
{
}


void FPhysicsSystem::Update()
{
	std::unique_lock<std::mutex> RigidLock(mRigidBodyMutex);
	while (!mRigidBodyQueue.empty())
	{
		RigidBodyRecord Record = mRigidBodyQueue.front();
		mRigidBodyQueue.pop();
		RigidLock.unlock();

		if (Record.ToBeAdded)
		{
			mDynamicsWorld.addRigidBody(Record.RigidBody);
		}
		else
		{
			mDynamicsWorld.removeRigidBody(Record.RigidBody);
		}

		RigidLock.lock();
	}

	RigidLock.unlock();

	std::unique_lock<std::mutex> ColliderLock(mColliderMutex);
	while (!mColliderQueue.empty())
	{
		ColliderRecord Record = mColliderQueue.front();
		mColliderQueue.pop();
		ColliderLock.unlock();

		if (Record.ToBeAdded)
		{
			mDynamicsWorld.addCollisionObject(Record.Collider);
		}
		else
		{
			mDynamicsWorld.removeCollisionObject(Record.Collider);
		}

		ColliderLock.lock();
	}

	ColliderLock.unlock();
	
	mDynamicsWorld.stepSimulation(STime::GetDeltaTime());
}

void FPhysicsSystem::RenderCollisionObjects()
{
	mDynamicsWorld.debugDrawWorld();
}

void FPhysicsSystem::AddCollider(btCollisionObject& CollisionObject)
{
	std::unique_lock<std::mutex> ColliderLock(mColliderMutex);
	mColliderQueue.push(ColliderRecord{ &CollisionObject, true });
}

void FPhysicsSystem::RemoveCollider(btCollisionObject& CollisionObject)
{
	std::unique_lock<std::mutex> ColliderLock(mColliderMutex);
	mColliderQueue.push(ColliderRecord{ &CollisionObject, false });
}

void FPhysicsSystem::AddRigidBody(btRigidBody& RigidBody)
{
	std::unique_lock<std::mutex> RigidLock(mRigidBodyMutex);
	mRigidBodyQueue.push(RigidBodyRecord{ &RigidBody, true });
}

void FPhysicsSystem::RemoveRigidBody(btRigidBody& RigidBody)
{
	std::unique_lock<std::mutex> RigidLock(mRigidBodyMutex);
	mRigidBodyQueue.push(RigidBodyRecord{ &RigidBody, false });
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

FRigidBodySystem::FRigidBodySystem(Atlas::FWorld& World, FPhysicsSystem& DynamicsWorld)
	: ISystem(World)
	, mPhysicsSystem(DynamicsWorld)
{
	AddComponentType<Atlas::EComponent::RigidBody>();
}

FRigidBodySystem::~FRigidBodySystem()
{

}

void FRigidBodySystem::OnGameObjectAdd(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent)
{
    FRigidBody* RigidBody = static_cast<FRigidBody*>(&UpdateComponent);
	mPhysicsSystem.AddRigidBody(RigidBody->Body);

	GameObject; 	// Suppress compiler warning
}

void FRigidBodySystem::OnGameObjectRemove(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent)
{
	FRigidBody* RigidBody = static_cast<FRigidBody*>(&UpdateComponent);
	mPhysicsSystem.RemoveRigidBody(RigidBody->Body);

	GameObject; 	// Suppress compiler warning
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////// Collider System ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

FColliderSystem::FColliderSystem(Atlas::FWorld& World, FPhysicsSystem& DynamicsWorld)
	: ISystem(World)
	, mPhysicsSystem(DynamicsWorld)
{
	AddComponentType<Atlas::EComponent::Collider>();
}

FColliderSystem::~FColliderSystem()
{

}

void FColliderSystem::OnGameObjectAdd(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent)
{
	FCollider* Collider = static_cast<FCollider*>(&UpdateComponent);
	mPhysicsSystem.RemoveCollider(Collider->CollisionObject);

	GameObject; 	// Suppress compiler warning
}

void FColliderSystem::OnGameObjectRemove(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent)
{
	FCollider* Collider = static_cast<FCollider*>(&UpdateComponent);
	mPhysicsSystem.RemoveCollider(Collider->CollisionObject);

	GameObject; 	// Suppress compiler warning
}