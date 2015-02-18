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

const uint32_t WindowWidth = 1800;
const uint32_t WindowHeight = 1100;

static FCamera MainCamera;
static const float WorldCenter{ (float)(FChunkManager::WORLD_SIZE / 2) * FChunk::CHUNK_SIZE + 1 };

using namespace Atlas;

FVoxiGineRoot::FVoxiGineRoot()
	: mGameWindow(sf::VideoMode{ WindowWidth, WindowHeight }, L"VoxiGine", sf::Style::Default, sf::ContextSettings(24,8,2,4,3))
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

}

FVoxiGineRoot::~FVoxiGineRoot()
{
	delete IFileSystem::GetInstancePtr();
}

void CameraSetup();

void FVoxiGineRoot::Start()
{
	CameraSetup();
	mChunkManager.Setup();
	
	// Load all subsystems
	FSystemManager& SystemManager = mWorld.GetSystemManager();
	SystemManager.AddSystem<FRenderSystem>(mGameWindow, mChunkManager);

	GameLoop();
}

void UpdateCamera(FTransform& Light);


void FVoxiGineRoot::GameLoop()
{
	STime::SetFixedUpdate(.02f);
	STime::SetDeltaTime(1.0f / 30.0f); // Default delta time with 30fps

	FSystemManager& SystemManager = mWorld.GetSystemManager();
	FGameObjectManager& GameObjectManager = mWorld.GetObjectManager();

	GameObjectManager.RegisterComponentType<EComponent::DirectionalLight>();
	GameObjectManager.RegisterComponentType<EComponent::PointLight>();
	GameObjectManager.RegisterComponentType<EComponent::SpotLight>();

	auto& obj = GameObjectManager.CreateGameObject();
	FPointLight& Light = obj.AddComponent<EComponent::PointLight>();
	Light.Color = Vector3f(.6, .6, .6);
	Light.MinDistance = 10;
	Light.MaxDistance = 200;
	//Light.SpotExponent = 40;
	//obj.Transform.SetRotation(FQuaternion{ Vector3f{1,0,1}.Normalize(), -45 });

	// Game Loop
	float lag = 0.0f;
	while (mGameWindow.isOpen())
	{	
		lag += STime::GetDeltaTime();
		while (lag >= STime::GetFixedUpdate())
		{
			UpdateCamera(obj.Transform);
			mChunkManager.Update();
			lag -= STime::GetFixedUpdate();
		}

		GameObjectManager.Update();
		
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

	// Reset button and axes from previous frame
	SButtonEvent::ResetButtonEvents();
	SMouseAxis::ResetAxes();

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
	FTransform& CameraTransform = MainCamera.Transform;
	const Vector3f CameraPosition = Vector3f{ WorldCenter, WorldCenter, WorldCenter };
	CameraTransform.SetPosition(CameraPosition);

	MainCamera.SetAspectRatio((float)WindowWidth / (float)WindowHeight);
	MainCamera.SetNearPlane(1.0f);
	MainCamera.SetFarPlane((float)FChunkManager::VISIBILITY_DISTANCE * FChunk::CHUNK_SIZE);
	MainCamera.SetFieldOfView(75.0f);
}

void UpdateCamera(FTransform& Light)
{
	float ZMovement = 0, XMovement = 0, YMovement = 0, MoveSpeed = 25.0f * STime::GetFixedUpdate();
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

	const float LookSpeed = 5.0f * STime::GetFixedUpdate();
	FQuaternion CameraRotation = MainCamera.Transform.GetRotation();

	Vector3f CameraForward = CameraRotation * Vector3f::Forward;
	CameraForward.y = 0.0f;
	CameraForward.Normalize();

	Vector3f CameraRight = Vector3f::Cross(CameraForward, Vector3f::Up);

	CameraRotation = FQuaternion{ CameraRight, (float)SMouseAxis::GetDelta().y * LookSpeed } * CameraRotation;
	CameraRotation = FQuaternion{ Vector3f::Up, -(float)SMouseAxis::GetDelta().x * LookSpeed } * CameraRotation;

	MainCamera.Transform.SetRotation(CameraRotation);

	const Vector3f Translation = Vector3f{ -XMovement, -YMovement, ZMovement } *MoveSpeed;
	MainCamera.Transform.Translate(Translation);

	Light.SetPosition(MainCamera.Transform.GetPosition());
	//Light.SetRotation(MainCamera.Transform.GetRotation());
}