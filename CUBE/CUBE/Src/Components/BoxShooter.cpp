#include "Components\BoxShooter.h"
#include "Input\ButtonEvent.h"
#include "Rendering\Camera.h"
#include "Atlas\GameObject.h"
#include "Components\MeshRenderer.h"
#include "Components\SoundEmitter.h"
#include "Rendering\Light.h"

CBoxShooter::CBoxShooter()
{
}


CBoxShooter::~CBoxShooter()
{
}

void CBoxShooter::Update()
{
	if (SButtonEvent::GetKeyDown(sf::Keyboard::Space))
	{
		FCamera& Camera = *FCamera::Main;

		auto& Box = CreateGameObject();
		Box.Transform.SetLocalPosition(Camera.Transform.GetWorldPosition() + Vector3f::Up * 2.0f);
		Box.Transform.SetScale(Vector3f{ .5f, .5f, .5f });

		auto& Body = Box.AddComponent<Atlas::EComponent::RigidBody>();
		Vector3f Forward = Camera.Transform.GetRotation() * -Vector3f::Forward;
		Body.CapsuleCollider.setImplicitShapeDimensions(btVector3{ 1, 1, 1 });
		Body.Body.setLinearVelocity(btVector3{ Forward.x, Forward.y, Forward.z } *40.0f);
		//Body.BoxCollider.setImplicitShapeDimensions(btVector3{ .5f, .5f, .5f });

		auto& Mesh = Box.AddComponent<Atlas::EComponent::MeshRenderer>();
		Mesh.LinkToMesh("Box");

		auto& Light = Box.AddComponent<Atlas::EComponent::PointLight>();
		Light.Intensity = 50.0f;
		Light.MaxDistance = 25.0f;
		Light.Quadratic = .5f;
		Light.Linear = .6f;
		Light.Color = Vector3f{ .4f, .8f, .5f };
	}
}