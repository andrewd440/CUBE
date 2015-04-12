#pragma once
#include "Utils/Singleton.h"
#include "Rendering\Mesh.h"
#include "Rendering\ShaderProgram.h"
#include "Math\Vector3.h"
#include "BulletPhysics\LinearMath\btIDebugDraw.h"

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
	class Draw : public TSingleton<Draw>, public btIDebugDraw
	{
	public:
		/**
		* Default Ctor.
		*/
		Draw();

		// Dtor
		~Draw();

		void drawLine(const btVector3& From, const btVector3& To, const btVector3& Color) override;
		void drawContactPoint(const btVector3& PointOnB, const btVector3& NormalOnB, btScalar Distance, int LifeTime, const btVector3& Color) override{}
		void reportErrorWarning(const char* WarningString) override{}
		void draw3dText(const btVector3& Position, const char* Text) override{}
		void setDebugMode(int DebugMode) override { mDebugMode = DebugMode; }
		int getDebugMode() const override { return mDebugMode; }

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
		TMesh<DrawVertex> mLines;
		FShaderProgram mShader;
		int mDebugMode;
	};

}
