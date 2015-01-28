#pragma once
#include "Singleton.h"
#include "SFML\Window\Window.hpp"

/**
* Root/Central class for the game engine
*/
class FAtlasRoot : public TSingleton<FAtlasRoot>
{
public:
	FAtlasRoot();

	~FAtlasRoot();

	// Disable copying of this object.
	FAtlasRoot(const FAtlasRoot& Other) = delete;
	FAtlasRoot& operator=(const FAtlasRoot& Other) = delete;

	void Start();

	void GameLoop();

private:
	void ServiceEvents();

	void UpdateTimers();

private:
	sf::Window mGameWindow;
};

