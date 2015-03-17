#include "Debugging\GameConsole.h"
#include "Input\ButtonEvent.h"
#include "Debugging\DebugText.h"
#include "Input\TextEntered.h"
#include "Physics\PhysicsSystem.h"
#include "Rendering\ChunkManager.h"
#include "Rendering\Screen.h"

namespace FDebug
{
	GameConsole::GameConsole()
		: mCommandBuffer()
		, mPhysicsSystem(nullptr)
		, mRenderSystem(nullptr)
		, mChunkManager(nullptr)
		, mIsActive(false)
		, mDrawPhysics(false)
	{
	}


	GameConsole::~GameConsole()
	{
	}


	void GameConsole::Render()
	{
		if (mDrawPhysics)
			mPhysicsSystem->RenderCollisionObjects();

		if (SButtonEvent::GetKeyDown(sf::Keyboard::Tilde))
		{
			mIsActive = !mIsActive;
			mCommandBuffer.clear();
			return;
		}

		if (!mIsActive)
			return;

		ProcessInput();

		FDebug::Text::GetInstance().AddText(mCommandBuffer.c_str(), FColor{ 1, 1, 1, 0.8f }, Vector2i{ 20, 20 });
	}

	void GameConsole::ProcessInput()
	{
		if (SButtonEvent::GetKeyDown(sf::Keyboard::Return))
		{
			ParseCommand();
			mCommandBuffer.clear();
			return;
		}

		if (SButtonEvent::GetKeyDown(sf::Keyboard::BackSpace) && !mCommandBuffer.empty())
		{
			mCommandBuffer.pop_back();
			return;
		}

		const std::wstring& TextEntered = STextEntered::GetTextInput();
		mCommandBuffer.insert(mCommandBuffer.end(), TextEntered.begin(), TextEntered.end());
	}

	void GameConsole::ParseCommand()
	{
		if (mPhysicsSystem && mCommandBuffer.substr(0, 11) == std::wstring{ L"DrawPhysics" })
		{
			if (mCommandBuffer.substr(12) == std::wstring{ L"true" })
				mDrawPhysics = true;
			else
				mDrawPhysics = false;
		}
		else if (mChunkManager && mCommandBuffer.substr(0, 9) == std::wstring{ L"LoadWorld" })
		{
			mChunkManager->LoadWorld(mCommandBuffer.substr(10).c_str());
		}
	}

	void GameConsole::SetPhysicsSystem(FPhysicsSystem* Physics)
	{
		mPhysicsSystem = Physics;
	}

	void GameConsole::SetRenderSystem(FRenderSystem* RenderSystem)
	{
		mRenderSystem = RenderSystem;
	}

	void GameConsole::SetChunkManager(FChunkManager* ChunkManager)
	{
		mChunkManager = ChunkManager;
	}
}