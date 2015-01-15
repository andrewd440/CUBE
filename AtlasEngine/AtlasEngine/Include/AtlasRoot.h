#pragma once
#include "Singleton.h"
#include "Window.h"

/**
* Root/Central class for the game engine
*/
class FAtlasRoot : public TSingleton<FAtlasRoot>
{
public:
	FAtlasRoot();

	~FAtlasRoot();

	void Start();

	void GameLoop();

private:
	void ServiceEvents();

private:
	FWindow mGameWindow;
};

