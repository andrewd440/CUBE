#include "Rendering\RenderSystem.h"
#include "ResourceHolder.h"
#include "Input\ButtonEvent.h"
#include "ChunkSystems\ChunkManager.h"
#include "Rendering\Camera.h"
#include "Rendering\Screen.h"
#include "STime.h"
#include "Debugging\DebugDraw.h"
#include "Debugging\DebugText.h"
#include "Debugging\GameConsole.h"
#include "Rendering\LightSystems.h"
#include <GL\glew.h>
#include "freetype-gl\markup.h"
#include "Atlas\World.h"
#include "Components\ObjectMesh.h"
#include "Rendering\GLBindings.h"
#include "ChunkSystems\Block.h"
#include "Components\MeshRenderer.h"
#include "ChunkSystems\BlockTypes.h"
#include "Rendering\GLUtils.h"

// Shader buffer blocks info
namespace
{
	namespace TransformBuffer
	{
		enum : uint32_t
		{
			Model = 0,
			View = 64,
			Projection = 128,
			InvProjection = 192,
			Size = 256
		};
	}

	namespace ProjectionInfoBlock
	{
		enum : uint32_t
		{
			Near = 0,
			Far = 4,
			Size = 8
		};
	}

	namespace ResolutionBlock
	{
		enum : uint32_t
		{
			Resolution = 0,
			Size = 8
		};
	}
}

TEvent<Vector2ui> FRenderSystem::OnResolutionChange;

FRenderSystem::FRenderSystem(Atlas::FWorld& World, sf::Window& GameWindow, FChunkManager& ChunkManager)
	: ISystem(World)
	, mWindow(GameWindow)
	, mChunkManager(ChunkManager)
	, mDeferredRender()
	, mChunkRender()
	, mGBuffer()
	, mPostProcesses()
	, mTransformBlock(GLUniformBindings::TransformBlock, TransformBuffer::Size)
	, mResolutionBlock(GLUniformBindings::ResolutionBlock, ResolutionBlock::Size)
	, mProjectionInfoBlock(GLUniformBindings::ProjectionInfoBlock, ProjectionInfoBlock::Size)
	, mBlockInfoBuffer(0)
{
	mGBuffer.FBO = 0;
	mGBuffer.DepthTex = 0;
	mGBuffer.ColorTex[0] = 0;

	SetResolution(Vector2ui{ GameWindow.getSize().x, GameWindow.getSize().y });
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0, 0, 0, 1.0f);

	LoadShaders();
	LoadSubSystems();

	AddComponentType<Atlas::EComponent::MeshRenderer>();
}

void FRenderSystem::LoadShaders()
{
	// Load all main rendering shaders
	SShaderHolder::Load("FullScreenQuad.vert", L"Shaders/FullScreenQuad.vert", GL_VERTEX_SHADER);
	
	// Load render shaders
	FShader DeferredVert{ L"Shaders/DeferredRender.vert", GL_VERTEX_SHADER };
	FShader DeferredFrag{ L"Shaders/DeferredRender.frag", GL_FRAGMENT_SHADER };
	mDeferredRender.AttachShader(DeferredVert);
	mDeferredRender.AttachShader(DeferredFrag);
	mDeferredRender.LinkProgram();

	FShader DeferredChunkVert{ L"Shaders/DeferredChunkRender.vert", GL_VERTEX_SHADER };
	mChunkRender.AttachShader(DeferredChunkVert);
	mChunkRender.AttachShader(DeferredFrag);
	mChunkRender.LinkProgram();
}

void FRenderSystem::LoadSubSystems()
{
	AddSubSystem<FDirectionalLightSystem>(*this);
	AddSubSystem<FPointLightSystem>(*this);
}

FRenderSystem::~FRenderSystem()
{
	glDeleteBuffers(1, &mBlockInfoBuffer);
	glDeleteFramebuffers(1, &mGBuffer.FBO);
	glDeleteTextures(2, mGBuffer.ColorTex);
	glDeleteTextures(1, &mGBuffer.DepthTex);
}

