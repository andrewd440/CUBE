#include "Components\TimeBombShooter.h"
#include "Atlas\GameObject.h"
#include "Rendering\Camera.h"
#include "Components\MeshRenderer.h"
#include "Components\TimeBomb.h"
#include "Input\ButtonEvent.h"
#include "Rendering\Light.h"

CTimeBombShooter::CTimeBombShooter()
	: FBehavior()
{
}


CTimeBombShooter::~CTimeBombShooter()
{
}

void CTimeBombShooter::OnStart()
{

}

void CTimeBombShooter::Update()
{
	if (SButtonEvent::GetKeyDown(sf::Keyboard::Space))
		ShootBomb();
}

void CTimeBombShooter::ShootBomb()
{
	FCamera& Camera = *FCamera::Main;

	auto& Box = CreateGameObject();
	Box.Transform.SetPosition(Camera.Transform.GetPosition() + Vector3f::Up * 2.0f);
	Box.Transform.SetScale(Vector3f{ .5f, .5f, .5f });
	
	auto& Body = Box.AddComponent<Atlas::EComponent::RigidBody>();
	Vector3f Forward = Camera.Transform.GetRotation() * -Vector3f::Forward;
	Body.Body.setLinearVelocity(btVector3{ Forward.x, Forward.y, Forward.z } * 40.0f);
	Body.BoxCollider.setImplicitShapeDimensions(btVector3{ .5f, .5f, .5f });
	
	auto& Mesh = Box.AddComponent<Atlas::EComponent::MeshRenderer>();
	Mesh.LinkToMesh("Box");

	//auto& Light = Box.AddComponent<Atlas::EComponent::PointLight>();
	//Light.Intensity = 15.0f;
	//Light.Constant = 0.1f;
	//Light.Linear = 0.4f;
	//Light.Quadratic = 0.7f;
	//Light.MaxDistance = 10.0f;
	//Light.Color = Vector3f{ 1, 0, 0 };

	Box.AddBehavior<CTimeBomb>();
}