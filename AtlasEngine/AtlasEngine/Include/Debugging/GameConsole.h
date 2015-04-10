#pragma once

#include <vector>
#include "Singleton.h"
#include "freetype-gl\markup.h"

class FPhysicsSystem;
class FRenderSystem;
class FChunkManager;

namespace FDebug
{
	/**
	* In-game console.
	* Commands:
	* DrawPhysics bool
	* LoadWorld string
	*/
	class GameConsole : public TSingleton<GameConsole>
	{
	public:
		GameConsole();
		~GameConsole();

		void Render();

		void SetPhysicsSystem(FPhysicsSystem* Physics);
		void SetRenderSystem(FRenderSystem* RenderSystem);
		void SetChunkManager(FChunkManager* ChunkManager);

	private:
		void ProcessInput();
		void ParseCommand();

	private:
		std::wstring        mCommandBuffer;
		markup_t            mTextMarkup;
		FPhysicsSystem*     mPhysicsSystem;
		FRenderSystem*      mRenderSystem;
		FChunkManager*      mChunkManager;
		bool                mDrawPhysics;
		bool                mIsActive;
	};
}