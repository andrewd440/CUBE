#pragma once
#include "Atlas\System.h"
#include "btBulletDynamicsCommon.h"

class FPhysicsSystem : public Atlas::ISystem
{
public:
	FPhysicsSystem(Atlas::FWorld& World);
	~FPhysicsSystem();

	void Update() override;

	/**
	* Checks if the gameobject has a rigidbody or collider. If so,
	* it is added to the dynamics world.
	*/
	void CheckInterest(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent) override;

private:
	void RemoveObject(Atlas::FGameObject& GameObject) override;

private:
	btDefaultCollisionConfiguration      mCollisionConfig; 
	btCollisionDispatcher                mCollisionDispatcher;
	btDbvtBroadphase                     mBroadPhase;
	btSequentialImpulseConstraintSolver  mConstraintSolver;
	btDiscreteDynamicsWorld              mDynamicsWorld;
};

class FRigidBodySystem : public Atlas::ISystem
{
public:
	FRigidBodySystem(Atlas::FWorld& World, btDynamicsWorld& DynamicsWorld);
	~FRigidBodySystem();

	void Update() override {};

private:
	void OnGameObjectAdd(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent) override;

	void OnGameObjectRemove(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent) override;

private:
	btDynamicsWorld& mDynamicsWorld;
};

class FColliderSystem : public Atlas::ISystem
{
public:
	FColliderSystem(Atlas::FWorld& World, btDynamicsWorld& DynamicsWorld);
	~FColliderSystem();

	void Update() override {};

private:
	void OnGameObjectAdd(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent) override;

	void OnGameObjectRemove(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent) override;

private:
	btDynamicsWorld& mDynamicsWorld;
};