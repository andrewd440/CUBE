#pragma once
#include "Atlas\System.h"
#include "FMOD\fmod.hpp"

class FAudioListenerSystem;

/**
* System that manages the audio engine.
*/
class FAudioSystem : public Atlas::ISystem
{
public:
	FAudioSystem(Atlas::FWorld& World);
	~FAudioSystem();

	void Update() override;

private:
	void OnGameObjectRemove(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent) override;
	void OnGameObjectAdd(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent) override;
private:
	FMOD::System* mSystem;
	FAudioListenerSystem* mListenerSubSystem;
};


/**
* Manages the audio listener gameobject.
*/
class FAudioListenerSystem : public Atlas::ISystem
{
public:
	FAudioListenerSystem(Atlas::FWorld& World, FMOD::System* System);
	~FAudioListenerSystem() = default;

	void Update() override;

private:
	void OnGameObjectAdd(Atlas::FGameObject& GameObject, Atlas::IComponent& UpdateComponent) override;

private:
	FMOD::System* mSystem;
};