void FRenderSystem::Start()
{
	// Setup texture for block info
	glActiveTexture(GL_TEXTURE0 + GLTextureBindings::BlockInfo);
	glGenTextures(1, &mBlockInfoBuffer);
	glBindTexture(GL_TEXTURE_1D, mBlockInfoBuffer);
		glTexStorage1D(GL_TEXTURE_1D, 1, GL_RGBA32F, FBlockTypes::mBlockTypes.size());
		glTexSubImage1D(GL_TEXTURE_1D, 0, 0, FBlockTypes::mBlockTypes.size(), GL_RGBA, GL_FLOAT, FBlockTypes::mBlockTypes.data());
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glActiveTexture(GL_TEXTURE0);
}

uint32_t FRenderSystem::AddPostProcess(std::unique_ptr<IImageEffect> PostProcess)
{
	uint32_t ID = mPostProcesses.size();

	mPostProcesses.push_back(PostProcessRecord{ std::move(PostProcess)});
	return ID;
}

void FRenderSystem::EnablePostProcess(const uint32_t ID)
{
	ASSERT(ID < mPostProcesses.size());
	mPostProcesses[ID].IsActive = true;
}

void FRenderSystem::DisablePostProcess(const uint32_t ID)
{
	ASSERT(ID < mPostProcesses.size());
	mPostProcesses[ID].IsActive = false;
}

void FRenderSystem::SetModelTransform(const FTransform& WorldTransform)
{
	mTransformBlock.SetData(TransformBuffer::Model, WorldTransform.LocalToWorldMatrix());
}

void FRenderSystem::Update()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	ConstructGBuffer();

	for (auto& Record : mPostProcesses)
	{
		if (Record.IsActive)
			Record.Process->OnPreLightingPass();
	}

	LightingPass();

	for (auto& Record : mPostProcesses)
	{
		if (Record.IsActive)
			Record.Process->OnPostLightingPass();
	}

	// Render overlayed facilities
	glDisable(GL_BLEND);
	FDebug::Draw::GetInstance().Render();
	FDebug::GameConsole::GetInstance().Render();
	FDebug::Text::GetInstance().Render();

	// Display renderings
	mWindow.display();
}

void FRenderSystem::SetResolution(const Vector2ui& Resolution)
{
	SScreen::SetResolution(Resolution);
	mWindow.setSize(sf::Vector2u{ Resolution.x, Resolution.y });
	AllocateGBuffer(Resolution);

	mResolutionBlock.SetData(ResolutionBlock::Resolution, Resolution);
	OnResolutionChange.Invoke(Resolution);
}

void FRenderSystem::RenderGeometry()
{
	TransferViewProjectionData();

	// Render geometry
	mChunkRender.Use();
	mChunkManager.Render(*this);

	mDeferredRender.Use();
	for (auto& GameObject : GetGameObjects())
	{
		auto& Transform = GameObject->Transform;
		auto& Mesh = GameObject->GetComponent<Atlas::EComponent::MeshRenderer>();

		SetModelTransform(Transform);
		Mesh.Mesh->Mesh.Render();
	}
}

void FRenderSystem::TransferViewProjectionData()
{
	// Send view data
	mTransformBlock.SetData(TransformBuffer::View, FCamera::Main->Transform.WorldToLocalMatrix());

	// Send projection data
	FMatrix4 Projection = FCamera::Main->GetProjection();
	mTransformBlock.SetData(TransformBuffer::Projection, Projection);
	mTransformBlock.SetData(TransformBuffer::InvProjection, Projection.GetInverse());

	const float Near = Projection.M[3][2] / (Projection.M[2][2] - 1.0);
	const float Far = Projection.M[3][2] / (Projection.M[2][2] + 1.0);
	mProjectionInfoBlock.SetData(ProjectionInfoBlock::Near, Near);
	mProjectionInfoBlock.SetData(ProjectionInfoBlock::Far, Far);
}

