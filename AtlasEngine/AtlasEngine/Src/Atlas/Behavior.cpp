#include "Atlas\Behavior.h"
#include "Atlas\GameObject.h"
#include "ChunkSystems\ChunkManager.h"

namespace Atlas
{
	//void FBehavior::Destroy()
	//{
	//	mGameObject->RemoveBehavior<typeid(this)>();
	//}

	void FBehavior::SetBlock(const Vector3i& Position, const FBlock::Type Block)
	{
		mChunkManager->SetBlock(Position, Block);
	}

	FBlock::Type FBehavior::GetBlock(const Vector3i& Position)
	{
		return mChunkManager->GetBlock(Position);
	}

	void FBehavior::DestroyBlock(const Vector3i& Position)
	{
		mChunkManager->DestroyBlock(Position);
	}

	void FBehavior::LoadWorld(const wchar_t* WorldName)
	{
		mChunkManager->LoadWorld(WorldName);
	}


	FGameObject& FBehavior::CreateGameObject()
	{
		return mGameObject->CreateGameObject();
	}

	void FBehavior::DestroyGameObject()
	{
		mGameObject->Destroy();
	}
}