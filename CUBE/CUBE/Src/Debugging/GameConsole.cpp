#include "Debugging\GameConsole.h"
#include "Input\ButtonEvent.h"
#include "Debugging\DebugText.h"
#include "Input\TextEntered.h"
#include "Physics\PhysicsSystem.h"
#include "ChunkSystems\ChunkManager.h"
#include "Rendering\Screen.h"
#include "Rendering\Camera.h"
#include "STime.h"

namespace FDebug
{
	GameConsole::GameConsole()
		: mCommandBuffer()
		, mTextMarkup()
		, mPhysicsSystem(nullptr)
		, mRenderSystem(nullptr)
		, mChunkManager(nullptr)
		, mIsActive(false)
		, mDrawPhysics(false)
	{
		const vec4 White{ { 1, 1, 1, 1 } };
		const vec4 Background{ { 0.3f, 0.3f, 0.3f, 0.8f } };
		mTextMarkup.family = "VeraMono.ttf";
		mTextMarkup.size = 16.0f;
		mTextMarkup.bold = 0;
		mTextMarkup.italic = 0;
		mTextMarkup.rise = 0.0f;
		mTextMarkup.spacing = 0.0f;
		mTextMarkup.gamma = 2.0f;
		mTextMarkup.foreground_color = White;
		mTextMarkup.background_color = Background;
		mTextMarkup.underline = 1;
		mTextMarkup.overline = 0;
		mTextMarkup.strikethrough = 0;
		mTextMarkup.font = 0;
	}


	GameConsole::~GameConsole()
	{
	}


	void GameConsole::Render()
	{
		/////////////////////////////
		// Debugging text ////////////////////////////

		static const vec4 White{ { 1, 1, 1, 1 } };
		static const vec4 None{ { 1, 1, 1, 0 } };
		static markup_t TextMarkup{ "Vera.ttf", 16, 0, 0, 0.0f, 0.0f, 2.0f, White, None, 0, White, 0, White, 0, White, 0 };

		// Debug Print
		const Vector3f CameraPosition = FCamera::Main->Transform.GetWorldPosition();
		auto& DebugText = FDebug::Text::GetInstance();
		wchar_t String[250];
		const Vector3f Direction = FCamera::Main->Transform.GetRotation() * -Vector3f::Forward;
		swprintf_s(String, L"FPS: %.0f   Position: %.1f %.1f %.1f Direction: %.1f %.1f %.1f", 1.0f / STime::GetDeltaTime(), CameraPosition.x, CameraPosition.y, CameraPosition.z, Direction.x, Direction.y, Direction.z);
		DebugText.AddText(std::wstring{ String }, Vector2i(50, SScreen::GetResolution().y - 50), TextMarkup);

		swprintf_s(String, L"+");
		DebugText.AddText(std::wstring{ String }, SScreen::GetResolution() / 2, TextMarkup);

		swprintf_s(String, L"Chunks used: %d", FChunk::ChunkAllocator.Size());
		DebugText.AddText(std::wstring{ String }, Vector2i(50, SScreen::GetResolution().y - 100), TextMarkup);

		Vector3i ChunkPosition = Vector3i(CameraPosition.x / FChunk::CHUNK_SIZE, CameraPosition.y / FChunk::CHUNK_SIZE, CameraPosition.z / FChunk::CHUNK_SIZE);
		swprintf_s(String, L"Chunk Position: %d %d %d", ChunkPosition.x, ChunkPosition.y, ChunkPosition.z);
		DebugText.AddText(std::wstring{ String }, Vector2i(50, SScreen::GetResolution().y - 150), TextMarkup);

		///////////////////////////////////////////////
		///////////////////////////////

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

		if (!mCommandBuffer.empty())
		{
			FDebug::Text::GetInstance().AddText(mCommandBuffer, Vector2i{ 40, (int32_t)SScreen::GetResolution().y - 30 }, mTextMarkup);
		}
	}

	void GameConsole::ProcessInput()
	{
		if (SButtonEvent::GetKeyDown(sf::Keyboard::Return))
		{
			ParseCommand();
			mCommandBuffer.clear();
			return;
		}

		if (SButtonEvent::GetKeyDown(sf::Keyboard::BackSpace))
		{
			if (!mCommandBuffer.empty())
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
		else if (mChunkManager && mCommandBuffer.substr(0, 15) == std::wstring{ L"SetViewDistance" })
		{
			std::wstring Distance = mCommandBuffer.substr(16, 18);
			mChunkManager->SetViewDistance((int32_t)std::stoi(Distance));
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