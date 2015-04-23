#include "VoxiGineRoot.h"
#include <cstdint>

#include <SFML\Window\Context.hpp>
#include <SFML\Window\Event.hpp>
#include <SFML\Window\VideoMode.hpp>
#include "Clock.h"
#include "STime.h"
#include "Input\ButtonEvent.h"
#include "Input\MouseAxis.h"

#include <GL/glew.h>
#include "SystemResources\SystemFile.h"
#include "Math\Vector3.h"
#include "Common.h"
#include "Math\Quaternion.h"
#include "ChunkSystems\ChunkManager.h"
#include "Rendering\Screen.h"
#include "Rendering\Camera.h"
#include "Rendering\RenderSystem.h"
#include "Rendering\Light.h"
#include "Components\Collider.h"
#include "Components\RigidBody.h"
#include "Physics\PhysicsSystem.h"
#include "Audio\AudioSystem.h"
#include "Debugging\DebugDraw.h"
#include "Debugging\DebugText.h"
#include "Input\TextEntered.h"
#include "Debugging\GameConsole.h"
#include "ResourceHolder.h"
#include "Components\ObjectMesh.h"
#include "Components\MeshRenderer.h"
#include "Components\SoundEmitter.h"
#include "Components\SoundListener.h"

using namespace Atlas;

FVoxiGineRoot::FVoxiGineRoot(const wchar_t* AppName, const Vector2ui Resolution, const uint32_t WindowStyle)
	: mGameWindow(sf::VideoMode{ Resolution.x, Resolution.y }, AppName, WindowStyle, sf::ContextSettings(32, 0, 0, 4, 3))
	, mWorld()
	, mChunkManager(nullptr)
	, mRenderSystem(nullptr)
	, mPhysicsSystem(nullptr)
	, mGameObjectManager(nullptr)
{
	if (glewInit())
	{
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
		exit(EXIT_FAILURE);
	}

	SMouseAxis::SetWindow(mGameWindow);
	SMouseAxis::UpdateDelta();
	SMouseAxis::UpdateDelta();
	
	AllocateSingletons();
	LoadEngineSystems();
}

void FVoxiGineRoot::AllocateSingletons()
{
	IFileSystem* FileSystem = new FFileSystem;
	FDebug::Text* DebugText = new FDebug::Text;
	FDebug::Draw* DebugDraw = new FDebug::Draw;
	FDebug::GameConsole*  GameConsole = new FDebug::GameConsole;
}

void FVoxiGineRoot::LoadEngineSystems()
{
	mChunkManager = new FChunkManager;

	// Load all subsystems
	FSystemManager& SystemManager = mWorld.GetSystemManager();
	mRenderSystem = &SystemManager.AddSystem<FRenderSystem>(mGameWindow, *mChunkManager);
	mPhysicsSystem = &SystemManager.AddSystem<FPhysicsSystem>();
	mAudioSystem = &SystemManager.AddSystem<FAudioSystem>();

	// Pass console dependencies
	FDebug::GameConsole& Console = FDebug::GameConsole::GetInstance();
	Console.SetChunkManager(mChunkManager);
	Console.SetPhysicsSystem(mPhysicsSystem);
	Console.SetRenderSystem(mRenderSystem);

	mChunkManager->SetPhysicsSystem(*mPhysicsSystem);
	mGameObjectManager = &mWorld.GetObjectManager();
	mGameObjectManager->SetChunkManager(mChunkManager);
	
	// Register all internal components
	mGameObjectManager->RegisterComponentType<EComponent::DirectionalLight>();
	mGameObjectManager->RegisterComponentType<EComponent::PointLight>();
	mGameObjectManager->RegisterComponentType<EComponent::Collider>();
	mGameObjectManager->RegisterComponentType<EComponent::RigidBody>();
	mGameObjectManager->RegisterComponentType<EComponent::MeshRenderer>();
	mGameObjectManager->RegisterComponentType<EComponent::SoundEmitter>();
	mGameObjectManager->RegisterComponentType<EComponent::SoundListener>();
}

FVoxiGineRoot::~FVoxiGineRoot()
{
	delete mChunkManager;
	delete FDebug::GameConsole::GetInstancePtr();
	delete FDebug::Draw::GetInstancePtr();
	delete FDebug::Text::GetInstancePtr();
	delete IFileSystem::GetInstancePtr();
}

void FVoxiGineRoot::Start()
{
	STime::StartGameTimer();
	mWorld.Start();
	GameLoop();
}

void FVoxiGineRoot::GameLoop()
{
	STime::SetFixedUpdate(1.0f / 60.0f);

	// Game Loop
	while (mGameWindow.isOpen())
	{	
		mGameObjectManager->Update();
		mChunkManager->Update();

		mPhysicsSystem->Update();
		mAudioSystem->Update();
		mRenderSystem->Update();

		STime::UpdateGameTimer();
		ServiceEvents();
	}
}

void FVoxiGineRoot::ServiceEvents()
{
	// Windows events
	sf::Event Event;

	// Reset button and axes from previous frame
	SButtonEvent::ResetButtonEvents();
	SMouseAxis::ResetAxes();
	SMouseAxis::UpdateDelta();
	STextEntered::Reset();

		
	// Service window events
	while (mGameWindow.pollEvent(Event))
	{
		if (Event.type == Event.Closed || SButtonEvent::GetKeyDown(sf::Keyboard::Escape))
			mGameWindow.close();

		if (SButtonEvent::IsButtonEvent(Event))
		{
			SButtonEvent::AddButtonEvent(Event);
		}
		else if (SMouseAxis::IsMouseAxisEvent(Event))
		{
			SMouseAxis::UpdateEvent(Event);
		}
		else if (Event.type == sf::Event::TextEntered)
		{
			STextEntered::AddTextEvent(Event);
		}
		else if (Event.type == sf::Event::Resized)
		{
			// adjust the viewport when the window is resized
			glViewport(0, 0, Event.size.width, Event.size.height);
			mRenderSystem->SetResolution(Vector2ui{Event.size.width, Event.size.height});
		}
	}
}