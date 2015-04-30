#pragma once
#include "Atlas\System.h"
#include "ShaderProgram.h"
#include "UniformBlockStandard.h"
#include "Camera.h"
#include "DepthRenderTarget.h"

class FRenderSystem;

class ILightSystem : public Atlas::ISystem
{
public:
	ILightSystem(Atlas::FWorld& World, FRenderSystem& RenderSystem)
		: ISystem(World)
		, mRenderSystem(RenderSystem)
		, mLightShader()
	{
	}

protected:
	FRenderSystem& mRenderSystem;
	FShaderProgram mLightShader;
};

class FDirectionalLightSystem : public ILightSystem
{
public:
	FDirectionalLightSystem(Atlas::FWorld& World, FRenderSystem& RenderSystem);
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
	FUniformBlock mLightUniformBuffer;
};

class FPointLightSystem : public ILightSystem
{
public:
	FPointLightSystem(Atlas::FWorld& World, FRenderSystem& RenderSystem);
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
		float     Constant;
		float     Linear;
		float     Quadratic;
		float     Intensity;
	};
#pragma pack (pop)

private:
	FUniformBlock mUniformBuffer;
};

//class FSpotLightSystem : public Atlas::ISystem
//{
//public:
//	FSpotLightSystem(Atlas::FWorld& World);
//	~FSpotLightSystem();
//
//	void Update() override;
//
//private:
//	/**
//	* Structure of spot light struct
//	* used in shader buffer block.
//	*/
//#pragma pack (push, 1)
//	struct ShaderSpotLight
//	{
//		Vector3f  Position;
//		uint32_t  Pad0;
//		Vector3f  Color;
//		uint32_t  Pad1;
//		Vector3f  Direction;
//		float     MinDistance;
//		float     MaxDistance;
//		float     SpotExponent;
//	};
//#pragma pack (pop)
//
//private:
//	FShaderProgram mLightShader;
//	FUniformBlock mUniformBuffer;
//};