void FRenderSystem::AllocateGBuffer(const Vector2ui& Resolution)
{
	// Delete old GBuffer
	if (mGBuffer.FBO != 0)
	{
		glDeleteFramebuffers(1, &mGBuffer.FBO);
		glDeleteTextures(2, mGBuffer.ColorTex);
		glDeleteTextures(1, &mGBuffer.DepthTex);
	}

	// Create targets with input parameters
	glGenTextures(1, mGBuffer.ColorTex);
	glBindTexture(GL_TEXTURE_2D, mGBuffer.ColorTex[0]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32UI, Resolution.x, Resolution.y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glActiveTexture(GL_TEXTURE0 + GLTextureBindings::GBuffer0);

	// Depth texture
	glGenTextures(1, &mGBuffer.DepthTex);
	glBindTexture(GL_TEXTURE_2D, mGBuffer.DepthTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, Resolution.x, Resolution.y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	float DepthBorder[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GL_CHECK(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, DepthBorder));
	glActiveTexture(GL_TEXTURE0);

	glGenFramebuffers(1, &mGBuffer.FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer.FBO);

	// Set attachments
	GL_CHECK(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mGBuffer.ColorTex[0], 0));
	GL_CHECK(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mGBuffer.DepthTex, 0));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FRenderSystem::ConstructGBuffer()
{
	// Open G-Buffer for writing and enable deferred render shader.
	const Vector2ui Resolution = SScreen::GetResolution();
	glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer.FBO);
	glViewport(0, 0, Resolution.x, Resolution.y);
	
	static const GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	static const GLuint UZeros[] = { 0, 0, 0, 0 };
	static const GLfloat FOnes[] = { 1, 1, 1, 1 };

	glDrawBuffers(1, DrawBuffers);
	glClearBufferuiv(GL_COLOR, 0, UZeros);
	glClearBufferfv(GL_DEPTH, 0, FOnes);

	// Make sure depth testing is enabled
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	RenderGeometry();

	// Close the G-Buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Resolution.x, Resolution.y);
	glDrawBuffer(GL_BACK);

	// Set GBuffers for reading
	glActiveTexture(GL_TEXTURE0 + GLTextureBindings::GBuffer0);
	glBindTexture(GL_TEXTURE_2D, mGBuffer.ColorTex[0]);

	glActiveTexture(GL_TEXTURE0 + GLTextureBindings::Depth);
	glBindTexture(GL_TEXTURE_2D, mGBuffer.DepthTex);
}

void FRenderSystem::LightingPass()
{
	// No depth testing for lighting and post-processes
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	auto& SubSystems = GetSubSystems();
	for (auto& SubSystem : SubSystems)
		SubSystem->Update();
}

//FBox FRenderSystem::GetViewBounds() const
//{
//	return mViewAABB;
//}

//void FRenderSystem::UpdateViewBounds()
//{
//	mViewAABB = FBox{};
//	const FMatrix4 CameraTransform = FCamera::Main->Transform.LocalToWorldMatrix();
//	const FMatrix4 InvProjection = FCamera::Main->GetProjection().GetInverse();
//
//	// Opengl normalized view volume corners
//	static const Vector4f NormalizedCorners[8] =
//	{
//		Vector4f{ -1, 1, 0, 1 },	// T - L - F
//		Vector4f{ -1, -1, 0, 1 },	// B - L - F
//		Vector4f{ 1, -1, 0, 1 },    // B - R - F
//		Vector4f{ 1, 1, 0, 1 },		// T - R - F
//		Vector4f{ -1, 1, 1, 1 },	// T - L - B
//		Vector4f{ -1, -1, 1, 1 },	// B - L - B
//		Vector4f{ 1, -1, 1, 1 },	// B - R - B
//		Vector4f{ 1, 1, 1, 1 }		// T - R - B
//	};
//
//	for (int32_t i = 0; i < 8; i++)
//	{
//		// Transform each vert by inv projection
//		Vector4f Vec4 = InvProjection.TransformVector(NormalizedCorners[i]);
//
//		// Divide by W component to get correct 3D coordinates in view space
//		Vector3f Point = Vector3f{ Vec4.x, Vec4.y, Vec4.z } / Vec4.w;
//
//		FMath::UpdateBounds(mViewAABB.Min, mViewAABB.Max, Point);
//	}
//
//	// Transform into world space
//	mViewAABB.TransformAABB(CameraTransform);
//}