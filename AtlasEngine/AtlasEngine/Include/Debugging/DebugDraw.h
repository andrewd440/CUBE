#pragma once
#include "Singleton.h"
#include "Rendering\Mesh.h"
#include "Rendering\ShaderProgram.h"
#include "Math\Vector3.h"

#include <cstdlib>
#include <vector>

class FCamera;

namespace FDebug
{	
	// Vertex type used for debug drawing
	// Debug draws with GL_LINE
	struct DrawVertex
	{
		Vector3f Position;
		Vector3f Color;
	};

	/**
	* Debug drawing facility. Used to draw useful debug
	* shapes in a scene.
	*/
	class Draw : public TSingleton<Draw>
	{
	public:
		/**
		* Default Ctor.
		*/
		Draw();

		// Dtor
		~Draw();

		/**
		* Draw the frustum a camera object.
		* @param Camera - The camera to extract the frustum from.
		* @param Color - The color to draw the frustum.
		* @param Lifetime - The length, in seconds, to draw the frustum. (default is one frame)
		**/
		void DrawFrustum(FCamera& Camera, const Vector3f& Color, const float Lifetime = 0.0f);

		/**
		* Draw a box.
		* @param Center - The center point of the box.
		* @param Dimensions - The width, height, and depth of the box. (x y z)
		* @param Color - The color to draw the box.
		* @param Lifetime - The length, in seconds, to draw the box. (default is one frame)
		*/
		void DrawBox(const Vector3f& Center, const Vector3f& Dimensions, const Vector3f& Color, const float Lifetime = 0.0f);

		/**
		* Renders all queued debug draw commands.
		*/
		void Render();

	private:
		struct ObjectData
		{
			ObjectData() = default;
			ObjectData(ObjectData&& Other)
				: Lifetime(Other.Lifetime)
				, Mesh(std::move(Other.Mesh))
			{
			}
			float Lifetime;
			TMesh<DrawVertex> Mesh;
		};

	private:
		std::vector<ObjectData> mObjects;
		FShaderProgram mShader;
	};

}
