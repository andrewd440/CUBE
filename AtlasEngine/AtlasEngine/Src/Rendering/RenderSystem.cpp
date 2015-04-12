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

FRenderSystem::FRenderSystem(Atlas::FWorld& World, sf::Window& GameWindow, FChunkManager& ChunkManager)
	: ISystem(World)
	, mWindow(GameWindow)
	, mChunkManager(ChunkManager)
	, mDeferredRender()
	, mChunkRender()
	, mGBuffer(Vector2ui{ GameWindow.getSize().x, GameWindow.getSize().y }, GL_RGBA32UI)
	, mPostProcesses()
	, mTransformBlock(GLUniformBindings::TransformBlock, TransformBuffer::Size)
	, mResolutionBlock(GLUniformBindings::ResolutionBlock, ResolutionBlock::Size)
	, mProjectionInfoBlock(GLUniformBindings::ProjectionInfoBlock, ProjectionInfoBlock::Size)
	, mBlockInfoBuffer(0)
{
	SetResolution(Vector2ui{ GameWindow.getSize().x, GameWindow.getSize().y });
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0, 0, 0, 1.0f);

	LoadShaders();
	LoadSubSystems();

	AddComponentType<Atlas::EComponent::MeshRenderer>();

	// Setup shader storage block for block info
	glGenBuffers(1, &mBlockInfoBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mBlockInfoBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(FBlock::Colors), FBlock::Colors, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, GLUniformBindings::BlockInfo, mBlockInfoBuffer);
}

void FRenderSystem::LoadShaders()
{
	// Load all main rendering shaders
	SShaderHolder::Load("FullScreenQuad.vert", L"Shaders/FullScreenQuad.vert", GL_VERTEX_SHADER);
	SShaderHolder::Load("DeferredLightingCommon.frag", L"Shaders/DeferredLightingCommon.frag", GL_FRAGMENT_SHADER);
	
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
}

uint32_t FRenderSystem::AddPostProcess(std::unique_ptr<IRenderPostProcess> PostProcess)
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
	
	//UpdateViewBounds();
	ConstructGBuffer();

	mGBuffer.StartRead();

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

	mGBuffer.EndRead();

	/////////////////////////////
	// Debugging text ////////////////////////////

	static const vec4 White{ { 1, 1, 1, 1 } };
	static const vec4 None{{ 1, 1, 1, 0 }};
	static markup_t TextMarkup{ "Vera.ttf", 16, 0, 0, 0.0f, 0.0f, 2.0f, White, None, 0, White, 0, White, 0, White, 0 };

	// Debug Print
	const Vector3f CameraPosition = FCamera::Main->Transform.GetWorldPosition();
	auto& DebugText = FDebug::Text::GetInstance();
	wchar_t String[250];
	const Vector3f Direction = FCamera::Main->Transform.GetRotation() * -Vector3f::Forward;
	swprintf_s(String, L"FPS: %.2f   Position: %.1f %.1f %.1f Direction: %.1f %.1f %.1f", 1.0f / STime::GetDeltaTime(), CameraPosition.x, CameraPosition.y, CameraPosition.z, Direction.x, Direction.y, Direction.z);
	DebugText.AddText(std::wstring{ String }, Vector2i(50, SScreen::GetResolution().y - 50), TextMarkup);

	swprintf_s(String, L"Chunks used: %d", FChunk::ChunkAllocator.Size());
	DebugText.AddText(std::wstring{ String }, Vector2i(50, SScreen::GetResolution().y - 100), TextMarkup);

	Vector3i ChunkPosition = Vector3i(CameraPosition.x / FChunk::CHUNK_SIZE, CameraPosition.y / FChunk::CHUNK_SIZE, CameraPosition.z / FChunk::CHUNK_SIZE);
	swprintf_s(String, L"Chunk Position: %d %d %d", ChunkPosition.x, ChunkPosition.y, ChunkPosition.z);
	DebugText.AddText(std::wstring{ String }, Vector2i(50, SScreen::GetResolution().y - 150), TextMarkup);

	///////////////////////////////////////////////
	///////////////////////////////

	// Render overlayed facilities
	glDisable(GL_BLEND);
	FDebug::Draw::GetInstance().Render();
	FDebug::GameConsole::GetInstance().Render();
	DebugText.Render();

	// Display renderings
	mWindow.display();
}

void FRenderSystem::SetResolution(const Vector2ui& Resolution)
{
	SScreen::SetResolution(Resolution);
	mWindow.setSize(sf::Vector2u{ Resolution.x, Resolution.y });
	mGBuffer = GBuffer{ Resolution, GL_RGBA32UI };

	mResolutionBlock.SetData(ResolutionBlock::Resolution, Resolution);
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

void FRenderSystem::ConstructGBuffer()
{
	// Open G-Buffer for writing and enable deferred render shader.
	mGBuffer.StartWrite();

	// Make sure depth testing is enabled
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	RenderGeometry();

	// Close the G-Buffer
	mGBuffer.EndWrite();
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