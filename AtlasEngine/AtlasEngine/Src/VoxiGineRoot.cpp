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
#include "Rendering\ChunkManager.h"
#include "Rendering\Screen.h"
#include "Rendering\Camera.h"
#include "Rendering\RenderSystem.h"
#include "Rendering\Light.h"
#include "Components\Collider.h"
#include "Components\RigidBody.h"
#include "Physics\PhysicsSystem.h"
#include "Debugging\DebugDraw.h"
#include "Debugging\DebugText.h"
#include "Input\TextEntered.h"
#include "Components\MeshComponent.h"
#include "Components\FlyingCamera.h"

const uint32_t WindowWidth = 1920;
const uint32_t WindowHeight = 1080;

static FCamera MainCamera;

using namespace Atlas;

FVoxiGineRoot::FVoxiGineRoot()
	: mGameWindow(sf::VideoMode{ WindowWidth, WindowHeight }, L"VoxiGine", sf::Style::Default, sf::ContextSettings(24, 8, 2, 4, 3))
	, mWorld()
	, mChunkManager(nullptr)
{
	if (glewInit())
	{
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
		exit(EXIT_FAILURE);
	}

	IFileSystem* FileSystem = new FFileSystem;
	FileSystem->SetToProgramDirectory();

	mGameWindow.setMouseCursorVisible(false);
	SMouseAxis::SetDefaultMousePosition(Vector2i(WindowWidth / 2, WindowHeight / 2));
	
	SScreen::SetResolution(TVector2<uint32_t>(WindowWidth, WindowHeight));

	// Initialize debug text
	FDebug::Text* DebugText = new FDebug::Text;
	FDebug::Draw* DebugDraw = new FDebug::Draw;
	FDebug::GameConsole*  GameConsole = new FDebug::GameConsole;

	mChunkManager = new FChunkManager;
}

FVoxiGineRoot::~FVoxiGineRoot()
{
	delete mChunkManager;
	delete IFileSystem::GetInstancePtr();
	delete FDebug::Draw::GetInstancePtr();
	delete FDebug::Text::GetInstancePtr();
	delete FDebug::GameConsole::GetInstancePtr();
}

void FVoxiGineRoot::Start()
{
	FCamera::Main = &MainCamera;
	FTransform& CameraTransform = MainCamera.Transform;
	const Vector3f CameraPosition = Vector3f{ 50.0f, 210.0f, 50.0f };
	CameraTransform.SetPosition(CameraPosition);

	MainCamera.SetProjection(FPerspectiveMatrix{ (float)WindowWidth / (float)WindowHeight, 35.0f, 1.0f, 512.0f });

	// Load all subsystems
	FSystemManager& SystemManager = mWorld.GetSystemManager();
	FRenderSystem& Renderer = SystemManager.AddSystem<FRenderSystem>(mGameWindow, *mChunkManager);
	FPhysicsSystem& Physics = SystemManager.AddSystem<FPhysicsSystem>();

	FDebug::GameConsole& Console = FDebug::GameConsole::GetInstance();
	Console.SetChunkManager(mChunkManager);
	Console.SetPhysicsSystem(&Physics);
	Console.SetRenderSystem(&Renderer);

	mChunkManager->SetPhysicsSystem(Physics);
	mChunkManager->LoadWorld(L"LargeWorld");

	FGameObjectManager& GameObjectManager = mWorld.GetObjectManager();
	GameObjectManager.SetChunkManager(mChunkManager);

	GameObjectManager.RegisterComponentType<EComponent::DirectionalLight>();
	GameObjectManager.RegisterComponentType<EComponent::PointLight>();
	GameObjectManager.RegisterComponentType<EComponent::Collider>();
	GameObjectManager.RegisterComponentType<EComponent::RigidBody>();
	GameObjectManager.RegisterComponentType<EComponent::Mesh>();

	ConstructScene();

	GameObjectManager.Start();
	GameLoop();
}

