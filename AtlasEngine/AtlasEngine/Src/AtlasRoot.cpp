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

namespace
{
	static const uint32_t WindowWidth = 1600;
	static const uint32_t WindowHeight = 1000;
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
	UniformBuffer = new FUniformBlockStandard(0, 128);

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

enum Attribute_IDs{ vPosition, vColor, AttrCount };
enum VertexArray {VVertices, VColors};
enum Buffers { BVerts, BColors};
GLuint CubeArray;
GLuint VertexBuffer;
GLuint EBO;
Vector3f Eye(0,0,10), LookAt(0,0,0), Up(0,1,0);
LookAtMatrix ViewTransform = LookAtMatrix{ Eye, LookAt, Up };
FShaderProgram* ShaderProgram = nullptr;

void DrawCube()
{
	static const float BlockWidth = .5f;

	static struct Vertex
	{
		Vector3f Position;
		Vector3f Color;
	} Vertices[8];

	Vertices[0].Position = Vector3f(-BlockWidth, -BlockWidth, BlockWidth); // Bottom - Left - Front
	Vertices[1].Position = Vector3f(-BlockWidth, BlockWidth, BlockWidth);  // Top - Left - Front
	Vertices[2].Position = Vector3f(BlockWidth, BlockWidth, BlockWidth);	// Top - Right - Front
	Vertices[3].Position = Vector3f(BlockWidth, -BlockWidth, BlockWidth);	// Bottom - Right - Front
	Vertices[4].Position = Vector3f(-BlockWidth, -BlockWidth, -BlockWidth);// Bottom - Left - Back
	Vertices[5].Position = Vector3f(-BlockWidth, BlockWidth, -BlockWidth); // Top - Left - Back
	Vertices[6].Position = Vector3f(BlockWidth, BlockWidth, -BlockWidth);  // Top - Right - Back
	Vertices[7].Position = Vector3f(BlockWidth, -BlockWidth, -BlockWidth); // Bottom - Right - Back

	Vertices[0].Color = Vector3f(1, 1, 1); // Front
	Vertices[1].Color = Vector3f(.85, .85, .85);  // Top
	Vertices[2].Color = Vector3f(.75, .75, .75);	// Right
	Vertices[3].Color = Vector3f(.65, .65, .65);	// Bottom
	Vertices[4].Color = Vector3f(.55, .55, .55);// Left
	Vertices[5].Color = Vector3f(.45, .45, .45); // Back
	Vertices[6].Color = Vector3f(.35, .35, .35);
	Vertices[7].Color = Vector3f(.25, .25, .25);


	static const uint32_t Indices[36] = {
		1, 0, 3, // Front
		1, 3, 2,
		5, 4, 0, // Left
		5, 0, 1,
		3, 0, 4, // Bottom
		3, 4, 7,
		5, 6, 7, // Back
		5, 7, 4,
		2, 3, 7, // Right
		7, 6, 2,
		5, 1, 2, // Top
		5, 2, 6
	};

	glGenVertexArrays(1, &CubeArray);
	glGenBuffers(1, &VertexBuffer);

	glGenBuffers(1, &EBO);
	glBindVertexArray(CubeArray);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(offsetof(Vertex, Position)));
	glEnableVertexAttribArray(vPosition);

	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(offsetof(Vertex, Color)));
	glEnableVertexAttribArray(vColor);
	glBindVertexArray(0);

	
}

void GLTests()
{
	FShader VShader{ L"Triangle.vert", GL_VERTEX_SHADER };
	FShader FShader{ L"Triangle.frag", GL_FRAGMENT_SHADER };
	ShaderProgram = new FShaderProgram{ &VShader, &FShader };
	ShaderProgram->Use();
	DrawCube();

	FPerspectiveMatrix Projection{ (float)WindowWidth, (float)WindowHeight, 35, 1, 100 };
	UniformBuffer->SetData(64, Projection);
}

void GLLoop(const FClock& Clock)
{
	//GLint TimeDeltaLocation = glGetUniformLocation(ShaderProgram->GetID(), "TimeDelta");
	//if (TimeDeltaLocation < 0)
	//{
	//	FDebug::PrintF("TimeDeltaLocation not found in shader.");
	//}
	//glUniform1f(TimeDeltaLocation, FClock::CyclesToSeconds(Clock.GetCycles()));


}

void UpdateCamera()
{
	float ZMovement = 0, XMovement = 0, YMovement = 0, MoveSpeed = .002f;
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


	const float LookSpeed = 0.05f;
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
	UniformBuffer->SendBuffer();
}

void FAtlasRoot::GameLoop()
{
	// Setup game timers
	FClock GameTimer;

	uint64_t FrameStart, FrameEnd;
	FTime::SetDeltaTime(1.0f / 30.0f); // Default delta time with 30fps
	FrameStart = FClock::ReadSystemTimer();

	GLTests();
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
		GLLoop(GameTimer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(CubeArray);
		UpdateCamera();
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		//glDrawArrays(GL_TRIANGLES, 0, 12);
		glBindVertexArray(0);
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