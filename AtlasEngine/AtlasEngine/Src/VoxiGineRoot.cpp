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
#include "Debugging\DebugDraw.h"
#include "Debugging\DebugText.h"
#include "Input\TextEntered.h"
#include "Components\MeshComponent.h"
#include "Components\FlyingCamera.h"
#include "Components\BlockPlacer.h"
#include "Components\TimeBombShooter.h"
#include "Components\BoxShooter.h"
#include "Rendering\FogPostProcess.h"
#include "Debugging\GameConsole.h"
#include "Rendering\SSAOPostProcess.h"

const uint32_t WindowWidth = 1920;
const uint32_t WindowHeight = 1080;

static FCamera MainCamera;

using namespace Atlas;

FVoxiGineRoot::FVoxiGineRoot()
	: mGameWindow(sf::VideoMode{ WindowWidth, WindowHeight }, L"VoxiGine", sf::Style::Default, sf::ContextSettings(24, 8, 2, 4, 3))
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
	SMouseAxis::SetDefaultMousePosition(Vector2i(WindowWidth / 2, WindowHeight / 2));
	SMouseAxis::SetMouseVisible(false);
	SMouseAxis::SetMouseLock(true);
	SMouseAxis::UpdateDelta();
	SMouseAxis::UpdateDelta();
	SScreen::SetResolution(TVector2<uint32_t>(WindowWidth, WindowHeight));
	
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
	mGameObjectManager->RegisterComponentType<EComponent::Mesh>();
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
	FCamera::Main = &MainCamera;
	const Vector3f CameraPosition = Vector3f{ 260.0f, 260.0f, 260.0f };
	MainCamera.Transform.SetPosition(CameraPosition);
	MainCamera.SetProjection(FPerspectiveMatrix{ (float)WindowWidth / (float)WindowHeight, 35.0f, 0.1f, 456.0f });

	std::unique_ptr<FSSAOPostProcess> SSAOPostProcess{ new FSSAOPostProcess{} };
	mRenderSystem->AddPostProcess(std::move(SSAOPostProcess));
	mRenderSystem->EnablePostProcess(0);

	std::unique_ptr<FFogPostProcess> FogPostProcess{ new FFogPostProcess{} };
	FogPostProcess->SetBounds(0, 1);
	FogPostProcess->SetColor(Vector3f{ .5f, .5f, .5f });
	FogPostProcess->SetDensity(0.00002f);

	mRenderSystem->AddPostProcess(std::move(FogPostProcess));
	mRenderSystem->EnablePostProcess(1);

	mChunkManager->LoadWorld(L"PrettyWorld");

	ConstructScene();
	STime::StartGameTimer();
	mGameObjectManager->Start();
	GameLoop();
}

void FVoxiGineRoot::ConstructScene()
{
	FGameObjectManager& GameObjectManager = *mGameObjectManager;

	auto& PlayerController = GameObjectManager.CreateGameObject();
	PlayerController.AddBehavior<CFlyingCamera>();
	PlayerController.AddBehavior<CBlockPlacer>();
	PlayerController.AddBehavior<CTimeBombShooter>();
	//PlayerController.AddBehavior<CBoxShooter>();

	////////////////////////////////////////////////////////////////////////
	//////// Directional Light /////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	auto& DirectionalLight = GameObjectManager.CreateGameObject();
	FDirectionalLight& DLight = DirectionalLight.AddComponent<EComponent::DirectionalLight>();
	DLight.Color = Vector3f(.7, .7, .7);
	DirectionalLight.Transform.SetRotation(FQuaternion{ -130, -20, 0 });

	
	//auto& PointLight = GameObjectManager.CreateGameObject();
	//PointLight.Transform.SetPosition(Vector3f{ 3 * i, 40, 3 * i });
	//FPointLight& PLight = PointLight.AddComponent<EComponent::PointLight>();
	//PLight.Color = Vector3f(0.05f * i, .1f, .5f);
	//PLight.MinDistance = 1;
	//PLight.MaxDistance = 10;

	//auto& Sword = GameObjectManager.CreateGameObject();
	//FMeshComponent& Mesh = Sword.AddComponent<Atlas::EComponent::Mesh>();
	//Mesh.LoadModel("Sword.obj");
	//Sword.Transform.SetPosition(Vector3f{ 100.0f, 230.0f, 100.0f });
	//Sword.Transform.Rotate(FQuaternion{ 90, 0, -45 });
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
		mRenderSystem->Update();

		STime::UpdateGameTimer();
		ServiceEvents();
	}
}

void FVoxiGineRoot::ServiceEvents()
{
	// Windows events
	sf::Event Event;

	static bool ResetMouse = false;

	if (SButtonEvent::GetKeyDown(sf::Keyboard::L))
	{
		ResetMouse = !ResetMouse;
		SMouseAxis::SetMouseLock(ResetMouse);
	}

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
		}
	}
}