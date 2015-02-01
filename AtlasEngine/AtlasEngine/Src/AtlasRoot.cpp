#include "..\Include\AtlasRoot.h"
#include <cstdint>

#include <SFML\Window\Context.hpp>
#include <SFML\Window\Event.hpp>
#include <SFML\Window\VideoMode.hpp>
#include "..\Include\Clock.h"
#include "..\Include\STime.h"
#include "..\Include\Input\ButtonEvent.h"
#include "..\Include\Input\MouseAxis.h"
#include "..\Include\Debugging\ConsoleOutput.h"

#include <GL/glew.h>
#include "..\Include\SystemFile.h"
#include "..\Include\Rendering\ShaderProgram.h"
#include "..\Include\Rendering\GLUtils.h"
#include "..\Include\Math\Vector3.h"
#include "..\Include\Math\Matrix4.h"
#include "..\Include\Math\PerspectiveMatrix.h"
#include "..\Include\Rendering\UniformBlockStandard.h"
#include "..\Include\Common.h"
#include "..\Include\Math\Quaternion.h"
#include "..\Include\Rendering\ChunkManager.h"
#include "..\Include\Debugging\DebugText.h"
#include "..\Include\Rendering\Screen.h"
#include "ResourceHolder.h"

const uint32_t WindowWidth = 1800;
const uint32_t WindowHeight = 1100;

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
	glClearColor(.1, .2, .3, 1);
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

static const float WorldCenter{ (float)(FChunkManager::WORLD_SIZE / 2) * FChunk::CHUNK_SIZE + 1 };
Vector3f gEyePosition(WorldCenter, WorldCenter, WorldCenter), LookAt(32, 16, 33), Up(0, 1, 0);
LookAtMatrix ViewTransform = LookAtMatrix{ gEyePosition, LookAt, Up };
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

		ServiceEvents();

		//// OpenGL /////////////////////
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Chunk rendering
		UpdateCamera();
		UniformBuffer->SendBuffer();
		Chunks.Update();
		Chunks.Render();


		// Debug Print
		wchar_t String[250];
		swprintf_s(String, L"FPS: %.2f   Position: %.1f %.1f %.1f", 1.0f / STime::GetDeltaTime(), gEyePosition.x, gEyePosition.y, gEyePosition.z);
		Texting.AddText(String, FColor(1.0f, .8f, 1.0f, .6f), Vector2i(50, SScreen::GetResolution().y - 50));

		swprintf_s(String, L"Chunks used: %d", FChunk::ChunkAllocator.Size());
		Texting.AddText(String, FColor(1.0f, .8f, 1.0f, .6f), Vector2i(50, SScreen::GetResolution().y - 100));

		Vector3i ChunkPosition = Vector3i(gEyePosition.x / FChunk::CHUNK_SIZE, gEyePosition.y / FChunk::CHUNK_SIZE, gEyePosition.z / FChunk::CHUNK_SIZE);
		swprintf_s(String, L"Chunk Position: %d %d %d", ChunkPosition.x, ChunkPosition.y, ChunkPosition.z);
		Texting.AddText(String, FColor(1.0f, .8f, 1.0f, .6f), Vector2i(50 , SScreen::GetResolution().y - 150));

		Texting.Render();

		//////////////////////////////////
		mGameWindow.display();

		UpdateTimers();
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
	FPerspectiveMatrix Projection{ (float)WindowWidth, (float)WindowHeight, 35, 1, (float)FChunkManager::VISIBILITY_DISTANCE };
	UniformBuffer->SetData(64, Projection);
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
	Vector3f NAxis = (LookAt - gEyePosition).Normalize();
	Vector3f UAxis = Vector3f::Cross(NAxis, Up).Normalize();
	// Y Rotation
	LookAt -= gEyePosition;
	FQuaternion RotateY{ Up, -(float)SMouseAxis::GetDelta().x * LookSpeed };
	LookAt = RotateY * LookAt;
	LookAt += gEyePosition;

	// X Rotation
	FQuaternion RotateX{ UAxis, -(float)SMouseAxis::GetDelta().y * LookSpeed };
	LookAt -= gEyePosition;
	LookAt = RotateX * LookAt;
	LookAt += gEyePosition;

	// Side movement
	Vector3f CameraMovement;
	NAxis = (LookAt - gEyePosition).Normalize();
	UAxis = Vector3f::Cross(NAxis, Up).Normalize();
	CameraMovement += UAxis * XMovement;
	CameraMovement += NAxis * ZMovement;
	CameraMovement += Up * YMovement;
	gEyePosition += CameraMovement;
	LookAt += CameraMovement;

	ViewTransform = LookAtMatrix(gEyePosition, LookAt, Up);
	UniformBuffer->SetData(0, ViewTransform);
}