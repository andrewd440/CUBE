#pragma once
#include "Atlas\System.h"

class FDirectionalLightSystem : public Atlas::ISystem
{
public:
	FDirectionalLightSystem(Atlas::FWorld& World);
	~FDirectionalLightSystem();

	void Update() override;
};

class FPointLightSystem : public Atlas::ISystem
{
public:
	FPointLightSystem(Atlas::FWorld& World);
	~FPointLightSystem();

	void Update() override;
};

class FSpotLightSystem : public Atlas::ISystem
{
public:
	FSpotLightSystem(Atlas::FWorld& World);
	~FSpotLightSystem();

	void Update() override;
};
