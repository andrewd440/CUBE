#include "Debugging\DebugDraw.h"
#include "ResourceHolder.h"
#include "STime.h"
#include "Math\Transform.h"
#include "Rendering\UniformBlockStandard.h"
#include "Rendering\Camera.h"

namespace FDebug
{
	Draw::Draw()
		: mObjects()
		, mLines(GL_DYNAMIC_DRAW)
		, mShader()
		, mDebugMode(btIDebugDraw::DBG_DrawWireframe)
	{
		FShader VertexShader{ L"Shaders/DebugDraw.vert", GL_VERTEX_SHADER };
		FShader FragShader{ L"Shaders/DebugDraw.frag", GL_FRAGMENT_SHADER };
		mShader.AttachShader(VertexShader);
		mShader.AttachShader(FragShader);
		mShader.LinkProgram();
	}

	Draw::~Draw()
	{
	}

	void Draw::drawLine(const btVector3& From, const btVector3& To, const btVector3& Color)
	{
		const float Verts[12] = 
		{
			From.x(), From.y(), From.z(), Color.x(), Color.y(), Color.z(),
			To.x(), To.y(), To.z(), Color.x(), Color.y(), Color.z()
		};

		const uint32_t BaseIndex = mLines.GetIndexCount();
		const uint32_t Indices[2] = { BaseIndex + 0, BaseIndex + 1 };
		mLines.AddVertex((DrawVertex*)Verts, 2);
		mLines.AddIndices(Indices, 2);
	}

	void Draw::DrawFrustum(FCamera& Camera, const Vector3f& Color, const float Lifetime)
	{
		const FMatrix4 CameraTransform = Camera.Transform.LocalToWorldMatrix();
		const FMatrix4 InvProjection = Camera.GetProjection().GetInverse();

		// Opengl normalized view volume corners
		static const Vector4f NormalizedCorners[8] =
		{
			Vector4f{ -1, 1, 0, 1 },	// T - L - F
			Vector4f{ -1, -1, 0, 1 },	// B - L - F
			Vector4f{ 1, -1, 0, 1 },    // B - R - F
			Vector4f{ 1, 1, 0, 1 },		// T - R - F
			Vector4f{ -1, 1, 1, 1 },	// T - L - B
			Vector4f{ -1, -1, 1, 1 },	// B - L - B
			Vector4f{ 1, -1, 1, 1 },	// B - R - B
			Vector4f{ 1, 1, 1, 1 }		// T - R - B
		};

		// Create a new object, then modify it's data
		ObjectData Frustum;
		Frustum.Lifetime = Lifetime;

		auto& Mesh = Frustum.Mesh;
		Mesh.SetUsageMode(GL_DYNAMIC_DRAW);

		DrawVertex Vertices[8];
		for (int32_t i = 0; i < 8; i++)
		{
			// Transform each vert by inv projection
			Vector4f Vec4 = InvProjection.TransformVector(NormalizedCorners[i]);

			// Divide by W component to get correct 3D coordinates in view space
			Vertices[i].Position = Vector3f{ Vec4.x / Vec4.w, Vec4.y / Vec4.w, Vec4.z / Vec4.w };

			// Transform into world space
			Vertices[i].Position = CameraTransform.TransformPosition(Vertices[i].Position);
			Vertices[i].Color = Color;
		}

		// Indices to connect each vertex with a line strip
		static const uint32_t Indices[16] =
		{
			0, 1, 2, 3, 0, 4, 5, 1, 2, 6, 5, 6, 7, 4, 7, 3
		};

		Mesh.AddVertex(Vertices, 8);
		Mesh.AddIndices(Indices, 16);
		Mesh.Activate();

		mObjects.push_back(std::move(Frustum));
	}

	void Draw::DrawBox(const Vector3f& Center, const Vector3f& Dimensions, const Vector3f& Color, const float Lifetime)
	{
		const Vector3f HalfWidths = Dimensions / 2.0f;
		const DrawVertex Vertices[8] =
		{
			{ Center + Vector3f{ -HalfWidths.x, HalfWidths.y, HalfWidths.z }, Color },
			{ Center + Vector3f{ -HalfWidths.x, -HalfWidths.y, HalfWidths.z }, Color },
			{ Center + Vector3f{ HalfWidths.x, -HalfWidths.y, HalfWidths.z }, Color },
			{ Center + Vector3f{ HalfWidths.x, HalfWidths.y, HalfWidths.z }, Color },
			{ Center + Vector3f{ -HalfWidths.x, HalfWidths.y, -HalfWidths.z }, Color },
			{ Center + Vector3f{ -HalfWidths.x, -HalfWidths.y, -HalfWidths.z }, Color },
			{ Center + Vector3f{ HalfWidths.x, -HalfWidths.y, -HalfWidths.z }, Color },
			{ Center + Vector3f{ HalfWidths.x, HalfWidths.y, -HalfWidths.z }, Color }
		};

		static const uint32_t Indices[16] =
		{
			0, 1, 2, 3, 0, 4, 5, 1, 2, 6, 5, 6, 7, 4, 7, 3
		};

		// Create a new object, then modify it's data
		ObjectData Box;

		Box.Lifetime = Lifetime;
		auto& Mesh = Box.Mesh;
		Mesh.SetUsageMode(GL_DYNAMIC_DRAW);

		// Add vertex and index data
		Mesh.AddVertex(Vertices, 8);
		Mesh.AddIndices(Indices, 16);
		Mesh.Activate();

		mObjects.push_back(std::move(Box));
	}

	void Draw::Render()
	{
		mShader.Use();
		const float DeltaTime = STime::GetDeltaTime();
		for (auto Itr = mObjects.begin(); Itr != mObjects.end();)
		{
			Itr->Mesh.Render(GL_LINE_STRIP);

			// Subtract each objects lifetime and remove if dead
			Itr->Lifetime -= DeltaTime;
			if (Itr->Lifetime <= 0)
				Itr = mObjects.erase(Itr);
			else
				Itr++;
		}

		mLines.Activate();
		mLines.Render(GL_LINES);
		mLines.ClearData();
		glUseProgram(0);
	}
}
