#pragma once
#include "Atlas\Component.h"
#include "Atlas\ComponentTypes.h"
#include "Math\Vector3.h"

struct FDirectionalLight : public Atlas::IComponent
{
	Vector3f Color;
};

struct FPointLight : public FDirectionalLight
{
	float MinDistance;
	float MaxDistance;
};

struct FSpotLight : public FPointLight
{
	float SpotExponent;
};

template <>
struct Atlas::ComponentTraits::Object<Atlas::EComponent::DirectionalLight>
{
	using Type = FDirectionalLight;
};

template <>
struct Atlas::ComponentTraits::Object<Atlas::EComponent::PointLight>
{
	using Type = FPointLight;
};

template <>
struct Atlas::ComponentTraits::Object<Atlas::EComponent::SpotLight>
{
	using Type = FSpotLight;
};