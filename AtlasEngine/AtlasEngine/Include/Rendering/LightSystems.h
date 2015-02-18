#pragma once
#include "Atlas\System.h"
#include "ShaderProgram.h"
#include "UniformBlockStandard.h"

class FDirectionalLightSystem : public Atlas::ISystem
{
public:
	FDirectionalLightSystem(Atlas::FWorld& World);
	~FDirectionalLightSystem();

	void Update() override;

private:
	/**
	* Structure of directional light struct
	* used in shader buffer block.
	*/
#pragma pack (push, 1)
	struct ShaderDirectionalLight
	{
		Vector3f  Direction;
		uint32_t  Pad0;
		Vector3f  Color;
	};
#pragma pack (pop)

private:
	FShaderProgram mLightShader;
	FUniformBlockStandard mUniformBuffer;
};

class FPointLightSystem : public Atlas::ISystem
{
public:
	FPointLightSystem(Atlas::FWorld& World);
	~FPointLightSystem();

	void Update() override;

private:
	/**
	* Structure of point light struct
	* used in shader buffer block.
	*/
#pragma pack (push, 1)
	struct ShaderPointLight
	{
		Vector3f  Position;
		uint32_t  Pad0;
		Vector3f  Color;
		float     MinDistance;
		float     MaxDistance;
	};
#pragma pack (pop)

private:
	FShaderProgram mLightShader;
	FUniformBlockStandard mUniformBuffer;
};

class FSpotLightSystem : public Atlas::ISystem
{
public:
	FSpotLightSystem(Atlas::FWorld& World);
	~FSpotLightSystem();

	void Update() override;

private:
	/**
	* Structure of spot light struct
	* used in shader buffer block.
	*/
#pragma pack (push, 1)
	struct ShaderSpotLight
	{
		Vector3f  Position;
		uint32_t  Pad0;
		Vector3f  Color;
		uint32_t  Pad1;
		Vector3f  Direction;
		float     MinDistance;
		float     MaxDistance;
		float     SpotExponent;
	};
#pragma pack (pop)

private:
	FShaderProgram mLightShader;
	FUniformBlockStandard mUniformBuffer;
};
