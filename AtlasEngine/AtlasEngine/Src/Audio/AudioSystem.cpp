#include "Audio\AudioSystem.h"
#include "FMOD\fmod_errors.h"
#include "Components\SoundEmitter.h"
#include "Components\SoundListener.h"

FAudioSystem::FAudioSystem(Atlas::FWorld& World)
	: ISystem(World)
	, mSystem(nullptr)
	, mListenerSubSystem(nullptr)
{
	FMOD_RESULT Result = FMOD::System_Create(&mSystem);
	if (Result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", Result, FMOD_ErrorString(Result));
		exit(-1);
	}

	Result = mSystem->init(512, FMOD_INIT_NORMAL, 0);
	if (Result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", Result, FMOD_ErrorString(Result));
		exit(-1);
	}

	AddComponentType<Atlas::EComponent::SoundEmitter>();
	mListenerSubSystem = &AddSubSystem<FAudioListenerSystem>(mSystem);
}


FAudioSystem::~FAudioSystem()
{
	mSystem->close();
	FMOD_RESULT Result = mSystem->release();
	if (Result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", Result, FMOD_ErrorString(Result));
		exit(-1);
	}
}


void FAudioSystem::Update()
{
	FMOD_VECTOR Velocity = { 0.0f, 0.0f, 0.0f }; // Disregard velocity for now

	auto& Objects = GetGameObjects();
	for (auto& Object : Objects)
	{
		const Vector3f Position = Object->Transform.GetWorldPosition();
		FMOD_VECTOR FMODPosition = { Position.x, Position.y, Position.z };

		FSoundEmitter& Emitter = Object->GetComponent<Atlas::EComponent::SoundEmitter>();
		Emitter.Channel->set3DAttributes(&FMODPosition, &Velocity);

		if (Emitter.Filename.size() > 0)
		{
			Emitter.Sound->release();
			mSystem->createSound(Emitter.Filename.data(), FMOD_3D, nullptr, &Emitter.Sound);
			Emitter.Filename.resize(0);
		}

		if (Emitter.ActivateSound)
		{
			mSystem->playSound(Emitter.Sound, nullptr, false, &Emitter.Channel);
			Emitter.ActivateSound = false;
		}
	}

	// Update audio listener
	mListenerSubSystem->Update();

	mSystem->update();
}

void FAudioSystem::OnGameObjectRemove(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent)
{
	FSoundEmitter& Emitter = *static_cast<FSoundEmitter*>(&UpdateComponent);
	Emitter.Sound->release();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////// FAudioListenerSystem ////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FAudioListenerSystem::FAudioListenerSystem(Atlas::FWorld& World, FMOD::System* System)
	: ISystem(World)
	, mSystem(System)
{
	AddComponentType<Atlas::EComponent::SoundListener>();
}

void FAudioListenerSystem::Update()
{
	// Only supporting one listener for now
	using namespace Atlas;

	FMOD_VECTOR Velocity = { 0.0f, 0.0f, 0.0f }; // Disregard velocity for now

	auto& Objects = GetGameObjects();
	if (Objects.size() > 0)
	{
		FGameObject& Object = *Objects[0];
		const Vector3f ListenerPosition = Object.Transform.GetWorldPosition();
		FMOD_VECTOR FMODPosition = { ListenerPosition.x, ListenerPosition.y, ListenerPosition.z };

		const FQuaternion Rotation = Object.Transform.GetRotation();
		const Vector3f ForwardDirection = Rotation * -Vector3f::Forward;
		FMOD_VECTOR FMODForward = { ForwardDirection.x, ForwardDirection.y, ForwardDirection.z };

		const Vector3f UpDirection = Rotation * Vector3f::Up;
		FMOD_VECTOR FMODUp = { UpDirection.x, UpDirection.y, UpDirection.z };

		FSoundListener& Listener = *static_cast<FSoundListener*>(&Objects[0]->GetComponent<EComponent::SoundListener>());
		mSystem->set3DListenerAttributes(0, &FMODPosition, &Velocity, &FMODForward, &FMODUp);
	}
}

void FAudioListenerSystem::OnGameObjectAdd(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent)
{
	FSoundListener& Listener = *static_cast<FSoundListener*>(&UpdateComponent);
	mSystem->set3DSettings(Listener.DopplerScale, Listener.DistanceFactor, Listener.RolloffScale);
}