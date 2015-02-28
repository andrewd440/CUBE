#pragma once
#include "Atlas\System.h"
#include "SFML\Window\Window.hpp"
#include "Rendering\UniformBlockStandard.h"
#include "Rendering\ShaderProgram.h"
#include "Rendering\GBuffer.h"
#include "Math\Box.h"

class FChunkManager;

class FRenderSystem : public Atlas::ISystem
{
public:
	/**
	* Constructs a rendering system.
 	* @param World - The world this system is part of.
	* @param GameWindow - The window to render to.
	* @param ChunkManager - The manager of chunks?
	*/
	FRenderSystem(Atlas::FWorld& World, sf::Window& GameWindow, FChunkManager& ChunkManager);
	~FRenderSystem();

	/**
	* Sets the current model transform for render calls. This is combined
	* with the current camera transform to produce the modelview transform.
	*/
	void SetModelTransform(const FTransform& WorldTransform);

	/**
	* Renders the scene.
	*/
	void Update() override;

	/**
	* Sends draw calls to all the currently visible geometry with respect to
 	* the main camera.
	*/
	void RenderGeometry();

	/**
	* Get the current world space coordinates of each of the 8 corners
	* of the view volume.
	*/
	FBox GetViewBounds() const;

private:
	void LoadShaders();
	void LoadSubSystems();

	void ConstructGBuffer();
	void LightingPass();

	void UpdateViewBounds();

private:
	// Each type of subsystem used by the rendering system
	struct SubSystems
	{
		enum Type : uint32_t
		{
			DirectionalLight,
			PointLight,
			Count
		};
	};

private:
	sf::Window& mWindow;
	FChunkManager& mChunkManager;
	EZGL::FUniformBlock mTransformBuffer;
	FShaderProgram mDeferredRender;
	GBuffer mGBuffer;
	FBox mViewAABB;
};