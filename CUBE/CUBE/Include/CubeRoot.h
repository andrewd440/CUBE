#pragma once

#include "Atlas\World.h"
#include "SFML\Window\Window.hpp"
#include "Math\Vector2.h"
#include <cstdint>

class Atlas::FGameObjectManager;
class FRenderSystem;
class FPhysicsSystem;
class FChunkManager;
class FAudioSystem;

/**
* Root/Central class for the game engine
*/
class FCubeRoot
{
public:
	FCubeRoot(const wchar_t* AppName, const Vector2ui Resolution, const uint32_t WindowStyle = sf::Style::Default);

	~FCubeRoot();

	// Disable copying of this object.
	FCubeRoot(const FCubeRoot& Other) = delete;
	FCubeRoot& operator=(const FCubeRoot& Other) = delete;

	void Start();

	FRenderSystem& GetRenderSystem(){ return *mRenderSystem; }
	FPhysicsSystem& GetPhysicsSystem() { return *mPhysicsSystem; }
	Atlas::FGameObjectManager& GetGameObjectManager() { return *mGameObjectManager; }
	FChunkManager& GetChunkManager() { return *mChunkManager; }

private:
	void AllocateSingletons();
	void LoadEngineSystems();
	void GameLoop();
	void ServiceEvents();

private:
	struct Systems
	{
		enum Index : uint32_t
		{
			Render,
			Physics,
			Audio,
			Count
		};
	};

private:
	sf::Window                  mGameWindow;
	Atlas::FWorld               mWorld;
	FChunkManager*              mChunkManager;
	FRenderSystem*              mRenderSystem;
	FPhysicsSystem*             mPhysicsSystem;
	FAudioSystem*               mAudioSystem;
	Atlas::FGameObjectManager*  mGameObjectManager;
};

