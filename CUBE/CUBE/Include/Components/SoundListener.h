#pragma once

#include "FMOD\fmod.hpp"
#include "Atlas\Component.h"
#include "Atlas\ComponentTypes.h"

struct FSoundListener : public Atlas::IComponent
{
	float DopplerScale{ 1.0f };
	float DistanceFactor{ 1.0f };
	float RolloffScale{ 1.0f };
};

template <>
struct Atlas::ComponentTraits::Object<Atlas::EComponent::SoundListener>
{
	using Type = FSoundListener;
};