#include "Atlas\Behavior.h"
#include "Atlas\GameObject.h"
#include "Rendering\ChunkManager.h"

namespace Atlas
{
	//void FBehavior::Destroy()
	//{
	//	mGameObject->RemoveBehavior<typeid(this)>();
	//}

	void FBehavior::SetBlock(const Vector3i& Position, const FBlock::BlockType Block)
	{
		mChunkManager->SetBlock(Position, Block);
	}

	FBlock::BlockType FBehavior::GetBlock(const Vector3i& Position)
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

	void FBehavior::DestroyGameObject()
	{
		mGameObject->Destroy();
	}
}