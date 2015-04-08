#pragma once

#include "Atlas\World.h"
#include "SFML\Window\Window.hpp"

class Atlas::FGameObjectManager;
class FRenderSystem;
class FPhysicsSystem;
class FChunkManager;

/**
* Root/Central class for the game engine
*/
class FVoxiGineRoot
{
public:
	FVoxiGineRoot();

	~FVoxiGineRoot();

	// Disable copying of this object.
	FVoxiGineRoot(const FVoxiGineRoot& Other) = delete;
	FVoxiGineRoot& operator=(const FVoxiGineRoot& Other) = delete;

	void Start();

	FRenderSystem& GetRenderSystem();
	FPhysicsSystem& GetPhysicsSystem();
	Atlas::FGameObjectManager& GetGameObjectManager();
	FChunkManager& GetChunkManager();

private:
	void AllocateSingletons();
	void LoadEngineSystems();
	void GameLoop();
	void ServiceEvents();
	void UpdateTimers();
	void ConstructScene();

private:
	struct Systems
	{
		enum Index : uint32_t
		{
			Render,
			Physics,
			Count
		};
	};

private:
	sf::Window mGameWindow;
	Atlas::FWorld mWorld;
	FChunkManager* mChunkManager;
};

