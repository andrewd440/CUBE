#pragma once
#include "Atlas\System.h"
#include "btBulletDynamicsCommon.h"

#include <queue>
#include <mutex>

class FPhysicsSystem : public Atlas::ISystem
{
public:
	FPhysicsSystem(Atlas::FWorld& World);
	~FPhysicsSystem();

	/**
	* Updates the physics system. This should be called each render loop.
	* The physics update will internally update the with a fixed time value
	* until the simulation is in its correct state.
	*/
	void Update() override;

	/**
	* Checks if the gameobject has a rigidbody or collider. If so,
	* it is added to the dynamics world.
	*/
	void CheckInterest(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent) override;

	/**
	* Renders collision object in the world.
	*/
	void RenderCollisionObjects();

	/**
	* Adds a collision object to the simulation.
	*/
	void AddCollider(btCollisionObject& CollisionObject);

	/**
	* Removes a collision object from the simulation.
	*/
	void RemoveCollider(btCollisionObject& CollisionObject);

	/**
	* Adds a rigidbody to the simulation.
	*/
	void AddRigidBody(btRigidBody& RigidBody);

	/**
	* Removes a rigidbody from the simulation.
	*/
	void RemoveRigidBody(btRigidBody& RigidBody);

	/**
	* Override for removing object from this system. Since this system
	* does not need to hold object IDs, this evaluates to nothing.
	*/
	void RemoveObject(Atlas::FGameObject& GameObject) override { GameObject; } // Suppress compiler warning

private:
	struct RigidBodyRecord
	{
		btRigidBody* RigidBody;
		bool         ToBeAdded;
	};

	struct ColliderRecord
	{
		btCollisionObject* Collider;
		bool               ToBeAdded;
	};

	std::mutex                  mRigidBodyMutex;
	std::queue<RigidBodyRecord> mRigidBodyQueue;

	std::mutex                  mColliderMutex;
	std::queue<ColliderRecord>  mColliderQueue;

private:
	btDefaultCollisionConfiguration      mCollisionConfig; 
	btCollisionDispatcher                mCollisionDispatcher;
	btDbvtBroadphase                     mBroadPhase;
	btSequentialImpulseConstraintSolver  mConstraintSolver;
	btDiscreteDynamicsWorld              mDynamicsWorld;
};

/**
* System for managing the addition and removal of 
* rigidbodies from the physics simulation.
*/
class FRigidBodySystem : public Atlas::ISystem
{
public:
	FRigidBodySystem(Atlas::FWorld& World, FPhysicsSystem& DynamicsWorld);
	~FRigidBodySystem();

	void Update() override {};

private:
	/**
	* Adds a rigidbody to the physics simulation.
	*/
	void OnGameObjectAdd(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent) override;

	/**
	* Removes a rigidbody from the physics simulation.
	*/
	void OnGameObjectRemove(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent) override;

private:
	FPhysicsSystem& mPhysicsSystem;
};


/**
* System for managing the addition and removal of
* collision objects from the physics simulation.
*/
class FColliderSystem : public Atlas::ISystem
{
public:
	FColliderSystem(Atlas::FWorld& World, FPhysicsSystem& DynamicsWorld);
	~FColliderSystem();

	void Update() override {};

private:
	/**
	* Adds a collision object to the physics simulation.
	*/
	void OnGameObjectAdd(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent) override;

	/**
	* Adds a collision object to the physics simulation.
	*/
	void OnGameObjectRemove(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent) override;

private:
	FPhysicsSystem& mPhysicsSystem;
};