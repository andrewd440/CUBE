#pragma once

#include "Atlas\Component.h"
#include "ObjectMesh.h"

/**
* A component used to link a triangle mesh with a 
* gameobject. Each FMeshRenderer is linked with a FObjectMesh
* that has been loaded into the mesh resource holder.
*/
class FMeshRenderer : public Atlas::IComponent
{
public:
	void LinkToMesh(const char* MeshName);

private:
	friend class FRenderSystem;
	FObjectMesh* Mesh;
};

template <>
struct Atlas::ComponentTraits::Object<Atlas::EComponent::MeshRenderer>
{
	using Type = FMeshRenderer;
};