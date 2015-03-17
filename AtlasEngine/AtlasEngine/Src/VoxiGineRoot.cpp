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

const uint32_t WindowWidth = 1800;
const uint32_t WindowHeight = 1100;

static FCamera MainCamera;

using namespace Atlas;

FVoxiGineRoot::FVoxiGineRoot()
	: mGameWindow(sf::VideoMode{ WindowWidth, WindowHeight }, L"VoxiGine", sf::Style::Default, sf::ContextSettings(24, 8, 2, 4, 3))
	, mWorld()
	, mChunkManager()
{
	IFileSystem* FileSystem = new FFileSystem;

	mGameWindow.setMouseCursorVisible(false);
	SMouseAxis::SetDefaultMousePosition(Vector2i(WindowWidth / 2, WindowHeight / 2));

	if (glewInit()) {
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	SScreen::SetResolution(TVector2<uint32_t>(WindowWidth, WindowHeight));

	// Initialize debug text
	FDebug::Draw* DebugDraw = new FDebug::Draw;
	FDebug::Text* DebugText = new FDebug::Text;
	FDebug::GameConsole*  GameConsole = new FDebug::GameConsole;
}

FVoxiGineRoot::~FVoxiGineRoot()
{
	delete IFileSystem::GetInstancePtr();
	delete FDebug::Draw::GetInstancePtr();
	delete FDebug::Text::GetInstancePtr();
	delete FDebug::GameConsole::GetInstancePtr();
}

void CameraSetup();

void FVoxiGineRoot::Start()
{
	CameraSetup();
	mChunkManager.LoadWorld(L"GenWorld");
	
	// Load all subsystems
	FSystemManager& SystemManager = mWorld.GetSystemManager();
	FRenderSystem& Renderer = SystemManager.AddSystem<FRenderSystem>(mGameWindow, mChunkManager);
	FPhysicsSystem& Physics = SystemManager.AddSystem<FPhysicsSystem>();

	FDebug::GameConsole& Console = FDebug::GameConsole::GetInstance();
	Console.SetChunkManager(&mChunkManager);
	Console.SetPhysicsSystem(&Physics);
	Console.SetRenderSystem(&Renderer);

	mChunkManager.SetPhysicsSystem(Physics);

	FGameObjectManager& GameObjectManager = mWorld.GetObjectManager();
	GameObjectManager.RegisterComponentType<EComponent::DirectionalLight>();
	GameObjectManager.RegisterComponentType<EComponent::PointLight>();
	GameObjectManager.RegisterComponentType<EComponent::Collider>();
	GameObjectManager.RegisterComponentType<EComponent::RigidBody>();
	
	auto& DirectionalLight = GameObjectManager.CreateGameObject();
	FDirectionalLight& DLight = DirectionalLight.AddComponent<EComponent::DirectionalLight>();
	DLight.Color = Vector3f(.7, .7, .7);
	DirectionalLight.Transform.SetRotation(FQuaternion{ -40, 20, 0 });

	GameLoop();
}

void UpdateCamera(FTransform& Light);


void FVoxiGineRoot::GameLoop()
{
	STime::SetFixedUpdate(.02f);
	STime::SetDeltaTime(1.0f / 30.0f); // Default delta time with 30fps

	FSystemManager& SystemManager = mWorld.GetSystemManager();
	FGameObjectManager& GameObjectManager = mWorld.GetObjectManager();

	MainCamera.Transform.SetPosition(Vector3f{ 10, 50, 30 });

	auto& PointLight = GameObjectManager.CreateGameObject();
	FPointLight& PLight = PointLight.AddComponent<EComponent::PointLight>();
	PLight.Color = Vector3f(1, 1, 1);
	PLight.MinDistance = 0;
	PLight.MaxDistance = 0;

	// Game Loop
	float lag = 0.0f;
	while (mGameWindow.isOpen())
	{	
		if (SButtonEvent::GetMouseDown(sf::Mouse::Right))
		{
			Vector3f CamForward = MainCamera.Transform.GetRotation() * -Vector3f::Forward * 4.0f;
			Vector3f CamPosition = MainCamera.Transform.GetPosition() + CamForward;
			Vector3i IntPosition{ (int32_t)CamPosition.x, (int32_t)CamPosition.y, (int32_t)CamPosition.z };
			mChunkManager.DestroyBlock(IntPosition);
		}
		if (SButtonEvent::GetMouseDown(sf::Mouse::Left))
		{
			Vector3f CamForward = MainCamera.Transform.GetRotation() * -Vector3f::Forward * 4.0f;
			Vector3f CamPosition = MainCamera.Transform.GetPosition() + CamForward;
			Vector3i IntPosition{ (int32_t)CamPosition.x, (int32_t)CamPosition.y, (int32_t)CamPosition.z };
			mChunkManager.SetBlock(IntPosition, FBlock::Brick);
		}

		//lag += STime::GetDeltaTime();
		//while (lag >= STime::GetFixedUpdate())
		//{
		//	mChunkManager.Update();
		//	lag -= STime::GetFixedUpdate();
		//}

		mChunkManager.Update();
		UpdateCamera(PointLight.Transform);
		SystemManager.GetSystem(Systems::Physics)->Update();
		GameObjectManager.Update();
		
		// Render the frame
		SystemManager.GetSystem(Systems::Render)->Update();

		UpdateTimers();
		ServiceEvents();
	}

	mChunkManager.Shutdown();
}

void FVoxiGineRoot::ServiceEvents()
{
	// Windows events
	sf::Event Event;

	// Reset button and axes from previous frame
	SButtonEvent::ResetButtonEvents();
	SMouseAxis::ResetAxes();
	STextEntered::Reset();

	SMouseAxis::UpdateDelta(mGameWindow);
	static sf::Vector2i DefaultMouse(SMouseAxis::GetDefaultMousePosition().x, SMouseAxis::GetDefaultMousePosition().y); 
	sf::Mouse::setPosition(DefaultMouse, mGameWindow);

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

void CameraSetup()
{
	FCamera::Main = &MainCamera;
	FTransform& CameraTransform = MainCamera.Transform;
	const Vector3f CameraPosition = Vector3f{ 36.0f, 36.0f, 36.0f };
	CameraTransform.SetPosition(CameraPosition);

	MainCamera.SetProjection(FPerspectiveMatrix{ (float)WindowWidth / (float)WindowHeight, 35.0f, 1.0f, 1000.0f });
}

void UpdateCamera(FTransform& Light)
{
	float ZMovement = 0, XMovement = 0, YMovement = 0;
	float MoveSpeed = (75.0f * STime::GetDeltaTime()) * ((sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) ? 2.0f : 1.0f);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		ZMovement = MoveSpeed;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		ZMovement = -MoveSpeed;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		XMovement = MoveSpeed;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		XMovement = -MoveSpeed;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
		YMovement = MoveSpeed;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		YMovement = -MoveSpeed;

	const float LookSpeed = 15.0f * STime::GetDeltaTime();
	FQuaternion CameraRotation = MainCamera.Transform.GetRotation();

	Vector3f CameraForward = CameraRotation * Vector3f::Forward;
	CameraForward.y = 0.0f;
	CameraForward.Normalize();

	Vector3f CameraRight = Vector3f::Cross(CameraForward, Vector3f::Up);

	CameraRotation = FQuaternion{ CameraRight, (float)SMouseAxis::GetDelta().y * LookSpeed } * CameraRotation;
	CameraRotation = FQuaternion{ Vector3f::Up, -(float)SMouseAxis::GetDelta().x * LookSpeed } * CameraRotation;

	Vector3f NewForward = CameraRotation * Vector3f::Forward;
	
	if (abs(Vector3f::Dot(NewForward, Vector3f::Up)) < 0.999f)
		MainCamera.Transform.SetRotation(CameraRotation);

	const Vector3f Translation = Vector3f{ -XMovement, -YMovement, ZMovement } *MoveSpeed;
	MainCamera.Transform.Translate(Translation);

	Light.SetPosition(MainCamera.Transform.GetPosition());
}