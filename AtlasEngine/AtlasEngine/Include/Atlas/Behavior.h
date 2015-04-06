#pragma once

#include "Component.h"
#include "Rendering\Block.h"

class FChunkManager;

namespace Atlas
{
	class FBehavior : public IComponent
	{
		friend class FGameObject;
	public:
		FBehavior() : mGameObject{ nullptr }{}
		~FBehavior(){}

		virtual void OnStart(){};
		virtual void Update(){};
		virtual void OnCollisionEnter(){};
		virtual void OnCollisionExit(){};

		void SetBlock(const Vector3i& Position, const FBlock::BlockType Block);
		FBlock::BlockType GetBlock(const Vector3i& Position);
		void DestroyBlock(const Vector3i& Position);
		void LoadWorld(const wchar_t* WorldName);

		FGameObject* GetGameObject() { return mGameObject; }

		//void Destroy();
		void DestroyGameObject();
		FGameObject& CreateGameObject();

	private:
		void SetGameObject(FGameObject* GameObject){ mGameObject = GameObject; }
		void SetChunkManager(FChunkManager* ChunkManager){ mChunkManager = ChunkManager; }

	private:
		FGameObject* mGameObject;
		FChunkManager* mChunkManager;
	};
}