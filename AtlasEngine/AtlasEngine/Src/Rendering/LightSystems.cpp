#include "Rendering/LightSystems.h"
#include "ResourceHolder.h"
#include "Rendering\GLUtils.h"
#include "Math\Transform.h"
#include "Atlas\GameObject.h"
#include "Atlas\World.h"
#include "Rendering\Light.h"

////////////////////////////////////////////////////////////////////////////////////
//////////////////////// Directional Light /////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////


FDirectionalLightSystem::FDirectionalLightSystem(Atlas::FWorld& World)
	:ISystem(World)
	, mLightShader()
	, mUniformBuffer(GLUniformBindings::DirectionalLight, sizeof(ShaderDirectionalLight))
{
	AddComponentType<Atlas::EComponent::DirectionalLight>();

	mLightShader.AttachShader(SShaderHolder::Get("FullScreenQuad.vert"));
	mLightShader.AttachShader(SShaderHolder::Get("DeferredLightingCommon.frag"));
	mLightShader.AttachShader(SShaderHolder::Get("DeferredDirectionalLighting.frag"));
	mLightShader.LinkProgram();
}

FDirectionalLightSystem::~FDirectionalLightSystem()
{

}

void FDirectionalLightSystem::Update()
{
	using namespace Atlas;
	mLightShader.Use();

	ShaderDirectionalLight Light;

	for (const auto& Object : GetGameObjects())
	{
		const FTransform& Transform = Object->Transform;
		const FDirectionalLight& LightComponent = Object->GetComponent<EComponent::DirectionalLight>();

		// Set light data
		Light.Direction = Transform.GetRotation() * -Vector3f::Forward;
		Light.Color = LightComponent.Color;

		// Copy to buffer
		mUniformBuffer.SetData(0, (uint8_t*)&Light, sizeof(ShaderDirectionalLight));

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}

////////////////////////////////////////////////////////////////////////////////////
//////////////////////// Point Light ///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

FPointLightSystem::FPointLightSystem(Atlas::FWorld& World)
	: ISystem(World)
	, mLightShader()
	, mUniformBuffer(GLUniformBindings::PointLight, sizeof(ShaderPointLight))
{
	AddComponentType<Atlas::EComponent::PointLight>();

	mLightShader.AttachShader(SShaderHolder::Get("FullScreenQuad.vert"));
	mLightShader.AttachShader(SShaderHolder::Get("DeferredLightingCommon.frag"));
	mLightShader.AttachShader(SShaderHolder::Get("DeferredPointLighting.frag"));
	mLightShader.LinkProgram();
}


FPointLightSystem::~FPointLightSystem()
{
}

void FPointLightSystem::Update()
{
	using namespace Atlas;
	mLightShader.Use();

	ShaderPointLight Light;

	for (const auto& Object : GetGameObjects())
	{
		const FTransform& Transform = Object->Transform;
		const FPointLight& LightComponent = Object->GetComponent<EComponent::PointLight>();

		// Set light data
		Light.Position = Transform.GetWorldPosition();
		Light.Color = LightComponent.Color;
		Light.MinDistance = LightComponent.MinDistance;
		Light.MaxDistance = LightComponent.MaxDistance;

		// Copy to buffer
		mUniformBuffer.SetData(0, (uint8_t*)&Light, sizeof(ShaderPointLight));

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}

////////////////////////////////////////////////////////////////////////////////////
//////////////////////// Spot Light ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

FSpotLightSystem::FSpotLightSystem(Atlas::FWorld& World)
	: ISystem(World)
	, mLightShader()
	, mUniformBuffer(GLUniformBindings::SpotLight, sizeof(ShaderSpotLight))
{
	AddComponentType<Atlas::EComponent::SpotLight>();

	mLightShader.AttachShader(SShaderHolder::Get("FullScreenQuad.vert"));
	mLightShader.AttachShader(SShaderHolder::Get("DeferredLightingCommon.frag"));
	mLightShader.AttachShader(SShaderHolder::Get("DeferredSpotLighting.frag"));
	mLightShader.LinkProgram();
}

FSpotLightSystem::~FSpotLightSystem()
{
}

void FSpotLightSystem::Update()
{
	using namespace Atlas;
	mLightShader.Use();

	ShaderSpotLight Light;

	for (const auto& Object : GetGameObjects())
	{
		const FTransform& Transform = Object->Transform;
		const FSpotLight& LightComponent = Object->GetComponent<EComponent::SpotLight>();

		// Set light data
		Light.Position = Transform.GetWorldPosition();
		Light.Color = LightComponent.Color;
		Light.Direction = Transform.GetRotation() * -Vector3f::Forward;
		Light.MinDistance = LightComponent.MinDistance;
		Light.MaxDistance = LightComponent.MaxDistance;
		Light.SpotExponent = LightComponent.SpotExponent;

		// Copy to buffer
		mUniformBuffer.SetData(0, (uint8_t*)&Light, sizeof(ShaderSpotLight));

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}