#pragma once

#include "Component.h"
#include "ChunkSystems\BlockTypes.h"
#include "Atlas\GameObject.h"
#include "ChunkSystems\ChunkManager.h"

namespace Atlas
{
	/**
	* Base for all updatable components that can
	* be attached to gameobjects.
	*/
	class FBehavior : public IComponent
	{
		friend class FGameObject;
	public:
		FBehavior() : mGameObject{ nullptr }{}
		~FBehavior(){}

		/**
		* This function is called when the
		* game is first started or this component is
		* initially added to a gameobject.
		*/
		virtual void OnStart(){};

		/**
		* Called once per frame.
		*/
		virtual void Update(){};

		//virtual void OnCollisionEnter(){};
		//virtual void OnCollisionExit(){};
		
		/**
		* Sets a block type within the world at a specific world position.
		*/
		void SetBlock(const Vector3i& Position, const FBlockTypes::BlockID ID);

		/**
		* Gets the type of block that is at a specific world position.
		*/
		FBlockTypes::BlockID GetBlock(const Vector3i& Position);

		/**
		* Destroys a block at a specific position within the world.
		*/
		void DestroyBlock(const Vector3i& Position);

		template <typename T, void(T::*Function)(Vector3i, FBlockTypes::BlockID)>
		/**
		* Adds a function to call when a block is set within the world. A single instance
		* should not have more that one listener for a single event. Remember to
		* remove this listener before the object is destroyed.
		*/
		void AddOnBlockSetListener(T* Instance);

		template <typename T>
		/**
		* Removes the listener from the OnBlockSet event. 
		*/
		void RemoveOnBlockSetListener(T* Instance);

		template <typename T, void(T::*Function)(Vector3i, FBlockTypes::BlockID)>
		/**
		* Adds a function to call when a block is destroyed within the world. A single instance
		* should not have more that one listener for a single event. Remember to
		* remove this listener before the object is destroyed.
		*/
		void AddOnBlockDestroyListener(T* Instance);

		template <typename T>
		/**
		* Removes the listener from the OnBlockDestroy event.
		*/
		void RemoveOnBlockDestroyListener(T* Instance);

		/**
		* Loads a world.
		* @param WorldName - The name of the world directory.
		*/
		void LoadWorld(const wchar_t* WorldName);

		/**
		* Saves the currently loaded world.
		*/
		void SaveWorld();

		/**
		* Gets the gameobject this component is attached to.
		*/
		FGameObject* GetGameObject() { return mGameObject; }

		/**
		* Destroys the gameobject this component is attached to. This
		* will also destroy all other components attached to this object.
		*/
		void DestroyGameObject();

		/**
		* Creates an empty gameobject.
		*/
		FGameObject& CreateGameObject();

	private:
		void SetGameObject(FGameObject* GameObject){ mGameObject = GameObject; }
		void SetChunkManager(FChunkManager* ChunkManager){ mChunkManager = ChunkManager; }

	private:
		FGameObject* mGameObject;
		FChunkManager* mChunkManager;
	};

	// Inlines for all pass-through functions

	inline void FBehavior::SetBlock(const Vector3i& Position, const FBlockTypes::BlockID ID)
	{
		mChunkManager->SetBlock(Position, ID);
	}

	inline FBlockTypes::BlockID FBehavior::GetBlock(const Vector3i& Position)
	{
		return mChunkManager->GetBlock(Position);
	}

	inline void FBehavior::DestroyBlock(const Vector3i& Position)
	{
		mChunkManager->DestroyBlock(Position);
	}

	template <typename T, void(T::*Function)(Vector3i, FBlockTypes::BlockID)>
	inline void FBehavior::AddOnBlockSetListener(T* Instance)
	{
		mChunkManager->mOnBlockSet.AddListener<T, Function>(Instance);
	}


	template <typename T>
	inline void FBehavior::RemoveOnBlockSetListener(T* Instance)
	{
		mChunkManager->mOnBlockSet.RemoveListener(Instance);
	}

	template <typename T, void(T::*Function)(Vector3i, FBlockTypes::BlockID)>
	inline void FBehavior::AddOnBlockDestroyListener(T* Instance)
	{
		mChunkManager->mOnBlockDestroy.AddListener<T, Function>(Instance);
	}

	template <typename T>
	inline void FBehavior::RemoveOnBlockDestroyListener(T* Instance)
	{
		mChunkManager->mOnBlockDestroy.RemoveListener(Instance);
	}

	inline void FBehavior::LoadWorld(const wchar_t* WorldName)
	{
		mChunkManager->LoadWorld(WorldName);
	}

	inline void FBehavior::SaveWorld()
	{
		mChunkManager->SaveWorld();
	}

	inline FGameObject& FBehavior::CreateGameObject()
	{
		return mGameObject->CreateGameObject();
	}

	inline void FBehavior::DestroyGameObject()
	{
		mGameObject->Destroy();
	}
}