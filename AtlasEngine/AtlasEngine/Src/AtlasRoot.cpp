#include "..\Include\AtlasRoot.h"
#include <cstdint>

#include <SFML\Window\Context.hpp>
#include <SFML\Window\Event.hpp>
#include <SFML\Window\VideoMode.hpp>
#include "..\Include\Clock.h"
#include "..\Include\FTime.h"
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
#include "..\Include\Rendering\VertexBufferObject.h"
#include "..\Include\Rendering\Chunk.h"

namespace
{
	static const uint32_t WindowWidth = 1800;
	static const uint32_t WindowHeight = 1100;
}

FUniformBlockStandard* UniformBuffer;

FAtlasRoot::FAtlasRoot()
	: mGameWindow(sf::VideoMode{ WindowWidth, WindowHeight }, L"Atlas Engine", sf::Style::Default, sf::ContextSettings(24,8,2,4,3))
{
	if (glewInit()) {
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
		exit(EXIT_FAILURE);
	}

	IFileSystem* FileSystem = new FFileSystem;
	UniformBuffer = new FUniformBlockStandard(0, 144);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	FMouseAxis::SetDefaultMousePosition(Vector2i(WindowWidth / 2, WindowHeight / 2));
}


FAtlasRoot::~FAtlasRoot()
{
	delete UniformBuffer;
	delete IFileSystem::GetInstancePtr();
}


void FAtlasRoot::Start()
{
	GameLoop();
}

Vector3f Eye(0,0,30), LookAt(0,0,0), Up(0,1,0);
LookAtMatrix ViewTransform = LookAtMatrix{ Eye, LookAt, Up };
FShaderProgram* ShaderProgram = nullptr;

void GLTests()
{
	FShader VShader{ L"Triangle.vert", GL_VERTEX_SHADER };
	FShader FShader{ L"Triangle.frag", GL_FRAGMENT_SHADER };
	ShaderProgram = new FShaderProgram{ &VShader, &FShader };
	ShaderProgram->Use();

	FPerspectiveMatrix Projection{ (float)WindowWidth, (float)WindowHeight, 35, 1, 100 };
	UniformBuffer->SetData(64, Projection);
	UniformBuffer->SetData(128, Vector3f(0,0,0));
}

void UpdateCamera()
{
	float ZMovement = 0, XMovement = 0, YMovement = 0, MoveSpeed = 20.0f * FTime::GetDeltaTime();;
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


	const float LookSpeed = 10.0f * FTime::GetFixedUpdate();
	Vector3f NAxis = (LookAt - Eye).Normalize();
	Vector3f UAxis = Vector3f::Cross(NAxis, Up).Normalize();
	// Y Rotation
	LookAt -= Eye;
	FQuaternion RotateY{ Up, -(float)FMouseAxis::GetDelta().x * LookSpeed};
	LookAt = RotateY * LookAt;
	LookAt += Eye;

	// X Rotation
	FQuaternion RotateX{ UAxis, -(float)FMouseAxis::GetDelta().y * LookSpeed };
	LookAt -= Eye;
	LookAt = RotateX * LookAt;
	LookAt += Eye;

	// Side movement
	Vector3f CameraMovement;
	NAxis = (LookAt - Eye).Normalize();
	UAxis = Vector3f::Cross(NAxis, Up).Normalize();
	CameraMovement += UAxis * XMovement;
	CameraMovement += NAxis * ZMovement;
	CameraMovement += Up * YMovement;
	Eye += CameraMovement;
	LookAt += CameraMovement;

	ViewTransform = LookAtMatrix(Eye, LookAt, Up);
	UniformBuffer->SetData(0, ViewTransform);
}

void FAtlasRoot::GameLoop()
{
	// Setup game timers
	FClock GameTimer;

	uint64_t FrameStart, FrameEnd;
	FTime::SetFixedUpdate(.0025f);
	FTime::SetDeltaTime(1.0f / 30.0f); // Default delta time with 30fps
	FrameStart = FClock::ReadSystemTimer();
	
	
	GLTests();
	FChunk LotsBlocks;
	LotsBlocks.CreateMesh();

	// Game Loop
	while (mGameWindow.isOpen())
	{
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
		FTime::SetDeltaTime(DeltaTime);
		FrameStart = FrameEnd;

		ServiceEvents();

		//// OpenGL /////////////////////
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		UpdateCamera();

		UniformBuffer->SendBuffer();
		LotsBlocks.Render();


		//std::cout << FTime::GetDeltaTime() << std::endl;
		//FDebug::PrintF("Frame Time: %f", FTime::GetDeltaTime());
		//////////////////////////////////
		mGameWindow.display();
		//Sleep(100);
	}
	//////////////
	////////////
	////////
	delete ShaderProgram;
}

void FAtlasRoot::ServiceEvents()
{
	// Windows events
	sf::Event Event;

	// Reset button and axes from previous frame
	FButtonEvent::ResetButtonEvents();
	FMouseAxis::ResetAxes();

	FMouseAxis::UpdateDelta(mGameWindow);
	static sf::Vector2i DefaultMouse(FMouseAxis::GetDefaultMousePosition().x, FMouseAxis::GetDefaultMousePosition().y);
	sf::Mouse::setPosition(DefaultMouse, mGameWindow);

	// Service window events
	while (mGameWindow.pollEvent(Event))
	{
		if (Event.type == Event.Closed || FButtonEvent::GetKeyDown(sf::Keyboard::Escape))
			mGameWindow.close();

		if (FButtonEvent::IsButtonEvent(Event))
		{
			FButtonEvent::AddButtonEvent(Event);
		}
		else if (FMouseAxis::IsMouseAxisEvent(Event))
		{
			FMouseAxis::UpdateEvent(Event);
		}
		else if (Event.type == sf::Event::Resized)
		{
			// adjust the viewport when the window is resized
			glViewport(0, 0, Event.size.width, Event.size.height);
		}
	}
}