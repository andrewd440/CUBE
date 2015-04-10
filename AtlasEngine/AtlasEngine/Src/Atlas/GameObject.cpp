#include "Atlas\GameObject.h"
#include "BulletPhysics\btBulletCollisionCommon.h"
#include "Atlas\Behavior.h"

namespace Atlas
{
	FGameObject::FGameObject(FGameObjectManager& GOManager, FChunkManager& ChunkManager)
		: mComponentBits()
		, mSystemBits()
		, mGOManager(GOManager)
		, mChunkManager(ChunkManager)
		, mBehaviors()
		, mID(0)
		, mIsActive(true)
	{
		for (uint32_t i = 0; i < EComponent::Count; i++)
			mComponents[i] = NULL_COMPONENT;
	}

	void FGameObject::getWorldTransform(btTransform& WorldTransform) const
	{
		WorldTransform.setFromOpenGLMatrix(*Transform.LocalToWorldMatrix().M);
	}

	void FGameObject::Update()
	{
		for (auto& Behavior : mBehaviors)
		{
			Behavior.second->Update();
		}
	}

	void FGameObject::OnStart()
	{
		for (auto& Behavior : mBehaviors)
		{
			Behavior.second->OnStart();
		}
	}

	FChunkManager& FGameObject::GetChunkManager()
	{
		return mChunkManager;
	}

	void FGameObject::setWorldTransform(const btTransform& WorldTransform)
	{
		const btVector3 Position = WorldTransform.getOrigin();
		Transform.SetPosition(Vector3f{ Position.m_floats[0], Position.m_floats[1], Position.m_floats[2] });

		const btQuaternion Rotation = WorldTransform.getRotation();
		Transform.SetRotation(FQuaternion{ Rotation.getW() , Rotation.getX(), Rotation.getY(), Rotation.getZ() });
	}
}