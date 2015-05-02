#include "Rendering/LightSystems.h"
#include "ResourceHolder.h"
#include "Rendering\GLUtils.h"
#include "Math\Transform.h"
#include "Atlas\GameObject.h"
#include "Atlas\World.h"
#include "Rendering\Light.h"
#include "Math\Sphere.h"
#include "Input\ButtonEvent.h"
#include "Rendering\RenderSystem.h"
#include "Debugging\DebugDraw.h"
#include "Math\Box.h"
#include "Rendering\Screen.h"
#include <limits>

////////////////////////////////////////////////////////////////////////////////////
//////////////////////// Directional Light /////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

FDirectionalLightSystem::FDirectionalLightSystem(Atlas::FWorld& World, FRenderSystem& RenderSystem)
	: ILightSystem(World, RenderSystem)
	, mLightUniformBuffer(GLUniformBindings::DirectionalLight, sizeof(ShaderDirectionalLight))
{
	AddComponentType<Atlas::EComponent::DirectionalLight>();

	FShader FragShader{ L"Shaders/DeferredDirectionalLighting.frag", GL_FRAGMENT_SHADER };

	mLightShader.AttachShader(SShaderHolder::Get("FullScreenQuad.vert"));
	mLightShader.AttachShader(FragShader);
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
		// Get light transform and light component
		FTransform& LightTransform = Object->Transform;
		const FDirectionalLight& LightComponent = Object->GetComponent<EComponent::DirectionalLight>();

		// Set light data
		Light.Direction = LightTransform.GetRotation() * -Vector3f::Forward;
		Light.Color = LightComponent.Color;

		// Send light data
		mLightUniformBuffer.SetData(0, (uint8_t*)&Light, sizeof(ShaderDirectionalLight));

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}

////////////////////////////////////////////////////////////////////////////////////
//////////////////////// Point Light ///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

FPointLightSystem::FPointLightSystem(Atlas::FWorld& World, FRenderSystem& RenderSystem)
	: ILightSystem(World, RenderSystem)
	, mUniformBuffer(GLUniformBindings::PointLight, sizeof(ShaderPointLight))
{
	AddComponentType<Atlas::EComponent::PointLight>();

	FShader FragShader{ L"Shaders/DeferredPointLighting.frag", GL_FRAGMENT_SHADER };

	mLightShader.AttachShader(SShaderHolder::Get("FullScreenQuad.vert"));
	mLightShader.AttachShader(SShaderHolder::Get("DeferredLightingCommon.frag"));
	mLightShader.AttachShader(FragShader);
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
	const FFrustum Frustum = FCamera::Main->GetViewFrustum();
	const FMatrix4 ViewTransform = FCamera::Main->Transform.WorldToLocalMatrix();

	for (const auto& Object : GetGameObjects())
	{
		const FTransform& Transform = Object->Transform;
		const FPointLight& LightComponent = Object->GetComponent<EComponent::PointLight>();

		// Check if light is in the view volume
		const Vector3f LightViewSpace = ViewTransform.TransformPosition(Transform.GetWorldPosition());
		const FSphere LightVolume{ LightViewSpace, LightComponent.MaxDistance };

		if (Frustum.IsSphereVisible(LightVolume))
		{
			// Set light data
			Light.Position = LightViewSpace;
			Light.Color = LightComponent.Color;
			Light.Constant = LightComponent.Constant;
			Light.Linear = LightComponent.Linear;
			Light.Quadratic = LightComponent.Quadratic;
			Light.Intensity = LightComponent.Intensity;
		

			// Copy to buffer
			mUniformBuffer.SetData(0, (uint8_t*)&Light, sizeof(ShaderPointLight));

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
//////////////////////// Spot Light ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

//FSpotLightSystem::FSpotLightSystem(Atlas::FWorld& World)
//	: ISystem(World)
//	, mLightShader()
//	, mUniformBuffer(GLUniformBindings::SpotLight, sizeof(ShaderSpotLight))
//{
//	AddComponentType<Atlas::EComponent::SpotLight>();
//
//	mLightShader.AttachShader(SShaderHolder::Get("FullScreenQuad.vert"));
//	mLightShader.AttachShader(SShaderHolder::Get("DeferredLightingCommon.frag"));
//	mLightShader.AttachShader(SShaderHolder::Get("DeferredSpotLighting.frag"));
//	mLightShader.LinkProgram();
//}
//
//FSpotLightSystem::~FSpotLightSystem()
//{
//}
//
//void FSpotLightSystem::Update()
//{
//	using namespace Atlas;
//	mLightShader.Use();
//
//	ShaderSpotLight Light;
//
//	for (const auto& Object : GetGameObjects())
//	{
//		const FTransform& Transform = Object->Transform;
//		const FSpotLight& LightComponent = Object->GetComponent<EComponent::SpotLight>();
//
//		// Set light data
//		Light.Position = Transform.GetWorldPosition();
//		Light.Color = LightComponent.Color;
//		Light.Direction = Transform.GetRotation() * -Vector3f::Forward;
//		Light.MinDistance = LightComponent.MinDistance;
//		Light.MaxDistance = LightComponent.MaxDistance;
//		Light.SpotExponent = LightComponent.SpotExponent;
//
//		// Copy to buffer
//		mUniformBuffer.SetData(0, (uint8_t*)&Light, sizeof(ShaderSpotLight));
//
//		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//	}
//}