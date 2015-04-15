#pragma once

#include <string>

#include "FMOD\fmod.hpp"
#include "Atlas\Component.h"
#include "Atlas\ComponentTypes.h"

struct FSoundEmitter : public Atlas::IComponent
{
	std::string    Filename;       // Filepath for the audio file
	FMOD::Sound*   Sound;
	FMOD::Channel* Channel;
	bool           ActivateSound;  // If set to true, the sound with be activated in the next update
};

template <>
struct Atlas::ComponentTraits::Object<Atlas::EComponent::SoundEmitter>
{
	using Type = FSoundEmitter;
};