void FVoxiGineRoot::ConstructScene()
{
	FGameObjectManager& GameObjectManager = mWorld.GetObjectManager();

	////////////////////////////////////////////////////////////////////////
	//////// Directional Light /////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	auto& DirectionalLight = GameObjectManager.CreateGameObject();
	FDirectionalLight& DLight = DirectionalLight.AddComponent<EComponent::DirectionalLight>();
	DLight.Color = Vector3f(.7, .7, .7);
	DirectionalLight.Transform.SetRotation(FQuaternion{ -40, 20, 0 });
	DirectionalLight.AddBehavior<CFlyingCamera>();

	auto& PointLight = GameObjectManager.CreateGameObject();
	//FPointLight& PLight = PointLight.AddComponent<EComponent::PointLight>();
	//PLight.Color = Vector3f(0.2f, .1f, .8f);
	//PLight.MinDistance = 1;
	//PLight.MaxDistance = 3;
	FMeshComponent& Mesh = PointLight.AddComponent<Atlas::EComponent::Mesh>();
	Mesh.LoadModel("Sword.obj");
	PointLight.Transform.SetPosition(Vector3f{ 250.0f, 220.0f, 250.0f });
	PointLight.Transform.Rotate(FQuaternion{ 90, 0, -45 });
}

void FVoxiGineRoot::GameLoop()
{
	STime::SetFixedUpdate(.02f);
	STime::SetDeltaTime(1.0f / 30.0f); // Default delta time with 30fps

	FSystemManager& SystemManager = mWorld.GetSystemManager();
	FGameObjectManager& GameObjectManager = mWorld.GetObjectManager();

	// Game Loop
	float lag = 0.0f;
	while (mGameWindow.isOpen())
	{	
		if (SButtonEvent::GetMouseDown(sf::Mouse::Right))
		{
			Vector3f CamForward = MainCamera.Transform.GetRotation() * -Vector3f::Forward * 4.0f;
			Vector3f CamPosition = MainCamera.Transform.GetPosition() + CamForward;
			Vector3i IntPosition{ (int32_t)CamPosition.x, (int32_t)CamPosition.y, (int32_t)CamPosition.z };
			mChunkManager->DestroyBlock(IntPosition);
		}
		if (SButtonEvent::GetMouseDown(sf::Mouse::Left))
		{
			Vector3f CamForward = MainCamera.Transform.GetRotation() * -Vector3f::Forward * 4.0f;
			Vector3f CamPosition = MainCamera.Transform.GetPosition() + CamForward;
			Vector3i IntPosition{ (int32_t)CamPosition.x, (int32_t)CamPosition.y, (int32_t)CamPosition.z };
			mChunkManager->SetBlock(IntPosition, FBlock::Brick);
		}

		mChunkManager->Update();
		GameObjectManager.Update();

		SystemManager.GetSystem(Systems::Physics)->Update();
		
		// Render the frame
		SystemManager.GetSystem(Systems::Render)->Update();

		UpdateTimers();
		ServiceEvents();
	}
}

void FVoxiGineRoot::ServiceEvents()
{
	// Windows events
	sf::Event Event;

	static bool ResetMouse = true;

	if (SButtonEvent::GetKeyDown(sf::Keyboard::L))
		ResetMouse = !ResetMouse;

	// Reset button and axes from previous frame
	SButtonEvent::ResetButtonEvents();
	SMouseAxis::ResetAxes();
	STextEntered::Reset();

	static sf::Vector2i DefaultMouse(SMouseAxis::GetDefaultMousePosition().x, SMouseAxis::GetDefaultMousePosition().y); 

	if (ResetMouse)
	{
		SMouseAxis::UpdateDelta(mGameWindow);
		sf::Mouse::setPosition(DefaultMouse, mGameWindow);
	}
		
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
		}
	}
}

void FVoxiGineRoot::UpdateTimers()
{
	static FClock GameTimer;
	static uint64_t FrameStart = FClock::ReadSystemTimer(), FrameEnd;

	// Manage frame timers
	FrameEnd = FClock::ReadSystemTimer();
	uint64_t FrameTime = FrameEnd - FrameStart;

	// Update the game clock with this frame's time and
	// compute the frame delta time by taking the game timer's time
	// before and after this update (GameTimer may be time scaled)
	const uint64_t PreUpdateTimer = GameTimer.GetCycles();
	GameTimer.Update(FClock::CyclesToSeconds(FrameTime));
	const uint64_t PostUpdateTimer = GameTimer.GetCycles();

	float DeltaTime = FClock::CyclesToSeconds(PostUpdateTimer - PreUpdateTimer);

	// If large delta time, we were probably in a breakpoint
	if (DeltaTime > 1.5f)
	{
		DeltaTime = 1.0f / 30.0f;
	}

	// Set delta time for this frame
	STime::SetDeltaTime(DeltaTime);
	FrameStart = FrameEnd;
}