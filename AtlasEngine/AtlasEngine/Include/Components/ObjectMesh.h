#pragma once

#include "Rendering\Mesh.h"
#include "Atlas\ComponentTypes.h"
#include "Rendering\GLBindings.h"
#include "ResourceHolder.h"

struct MeshVertex
{
	Vector4f Position;
	Vector3f Normal;
	Vector3f Color;
};

template <>
struct VertexTraits::Attribute_Count < MeshVertex >
{
	const static uint32_t Count = 3;
};

const uint32_t VertexTraits::GL_Attribute<MeshVertex>::Position[] = { GLAttributePosition::Position, GLAttributePosition::Normal, GLAttributePosition::Color };
const uint32_t VertexTraits::GL_Attribute<MeshVertex>::ElementCount[] = { 4, 3, 3 };
const uint32_t VertexTraits::GL_Attribute<MeshVertex>::Type[] = { GL_FLOAT, GL_FLOAT, GL_FLOAT };
const bool VertexTraits::GL_Attribute<MeshVertex>::Normalized[] = { GL_FALSE, GL_FALSE, GL_FALSE };
const uint32_t VertexTraits::GL_Attribute<MeshVertex>::Offset[] = { 0, 16, 28 };


class FMeshRenderer;

/**
* A mesh that can be rendered with a gameobject.
* MeshRenderer components are linked with a single
* ObjectMesh and ObjectMeshs can be linked to many
* MeshRenderers.
*/
struct FObjectMesh
{
	TMesh<MeshVertex> Mesh;
	std::vector<FMeshRenderer*> Renderers;
};

using SMeshHolder = TResourceHolder<FObjectMesh>;