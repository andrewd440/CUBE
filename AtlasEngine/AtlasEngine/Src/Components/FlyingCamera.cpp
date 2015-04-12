#include "Components\FlyingCamera.h"
#include "Rendering\Camera.h"
#include "STime.h"
#include "Input\ButtonEvent.h"
#include "Input\MouseAxis.h"
#include "ChunkSystems\ChunkManager.h"
#include "Atlas\GameObject.h"

CFlyingCamera::CFlyingCamera()
	: FBehavior()
	, mCamera(nullptr)
	, mMoveSpeed(10.0f)
	, mLookSpeed(5.0f)
{
}


CFlyingCamera::~CFlyingCamera()
{
}

void CFlyingCamera::OnStart()
{
	mCamera = FCamera::Main;
}

void CFlyingCamera::Update()
{
	float ZMovement = 0, XMovement = 0, YMovement = 0;
	float MoveSpeed = (mMoveSpeed * STime::GetDeltaTime()) * ((sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) ? 2.0f : 1.0f);

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

	const float LookSpeed = mLookSpeed * STime::GetDeltaTime();
	FQuaternion CameraRotation = mCamera->Transform.GetRotation();

	Vector3f CameraForward = CameraRotation * Vector3f::Forward;
	CameraForward.y = 0.0f;
	CameraForward.Normalize();

	Vector3f CameraRight = Vector3f::Cross(CameraForward, Vector3f::Up);

	CameraRotation = FQuaternion{ CameraRight, (float)SMouseAxis::GetDelta().y * LookSpeed } * CameraRotation;
	CameraRotation = FQuaternion{ Vector3f::Up, -(float)SMouseAxis::GetDelta().x * LookSpeed } * CameraRotation;

	Vector3f NewForward = CameraRotation * Vector3f::Forward;

	if (abs(Vector3f::Dot(NewForward, Vector3f::Up)) < 0.9f)
		mCamera->Transform.SetRotation(CameraRotation);

	const Vector3f Translation = Vector3f{ -XMovement, -YMovement, ZMovement };
	mCamera->Transform.Translate(Translation);
}