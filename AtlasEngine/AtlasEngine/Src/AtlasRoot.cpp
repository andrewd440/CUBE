#include "..\Include\AtlasRoot.h"
#include <cstdint>

#include <SFML\Window\Context.hpp>
#include <SFML\Window\Event.hpp>
#include <SFML\Window\VideoMode.hpp>
#include "Clock.h"
#include "STime.h"
#include "Input\ButtonEvent.h"
#include "Input\MouseAxis.h"
#include "Debugging\ConsoleOutput.h"

#include <GL/glew.h>
#include "SystemResources\SystemFile.h"
#include "Rendering\ShaderProgram.h"
#include "Rendering\GLUtils.h"
#include "Math\Vector3.h"
#include "Math\Matrix4.h"
#include "Math\PerspectiveMatrix.h"
#include "Rendering\UniformBlockStandard.h"
#include "Common.h"
#include "Math\Quaternion.h"
#include "Rendering\ChunkManager.h"
#include "Debugging\DebugText.h"
#include "Rendering\Screen.h"
#include "ResourceHolder.h"
#include "Rendering\Camera.h"

const uint32_t WindowWidth = 1800;
const uint32_t WindowHeight = 1100;

static FCamera MainCamera;
static const float WorldCenter{ (float)(FChunkManager::WORLD_SIZE / 2) * FChunk::CHUNK_SIZE + 1 };
FUniformBlockStandard* UniformBuffer;

FAtlasRoot::FAtlasRoot()
	: mGameWindow(sf::VideoMode{ WindowWidth, WindowHeight }, L"Atlas Engine", sf::Style::Default, sf::ContextSettings(24,8,2,4,3))
{
	IFileSystem* FileSystem = new FFileSystem;
	mGameWindow.setMouseCursorVisible(false);
	SMouseAxis::SetDefaultMousePosition(Vector2i(WindowWidth / 2, WindowHeight / 2));

	if (glewInit()) {
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	UniformBuffer = new FUniformBlockStandard(0, 144);
	SScreen::SetResolution(TVector2<uint32_t>(WindowWidth, WindowHeight));

	// Load shaders
	FShaderHolder* Shaders = new FShaderHolder;
	Shaders->Load("BlinnVertex", L"Shaders/BlinnLighting.vert", GL_VERTEX_SHADER);
	Shaders->Load("BlinnFragment", L"Shaders/BlinnLighting.frag", GL_FRAGMENT_SHADER);
	Shaders->Load("DebugTextVertex", L"Shaders/DebugText.vert", GL_VERTEX_SHADER);
	Shaders->Load("DebugTextFragment", L"Shaders/DebugText.frag", GL_FRAGMENT_SHADER);
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(.1f, .2f, .3f, 1.0f);
}

FAtlasRoot::~FAtlasRoot()
{
	delete UniformBuffer;
	delete IFileSystem::GetInstancePtr();
	delete FShaderHolder::GetInstancePtr();
}

void FAtlasRoot::Start()
{
	GameLoop();
}

void GLTests();
void UpdateCamera();

void FAtlasRoot::GameLoop()
{
	STime::SetFixedUpdate(.0025f);
	STime::SetDeltaTime(1.0f / 30.0f); // Default delta time with 30fps
	
	GLTests();
	FChunkManager Chunks;
	Chunks.Setup();
	FDebugText Texting;
	Texting.SetStyle("Vera.ttf", 20, FDebugText::AtlasInfo{ 512, 512, 1 });
	
	// Game Loop
	while (mGameWindow.isOpen())
	{	
		if (SButtonEvent::GetKeyDown(sf::Keyboard::L))
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (SButtonEvent::GetKeyDown(sf::Keyboard::K))
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//// OpenGL /////////////////////
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Chunk rendering
		UpdateCamera();
		UniformBuffer->SetData(0, MainCamera.Transform.WorldToLocalMatrix());
		UniformBuffer->SendBuffer();
		Chunks.Update();
		Chunks.Render();

		// Debug Print
		const Vector3f CameraPosition = MainCamera.Transform.GetPosition();
		wchar_t String[250];
		swprintf_s(String, L"FPS: %.2f   Position: %.1f %.1f %.1f", 1.0f / STime::GetDeltaTime(), CameraPosition.x, CameraPosition.y, CameraPosition.z);
		Texting.AddText(String, FColor(1.0f, .8f, 1.0f, .6f), Vector2i(50, SScreen::GetResolution().y - 50));

		swprintf_s(String, L"Chunks used: %d", FChunk::ChunkAllocator.Size());
		Texting.AddText(String, FColor(1.0f, .8f, 1.0f, .6f), Vector2i(50, SScreen::GetResolution().y - 100));

		Vector3i ChunkPosition = Vector3i(CameraPosition.x / FChunk::CHUNK_SIZE, CameraPosition.y / FChunk::CHUNK_SIZE, CameraPosition.z / FChunk::CHUNK_SIZE);
		swprintf_s(String, L"Chunk Position: %d %d %d", ChunkPosition.x, ChunkPosition.y, ChunkPosition.z);
		Texting.AddText(String, FColor(1.0f, .8f, 1.0f, .6f), Vector2i(50 , SScreen::GetResolution().y - 150));

		Texting.Render();

		//////////////////////////////////
		mGameWindow.display();

		UpdateTimers();
		ServiceEvents();
	}
}

void FAtlasRoot::ServiceEvents()
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

void FAtlasRoot::UpdateTimers()
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

void GLTests()
{
	FTransform& CameraTransform = MainCamera.Transform;
	const Vector3f CameraPosition = Vector3f{ WorldCenter, WorldCenter, WorldCenter };
	const Vector3f CameraLookAt = CameraPosition + Vector3f{ 0, 0, -1 };
	CameraTransform.SetPosition(CameraPosition);
	CameraTransform.SetRotation(FQuaternion::LookAt(CameraPosition, CameraLookAt));

	MainCamera.SetAspectRatio((float)WindowWidth / (float)WindowHeight);
	MainCamera.SetNearPlane(1.0f);
	MainCamera.SetFarPlane((float)FChunkManager::VISIBILITY_DISTANCE * FChunk::CHUNK_SIZE);
	MainCamera.SetFieldOfView(75.0f);

	UniformBuffer->SetData(0, CameraTransform.WorldToLocalMatrix());
	UniformBuffer->SetData(64, MainCamera.GetProjection());
	UniformBuffer->SetData(128, Vector3f(0, 0, 0));
}

void UpdateCamera()
{
	float ZMovement = 0, XMovement = 0, YMovement = 0, MoveSpeed = 20.0f * STime::GetDeltaTime();;
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

	const float LookSpeed = 10.0f * STime::GetFixedUpdate();
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
}